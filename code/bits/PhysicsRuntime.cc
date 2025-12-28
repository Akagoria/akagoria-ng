#include "PhysicsRuntime.h"

#include <cassert>
#include <cstdint>
#include <cstdlib>

#include <gf2/core/Geometry.h>
#include <gf2/core/Log.h>
#include <gf2/core/Mat3.h>
#include <gf2/core/Polygon.h>
#include <gf2/core/StringUtils.h>

#include <gf2/physics/PhysicsChain.h>

#include "PhysicsUtils.h"
#include "WorldData.h"

namespace akgr {

  namespace {

    // hero

    constexpr float CharacterMass = 1.0f;
    constexpr float CharacterRadius = 20.0f;

    // map

    constexpr float ShapeRadius = 1.0f;

    constexpr int EllipseSegmentCount = 10;
    constexpr float EllipseSegmentLength = 40.0f;

    gf::PhysicsShape create_convex_shape_from_object(gf::PhysicsBody* body, const gf::PhysicsShapeData& data, const gf::MapObject& object)
    {
      switch (object.type) {
        case gf::MapObjectType::Polygon:
          {
            const auto polygon = std::get<std::vector<gf::Vec2F>>(object.feature);
            assert(gf::is_convex(polygon));
            assert(polygon.size() < B2_MAX_POLYGON_VERTICES);
            return gf::PhysicsShape::create_polygon(body, data, polygon, ShapeRadius);
          }
        case gf::MapObjectType::Rectangle:
          {
            const gf::Vec2F size = std::get<gf::Vec2F>(object.feature);
            return gf::PhysicsShape::create_box(body, data, size / 2.0f);
          }
        default:
          assert(false);
          break;
      }

      return {};
    }

    void create_collision_shapes_from_object(gf::PhysicsBody* body, const gf::PhysicsShapeData& data, const gf::MapObject& object)
    {
      switch (object.type) {
        case gf::MapObjectType::Rectangle:
          {
            gf::Vec2F size = std::get<gf::Vec2F>(object.feature);

            gf::PhysicsShape shape = gf::PhysicsShape::create_box(body, data, gf::RectF::from_position_size(object.location, size));
            shape.release();
          }
          break;
        case gf::MapObjectType::Polyline:
        case gf::MapObjectType::Polygon:
          {
            auto polyline = std::get<std::vector<gf::Vec2F>>(object.feature);

            for (gf::Vec2F& point : polyline) {
              point += object.location;
            }

            gf::PhysicsChainData chain_data;
            chain_data.points = polyline;
            chain_data.loop = (object.type == gf::MapObjectType::Polygon);
            chain_data.filter = data.filter;

            gf::PhysicsChain chain_ccw(body, chain_data);
            chain_ccw.release();

            std::reverse(polyline.begin(), polyline.end());
            chain_data.points = polyline;

            gf::PhysicsChain chain_cw(body, chain_data);
            chain_cw.release();
          }
          break;
        case gf::MapObjectType::Ellipse:
          {
            const gf::Vec2F size = std::get<gf::Vec2F>(object.feature);

            if (gf::almost_equals(size.w, size.h)) {
              const float radius = (size.w + size.h) / 2.0f;

              gf::PhysicsShape shape = gf::PhysicsShape::create_circle(body, data, gf::CircF::from_center_radius(object.location, radius));
              shape.release();
            } else {
              std::vector<gf::Vec2F> polyline;

              const float perimeter_approx = gf::Pi * std::sqrt(2 * (gf::square(size.w) + gf::square(size.h)));
              const int count = std::max(EllipseSegmentCount, static_cast<int>(perimeter_approx / EllipseSegmentLength));
              polyline.reserve(count);

              for (int i = 0; i < count; ++i) {
                polyline.push_back(object.location + size / 2 + size / 2 * gf::unit(2.0f * gf::Pi * float(i) / float(count)));
              }

              gf::PhysicsChainData chain_data;
              chain_data.points = polyline;
              chain_data.loop = true;
              chain_data.filter = data.filter;

              gf::PhysicsChain chain_ccw(body, chain_data);
              chain_ccw.release();

              std::reverse(polyline.begin(), polyline.end());
              chain_data.points = polyline;

              gf::PhysicsChain chain_cw(body, chain_data);
              chain_cw.release();

            }
          }
          break;
        default:
          assert(false);
          break;
      }
    }
  }

  /*
   * PhysicsDetail
   */

  void CharacterPhysics::set_spot(Spot spot)
  {
    const gf::PhysicsRotation rotation = body.rotation();
    body.set_transform(spot.location, rotation);
    shape.set_filter(filter_from_floor(spot.floor));
  }

