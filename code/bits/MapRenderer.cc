#include "MapRenderer.h"

#include <gf2/core/Log.h>
#include <gf2/core/Mat3.h>
#include <gf2/core/TiledMap.h>
#include <gf2/graphics/RenderObject.h>

#include "FloorRenderer.h"
#include "WorldState.h"

namespace akgr {

  namespace {

    int layer_priority_from_plane(PlaneType plane_type, Plane plane) {
      switch (plane_type) {
        case PlaneType::Object:
          switch (plane) {
            case Plane::High:
              return +15;
            case Plane::Low:
              return -5;
            case Plane::Ground:
              assert(false);
              break;
          }
          break;
        case PlaneType::Tile:
          switch (plane) {
            case Plane::High:
              return +10;
            case Plane::Low:
              return -10;
            case Plane::Ground:
              return -15;
          }
          break;
      }

      assert(false);
      return 0;
    }

    const char* object_layer_name_from_plane(Plane plane) {
      switch (plane) {
        case Plane::High:
          return "high_sprites";
        case Plane::Low:
          return "low_sprites";
        case Plane::Ground:
          assert(false);
          break;
      }

      assert(false);
      return "";
    }

    const char* tile_layer_name_from_plane(Plane plane) {
      switch (plane) {
        case Plane::High:
          return "high_tiles";
        case Plane::Low:
          return "low_tiles";
        case Plane::Ground:
          return "ground_tiles";
      }

      assert(false);
      return "";
    }

    std::map<int, gf::MapLayerStructure> compute_layers(const gf::TiledMap* tiled_map, PlaneType type, Plane plane)
    {
      std::map<int, gf::MapLayerStructure> floor_layers;

      for (const auto& layer : tiled_map->layers) {
        assert(layer.type == gf::MapLayerType::Group);
        const auto& group_layer = tiled_map->group_layers[layer.layer_index];
        assert(group_layer.layer.properties_index != gf::NoIndex);
        const auto& group_properties = tiled_map->properties[group_layer.layer.properties_index];
        assert(group_properties.has_property("floor"));
        const auto floor = static_cast<int32_t>(group_properties("floor").as_int());

        for (const auto& sub_layer : group_layer.sub_layers) {
          if (sub_layer.type == gf::MapLayerType::Tile && type == PlaneType::Tile) {
            const auto& tile_layer = tiled_map->tile_layers[sub_layer.layer_index];
            assert(tile_layer.layer.properties_index != gf::NoIndex);
            const auto& layer_properties = tiled_map->properties[tile_layer.layer.properties_index];
            assert(layer_properties.has_property("kind"));
            const std::string& kind = layer_properties("kind").as_string();

            if (kind == tile_layer_name_from_plane(plane)) {
              floor_layers.emplace(floor, sub_layer);
            }
          }

          if (sub_layer.type == gf::MapLayerType::Object && type == PlaneType::Object) {
            const auto& object_layer = tiled_map->object_layers[sub_layer.layer_index];
            assert(object_layer.layer.properties_index != gf::NoIndex);
            const auto& layer_properties = tiled_map->properties[object_layer.layer.properties_index];
            assert(layer_properties.has_property("kind"));
            const std::string& kind = layer_properties("kind").as_string();

            if (kind == object_layer_name_from_plane(plane)) {
              floor_layers.emplace(floor, sub_layer);
            }
          }
        }
      }

      return floor_layers;
    }

  }

  MapRenderer::MapRenderer(PlaneType plane_type, Plane plane, WorldState* world_state, gf::RichMapRenderer* rich_map_renderer)
  : FloorRenderer(layer_priority_from_plane(plane_type, plane))
  , m_plane_type(plane_type)
  , m_plane(plane)
  , m_world_state(world_state)
  , m_rich_map_renderer(rich_map_renderer)
  , m_layers(compute_layers(rich_map_renderer->tiled_map(), plane_type, plane))
  {
  }

  void MapRenderer::render_floor(gf::RenderRecorder& recorder, int32_t floor)
  {
    using namespace std::literals;

    auto iterator = m_layers.find(floor);

    if (iterator == m_layers.end()) {
      return;
    }

    const gf::MapLayerStructure structure[] = { iterator->second };
    gf::Vec2I position = m_rich_map_renderer->compute_position(m_world_state->hero.spot.location);
    auto geometries = m_rich_map_renderer->select_geometry(position, structure);

    gf::RenderObject object;
    object.priority = priority();
    object.transform = gf::Identity3F;

    for (auto& geometry : geometries) {
      object.geometry = geometry;
      recorder.record(object);
    }
  }

}
