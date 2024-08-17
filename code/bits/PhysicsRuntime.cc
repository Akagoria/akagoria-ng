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
#include "gf2/core/Math.h"
#include "gf2/core/Polyline.h"
#include "gf2/core/TiledMapData.h"

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
      gf::PhysicsShapeFilter filter;
      filter.categories = filter.mask = bits_from_floor(floor);
      return filter;
    }

    gf::PhysicsShape object_to_convex_shape(gf::PhysicsBody* body, const gf::ObjectData& object)
    {
      switch (object.type) {
        case gf::ObjectType::Polygon:
          {
            auto polygon = std::get<std::vector<gf::Vec2F>>(object.feature);
            assert(gf::is_convex(polygon));
            return gf::PhysicsShape::make_polygon(body, polygon, gf::Identity3F, ShapeRadius);
          }
        case gf::ObjectType::Rectangle:
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

    std::vector<gf::PhysicsShape> object_to_collision_shapes(gf::PhysicsBody* body, const gf::ObjectData& object)
    {
      switch (object.type) {
        case gf::ObjectType::Rectangle:
          {
            gf::Vec2F size = std::get<gf::Vec2F>(object.feature);
            return { gf::PhysicsShape::make_box(body, gf::RectF::from_size(size), ShapeRadius) };
          }
        case gf::ObjectType::Polyline:
        case gf::ObjectType::Polygon:
          {
            auto polyline = std::get<std::vector<gf::Vec2F>>(object.feature);
            auto type = (object.type == gf::ObjectType::Polygon) ? gf::PolylineType::Loop : gf::PolylineType::Chain;
            return gf::make_polyline_shapes(body, polyline, ShapeRadius, type);
          }
        case gf::ObjectType::Ellipse:
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

  bool PhysicsRuntime::begin(gf::PhysicsArbiter arbiter, gf::PhysicsWorld world)
  {
    return true;
  }

  void PhysicsRuntime::bind(const WorldData& data, gf::PhysicsWorld& physics_world)
  {
    const auto& map = data.map;

    for (const auto& layer : map.data.layers) {
      assert(layer.type == gf::LayerType::Group);

      const auto& group_layer = map.data.group_layers[layer.layer_index];
      const auto& group_properties = map.data.properties[group_layer.layer.properties_index];

      const auto& floor_property = group_properties("floor");
      assert(floor_property.is_int());
      const auto floor = static_cast<int32_t>(floor_property.as_int());

      for (const auto& sub_layer : group_layer.sub_layers) {
        if (sub_layer.type != gf::LayerType::Object) {
          continue;
        }

        const auto& object_layer = map.data.object_layers[sub_layer.layer_index];
        const auto& object_properties = map.data.properties[object_layer.layer.properties_index];

        const auto& kind_property = object_properties("kind");
        assert(kind_property.is_string());
        const std::string kind = kind_property.as_string();

        if (kind == "zones") {
          for (const auto& object : object_layer.objects) {
            extract_zone(object, floor, map, physics_world);
          }

          continue;
        }

        if (kind == "low_sprites" || kind == "high_sprites") {
          for (const auto& object : object_layer.objects) {
            extract_sprites(object, floor, map, physics_world);
          }

          continue;
        }
      }
    }
  }

  void PhysicsRuntime::extract_zone(const gf::ObjectData& object, int32_t floor, const gf::TiledMapResource& map, gf::PhysicsWorld& physics_world)
  {
    // 1. prepare zone data

    Zone zone;
    zone.name = object.name;

    const auto& object_properties = map.data.properties[object.properties_index];

    if (!object_properties.has_property("message")) {
      gf::Log::error("Missing message in a zone object: '{}'", zone.name);
      return;
    }

    const auto& message_property = object_properties("message");
    assert(message_property.is_string());
    zone.message = message_property.as_string();

    if (object_properties.has_property("requirements")) {
      const auto& requirements_property = object_properties("requirements");
      assert(requirements_property.is_string());
      std::string requirements = requirements_property.as_string();
      auto requirement_list = gf::split_string(requirements, ",");

      for (auto requirement : requirement_list) {
        if (requirement.empty()) {
          continue;
        }

        zone.requirements.insert(gf::hash_string(requirement));
      }
    }

    // 2. create shape

    auto body = gf::PhysicsBody::make_static();
    body.set_location(object.location);
    physics_world.add_body(body);

    auto shape = object_to_convex_shape(&body, object);
    shape.set_collision_type(ZoneCollisionType);
    shape.set_sensor(true);
    shape.set_shape_filter(filter_from_floor(floor));
    physics_world.add_shape(shape);

    // 3. add zone to known zones

    m_zones.emplace(shape.id(), std::move(zone));
  }

  void PhysicsRuntime::extract_sprites(const gf::ObjectData& object, int32_t floor, const gf::TiledMapResource& map, gf::PhysicsWorld& physics_world)
  {
    assert(object.type == gf::ObjectType::Tile);
    const auto tile = std::get<gf::TileData>(object.feature);

    const auto* tileset = map.data.tileset_from_gid(tile.gid);
    const uint32_t id = tile.gid - tileset->first_gid;
    const auto& tileset_tile = tileset->tiles[id];

    if (!tileset_tile.objects) {
      return;
    }

    const auto& object_layer = map.data.object_layers[*tileset_tile.objects];

    auto body = gf::PhysicsBody::make_static();
    body.set_location(object.location); // TODO: check if correct
    body.set_angle(gf::degrees_to_radians(object.rotation));
    physics_world.add_body(body);

    for (const auto& physics_object : object_layer.objects) {
      auto shapes = object_to_collision_shapes(&body, physics_object);

      for (auto& shape : shapes) {
        shape.set_shape_filter(filter_from_floor(floor));
        physics_world.add_shape(shape);
      }
    }

  }
}