  /*
   * PhysicsRuntime
   */

  void PhysicsRuntime::bind(const WorldData& data, const WorldState& state)
  {
    gf::PhysicsWorldData world_data;
    world_data.gravity = { 0.0f, 0.0f };

    world = { world_data };

    // hero

    bind_hero(state.hero);

    // characters

    bind_characters(state.characters);

    // map

    const gf::TiledMap& map = data.map;

    for (const gf::MapLayerStructure& layer : map.layers) {
      assert(layer.type == gf::MapLayerType::Group);

      const gf::MapGroupLayer& group_layer = map.group_layers[layer.layer_index];
      const gf::PropertyMap& group_properties = map.properties[group_layer.layer.properties_index];

      const gf::Property& floor_property = group_properties("floor");
      assert(floor_property.is_int());
      const auto floor = static_cast<int32_t>(floor_property.as_int());

      std::vector<gf::SegmentI> fences;

      for (const gf::MapLayerStructure& sub_layer : group_layer.sub_layers) {
        switch (sub_layer.type) {
          case gf::MapLayerType::Object:
            compute_object_layer(sub_layer, floor, map);
            break;
          case gf::MapLayerType::Tile:
            compute_tile_layer(sub_layer, map, fences);
            break;
          case gf::MapLayerType::Group:
            assert(false);
            break;
        }
      }

      extract_collisions(fences, floor);
    }
  }

  CharacterPhysics PhysicsRuntime::create_character(Spot spot, float rotation)
  {
    CharacterPhysics detail;

    gf::PhysicsBodyData body_data;
    body_data.type = gf::PhysicsBodyType::Dynamic;
    body_data.location = spot.location;
    body_data.rotation = rotation;
    body_data.motion_locks.angular_z = true;

    detail.body = { &world, body_data };

    gf::PhysicsShapeData shape_data;
    shape_data.density = CharacterMass;
    shape_data.enable_sensor_events = true;
    shape_data.filter = filter_from_floor(spot.floor);

    detail.shape = gf::PhysicsShape::create_circle(&detail.body, shape_data, gf::CircF::from_radius(CharacterRadius));

    return detail;
  }

  void PhysicsRuntime::bind_hero(const HeroState& state)
  {
    hero = create_character(state.spot, state.rotation);
  }

  void PhysicsRuntime::bind_characters(const std::vector<CharacterState>& states)
  {
    for (const CharacterState& character : states) {
      characters.push_back(create_character(character.spot, character.rotation));
    }
  }

  void PhysicsRuntime::compute_tile_layer(const gf::MapLayerStructure& layer, const gf::TiledMap& map, std::vector<gf::SegmentI>& fences)
  {
    assert(layer.type == gf::MapLayerType::Tile);

    const auto& tile_layer = map.tile_layers[layer.layer_index];

    for (auto position : tile_layer.tiles.position_range()) {
      const auto& cell = tile_layer.tiles(position);

      if (cell.gid == 0) {
        continue;
      }

      const gf::MapTileset* tileset = map.tileset_from_gid(cell.gid);
      const uint32_t gid = cell.gid - tileset->first_gid;
      const gf::MapTilesetTile* tile = tileset->tile(gid);
      assert(tile != nullptr);

      if (tile->properties_index == gf::NoIndex) {
        continue;
      }

      const auto& properties = map.properties[tile->properties_index];

      if (!properties.has_property("fence_count")) {
        continue;
      }

      const int64_t fence_count = properties("fence_count").as_int();

      for (int64_t i = 0; i < fence_count; ++i) {
        const std::string& fence = properties("fence" + std::to_string(i)).as_string();
        auto coords = gf::split_string(fence, ",");
        assert(coords.size() == 4);

        gf::SegmentI segment;
        segment.p0 = gf::vec(std::atoi(coords[0].data()), std::atoi(coords[1].data()));
        segment.p1 = gf::vec(std::atoi(coords[2].data()), std::atoi(coords[3].data()));

        if (cell.flip.test(gf::CellFlip::Diagonally)) {
          std::swap(segment.p0.x, segment.p0.y);
          std::swap(segment.p1.x, segment.p1.y);
        }

        if (cell.flip.test(gf::CellFlip::Horizontally)) {
          segment.p0.x = map.tile_size.x - segment.p0.x;
          segment.p1.x = map.tile_size.x - segment.p1.x;
        }

        if (cell.flip.test(gf::CellFlip::Vertically)) {
          segment.p0.y = map.tile_size.y - segment.p0.y;
          segment.p1.y = map.tile_size.y - segment.p1.y;
        }

        segment.p0 += position * map.tile_size;
        segment.p1 += position * map.tile_size;

        fences.push_back(segment);
      }
    }
  }

