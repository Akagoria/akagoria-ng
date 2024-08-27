#include "PhysicsRuntime.h"

#include <cassert>

#include <gf2/core/Log.h>
#include <gf2/core/Mat3.h>
#include <gf2/core/Polygon.h>
#include <gf2/core/StringUtils.h>

#include <gf2/physics/PhysicsArbiter.h>
#include <gf2/physics/PhysicsShape.h>
#include <gf2/physics/PhysicsShapeEx.h>
#include <gf2/physics/PhysicsWorld.h>

#include "WorldData.h"

namespace akgr {

  namespace {
    constexpr uintptr_t ZoneCollisionType = 42;

    constexpr float ShapeRadius = 1.0f;

    constexpr int EllipseSegmentCount = 10;
    constexpr float EllipseSegmentLength = 50.0f;

    constexpr unsigned bits_from_floor(int32_t floor) {
      if (floor < -6 || floor > 7) {
        gf::Log::debug("Unknown floor: {}", floor);
        return 0;
      }

      assert(-6 <= floor && floor <= 7);
      return static_cast<unsigned>(1 << (floor + 8));
    }

    gf::PhysicsShapeFilter filter_from_floor(int32_t floor)
    {
      gf::PhysicsShapeFilter filter = {};
      filter.categories = filter.mask = bits_from_floor(floor);
      return filter;
    }

    gf::PhysicsShape object_to_convex_shape(gf::PhysicsBody* body, const gf::MapObject& object)
    {
      switch (object.type) {
        case gf::MapObjectType::Polygon:
          {
            auto polygon = std::get<std::vector<gf::Vec2F>>(object.feature);
            assert(gf::is_convex(polygon));
            return gf::PhysicsShape::make_polygon(body, polygon, gf::Identity3F, ShapeRadius);
          }
        case gf::MapObjectType::Rectangle:
          {
            gf::Vec2F size = std::get<gf::Vec2F>(object.feature);
            return gf::PhysicsShape::make_box(body, gf::RectF::from_size(size), ShapeRadius);
          }
        default:
          assert(false);
          break;
      }

      return {};
    }

    std::vector<gf::PhysicsShape> object_to_collision_shapes(gf::PhysicsBody* body, const gf::MapObject& object)
    {
      switch (object.type) {
        case gf::MapObjectType::Rectangle:
          {
            gf::Vec2F size = std::get<gf::Vec2F>(object.feature);
            return { gf::PhysicsShape::make_box(body, gf::RectF::from_size(size), ShapeRadius) };
          }
        case gf::MapObjectType::Polyline:
        case gf::MapObjectType::Polygon:
          {
            auto polyline = std::get<std::vector<gf::Vec2F>>(object.feature);
            auto type = (object.type == gf::MapObjectType::Polygon) ? gf::PolylineType::Loop : gf::PolylineType::Chain;
            return gf::make_polyline_shapes(body, polyline, ShapeRadius, type);
          }
        case gf::MapObjectType::Ellipse:
          {
            const gf::Vec2F size = std::get<gf::Vec2F>(object.feature);

            if (gf::almost_equals(size.w, size.h)) {
              const float radius = (size.w + size.h) / 2.0f;
              return { gf::PhysicsShape::make_circle(body, radius, { 0.0f, 0.0f }) };
            }

            const gf::Vec2F center = size / 2.0f;
            std::vector<gf::Vec2F> polyline;

            const float perimeter_approx = gf::Pi * std::sqrt(2 * (gf::square(size.w) + gf::square(size.h)));
            const int count = std::max(EllipseSegmentCount, static_cast<int>(perimeter_approx / EllipseSegmentLength));
            polyline.reserve(count);

            for (int i = 0; i < count; ++i) {
              polyline.push_back(size / 2 * gf::unit(2.0f * gf::Pi * float(i) / float(count)) + center);
            }

            return gf::make_polyline_shapes(body, polyline, ShapeRadius, gf::PolylineType::Loop);
          }
        default:
          assert(false);
          break;
      }

      return {};
    }
  }

  /*
   * ZoneHandler
   */

  void ZoneHandler::add_zone(gf::PhysicsId id, std::string name, std::string message, std::set<gf::Id> requirements)
  {
    Zone zone = { std::move(name), std::move(message), std::move(requirements) };
    m_zones.emplace(id, std::move(zone));
  }


  bool ZoneHandler::begin(gf::PhysicsArbiter arbiter, gf::PhysicsWorld world)
  {
    return true;
  }

  /*
   * PhysicsRuntime
   */

  void PhysicsRuntime::bind(const WorldData& data)
  {
    const auto& map = data.map;

    for (const auto& layer : map.layers) {
      assert(layer.type == gf::MapLayerType::Group);

      const auto& group_layer = map.group_layers[layer.layer_index];
      const auto& group_properties = map.properties[group_layer.layer.properties_index];

      const auto& floor_property = group_properties("floor");
      assert(floor_property.is_int());
      const auto floor = static_cast<int32_t>(floor_property.as_int());

      for (const auto& sub_layer : group_layer.sub_layers) {
        if (sub_layer.type != gf::MapLayerType::Object) {
          continue;
        }

        const auto& object_layer = map.object_layers[sub_layer.layer_index];
        const auto& object_properties = map.properties[object_layer.layer.properties_index];

        const auto& kind_property = object_properties("kind");
        assert(kind_property.is_string());
        const std::string kind = kind_property.as_string();

        if (kind == "zones") {
          for (const auto& object : object_layer.objects) {
            extract_zone(object, floor, map);
          }

          continue;
        }

        if (kind == "low_sprites" || kind == "high_sprites") {
          for (const auto& object : object_layer.objects) {
            extract_sprites(object, floor, map);
          }

          continue;
        }
      }
    }
  }

  void PhysicsRuntime::extract_zone(const gf::MapObject& object, int32_t floor, const gf::TiledMap& map)
  {
    // 1. prepare zone data

    const auto& object_properties = map.properties[object.properties_index];

    if (!object_properties.has_property("message")) {
      gf::Log::error("Missing message in a zone object: '{}'", object.name);
      return;
    }

    const auto& message = object_properties("message").as_string();

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

    auto body = gf::PhysicsBody::make_static();
    body.set_location(object.location);
    world.add_body(body);

    auto shape = object_to_convex_shape(&body, object);
    shape.set_collision_type(ZoneCollisionType);
    shape.set_sensor(true);
    shape.set_shape_filter(filter_from_floor(floor));
    world.add_shape(shape);

    // 3. add zone to known zones

    zone_handler.add_zone(shape.id(), object.name, message, std::move(requirements));
  }

  void PhysicsRuntime::extract_sprites(const gf::MapObject& object, int32_t floor, const gf::TiledMap& map)
  {
    assert(object.type == gf::MapObjectType::Tile);
    const auto tile = std::get<gf::MapTile>(object.feature);

    const auto* tileset = map.tileset_from_gid(tile.gid);
    const uint32_t id = tile.gid - tileset->first_gid;
    const auto& tileset_tile = tileset->tiles[id];

    if (!tileset_tile.objects) {
      return;
    }

    const auto& object_layer = map.object_layers[*tileset_tile.objects];

    auto body = gf::PhysicsBody::make_static();
    body.set_location(object.location); // TODO: check if correct
    body.set_angle(gf::degrees_to_radians(object.rotation));
    world.add_body(body);

    for (const auto& physics_object : object_layer.objects) {
      auto shapes = object_to_collision_shapes(&body, physics_object);

      for (auto& shape : shapes) {
        shape.set_shape_filter(filter_from_floor(floor));
        world.add_shape(shape);
      }
    }

  }
}