  void PhysicsRuntime::extract_collisions(const std::vector<gf::SegmentI>& fences, int32_t floor)
  {
    gf::PhysicsBodyData body_data;
    body_data.type = gf::PhysicsBodyType::Static;

    gf::PhysicsBody body(&world, body_data);

    std::vector<gf::Polyline> collisions = gf::compute_lines(fences);

    for (gf::Polyline collision : collisions) {
      gf::PhysicsChainData chain_data;
      chain_data.points = collision.points;
      chain_data.loop = (collision.type == gf::PolylineType::Loop);
      chain_data.filter = filter_from_floor(floor);

      gf::PhysicsChain chain_ccw(&body, chain_data);
      chain_ccw.release();

      std::reverse(collision.points.begin(), collision.points.end());
      chain_data.points = collision.points;

      gf::PhysicsChain chain_cw(&body, chain_data);
      chain_cw.release();
    }

    body.release();
  }

  void PhysicsRuntime::compute_object_layer(const gf::MapLayerStructure& layer, int32_t floor, const gf::TiledMap& map)
  {
    assert(layer.type == gf::MapLayerType::Object);

    const auto& object_layer = map.object_layers[layer.layer_index];
    const std::string& type = object_layer.layer.type;

    if (type == "zones") {
      for (const auto& object : object_layer.objects) {
        extract_zone(object, floor, map);
      }

      return;
    }

    if (type == "low_sprites" || type == "high_sprites") {
      for (const auto& object : object_layer.objects) {
        extract_sprites(object, floor, map);
      }

      return;
    }
  }

  void PhysicsRuntime::extract_zone(const gf::MapObject& object, int32_t floor, const gf::TiledMap& map)
  {
    // 1. prepare zone data

    const gf::PropertyMap& object_properties = map.properties[object.properties_index];

    if (!object_properties.has_property("message")) {
      gf::Log::error("Missing message in a zone object: '{}'", object.name);
      return;
    }

    const std::string& message = object_properties("message").as_string();

    std::set<gf::Id> requirements;

    if (object_properties.has_property("requirements")) {
      std::string requirements_string = object_properties("requirements").as_string();
      auto requirement_list = gf::split_string(requirements_string, ",");

      for (auto requirement : requirement_list) {
        if (requirement.empty()) {
          continue;
        }

        requirements.insert(gf::hash_string(requirement));
      }
    }

    // 2. create shape

    gf::PhysicsBodyData body_data;
    body_data.type = gf::PhysicsBodyType::Static;
    body_data.location = object.location;
    body_data.rotation = gf::degrees_to_radians(object.rotation);

    gf::PhysicsBody body(&world, body_data);

    gf::PhysicsShapeData shape_data;
    shape_data.filter = filter_from_floor(floor);
    shape_data.enable_sensor_events = true;
    shape_data.sensor = true;

    gf::PhysicsShape shape = create_convex_shape_from_object(&body, shape_data, object);

    // 3. add zone to known zones

    Zone zone = { object.name, message, std::move(requirements) };
    zones.emplace(shape.id(), std::move(zone));

    shape.release();
    body.release();
  }

  void PhysicsRuntime::extract_sprites(const gf::MapObject& object, int32_t floor, const gf::TiledMap& map)
  {
    assert(object.type == gf::MapObjectType::Tile);
    const gf::MapTile tile = std::get<gf::MapTile>(object.feature);

    const gf::MapTileset* tileset = map.tileset_from_gid(tile.gid);
    const uint32_t id = tile.gid - tileset->first_gid;
    const gf::MapTilesetTile& tileset_tile = tileset->tiles[id];

    if (!tileset_tile.objects) {
      return;
    }

    const gf::MapObjectLayer& object_layer = map.object_layers[*tileset_tile.objects];

    const gf::Vec2F object_size = tileset->tile_size;
    const gf::Vec2F object_center = object.location - gf::diry(object_size.h);
    const gf::Vec2F bottom_left = object.location;
    const gf::Vec2F location = gf::transform_point(gf::rotation(gf::degrees_to_radians(object.rotation), bottom_left), object_center);

    gf::PhysicsBodyData body_data;
    body_data.type = gf::PhysicsBodyType::Static;
    body_data.location = location;
    body_data.rotation = gf::degrees_to_radians(object.rotation);

    gf::PhysicsBody body(&world, body_data);

    gf::PhysicsShapeData shape_data;
    shape_data.filter = filter_from_floor(floor);

    for (const gf::MapObject& physics_object : object_layer.objects) {
      create_collision_shapes_from_object(&body, shape_data, physics_object);
    }
  }
}
