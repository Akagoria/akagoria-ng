#include "PhysicsRuntime.h"

#include <cassert>
#include <cstdint>
#include <cstdlib>

#include <gf2/core/Geometry.h>
#include <gf2/core/Log.h>
#include <gf2/core/Mat3.h>
#include <gf2/core/Polygon.h>
#include <gf2/core/StringUtils.h>

#include <gf2/physics/PhysicsArbiter.h>
#include <gf2/physics/PhysicsConstraint.h>
#include <gf2/physics/PhysicsShape.h>
#include <gf2/physics/PhysicsShapeEx.h>
#include <gf2/physics/PhysicsWorld.h>

#include "PhysicsUtils.h"
#include "WorldData.h"

namespace akgr {

  namespace {

    // hero

    constexpr float HeroMass = 1.0f;
    constexpr float HeroRadius = 20.0f;

    constexpr uintptr_t HeroCollisionType = 69;

    // map

    constexpr uintptr_t ZoneCollisionType = 42;

    constexpr float ShapeRadius = 1.0f;

    constexpr int EllipseSegmentCount = 10;
    constexpr float EllipseSegmentLength = 40.0f;

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
            return { gf::PhysicsShape::make_box(body, gf::RectF::from_position_size(object.location, size), ShapeRadius) };
          }
        case gf::MapObjectType::Polyline:
        case gf::MapObjectType::Polygon:
          {
            auto polyline = std::get<std::vector<gf::Vec2F>>(object.feature);

            for (gf::Vec2F& point : polyline) {
              point += object.location;
            }

            auto type = (object.type == gf::MapObjectType::Polygon) ? gf::PolylineType::Loop : gf::PolylineType::Chain;
            return gf::make_polyline_shapes(body, polyline, ShapeRadius, type);
          }
        case gf::MapObjectType::Ellipse:
          {
            const gf::Vec2F size = std::get<gf::Vec2F>(object.feature);

            if (gf::almost_equals(size.w, size.h)) {
              const float radius = (size.w + size.h) / 2.0f;
              return { gf::PhysicsShape::make_circle(body, radius, object.location) };
            }

            std::vector<gf::Vec2F> polyline;

            const float perimeter_approx = gf::Pi * std::sqrt(2 * (gf::square(size.w) + gf::square(size.h)));
            const int count = std::max(EllipseSegmentCount, static_cast<int>(perimeter_approx / EllipseSegmentLength));
            polyline.reserve(count);

            for (int i = 0; i < count; ++i) {
              polyline.push_back(object.location + size / 2 + size / 2 * gf::unit(2.0f * gf::Pi * float(i) / float(count)));
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

  bool ZoneHandler::begin(gf::PhysicsArbiter arbiter, [[maybe_unused]] gf::PhysicsWorld world)
  {
    auto [ zone_body, hero_body  ] = arbiter.bodies();
    assert(zone_body.type() == gf::PhysicsBodyType::Static);
    const gf::PhysicsId id = zone_body.id();

    if (auto iterator = m_zones.find(id); iterator != m_zones.end()) {
      const auto& zone = iterator->second;
      on_message.emit(zone.message, zone.requirements);
    } else {
      gf::Log::error("No zone found for a body.");
    }

    return true;
  }

  /*
   * PhysicsRuntime
   */

  void PhysicsRuntime::bind(const WorldData& data, const WorldState& state)
  {
    // zone_handler

    world.add_collision_handler(&zone_handler, ZoneCollisionType, HeroCollisionType);

    // hero

    bind_hero(state.hero);

    // map

    const auto& map = data.map;

    for (const auto& layer : map.layers) {
      assert(layer.type == gf::MapLayerType::Group);

      const auto& group_layer = map.group_layers[layer.layer_index];
      const auto& group_properties = map.properties[group_layer.layer.properties_index];

      const auto& floor_property = group_properties("floor");
      assert(floor_property.is_int());
      const auto floor = static_cast<int32_t>(floor_property.as_int());

      std::vector<gf::SegmentI> fences;

      for (const auto& sub_layer : group_layer.sub_layers) {
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

    // hero

  void PhysicsRuntime::bind_hero(const HeroState& state)
  {
    hero.controller = gf::PhysicsBody::make_kinematic();
    world.add_body(hero.controller);

    hero.body = gf::PhysicsBody::make_dynamic(HeroMass, gf::compute_moment_for_circle(HeroMass, 0.0f, HeroRadius, { 0.0f, 0.0f }));
    hero.body.set_location(state.spot.location);
    hero.body.set_rotation(state.rotation);
    world.add_body(hero.body);

    auto pivot = gf::PhysicsConstraint::make_pivot_joint(&hero.controller, &hero.body, { 0.0f, 0.0f }, { 0.0f, 0.0f });
    pivot.set_max_bias(0.0f);
    pivot.set_max_force(1'000.0f);
    world.add_constraint(pivot);

    auto gear = gf::PhysicsConstraint::make_gear_joint(&hero.controller, &hero.body, 0.0f, 1.0f);
    gear.set_error_bias(0.0f);
    gear.set_max_bias(1.2f);
    gear.set_max_force(5'000.0f);
    world.add_constraint(gear);

    hero.shape = gf::PhysicsShape::make_circle(&hero.body, HeroRadius, { 0.0f, 0.0f });
    hero.shape.set_shape_filter(filter_from_floor(state.spot.floor));
    hero.shape.set_collision_type(HeroCollisionType);
    world.add_shape(hero.shape);
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

      const auto* tileset = map.tileset_from_gid(cell.gid);
      const uint32_t gid = cell.gid - tileset->first_gid;
      const auto* tile = tileset->tile(gid);
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
        segment[0] = gf::vec(std::atoi(coords[0].data()), std::atoi(coords[1].data()));
        segment[1] = gf::vec(std::atoi(coords[2].data()), std::atoi(coords[3].data()));

        if (cell.flip.test(gf::CellFlip::Diagonally)) {
          std::swap(segment[0].x, segment[0].y);
          std::swap(segment[1].x, segment[1].y);
        }

        if (cell.flip.test(gf::CellFlip::Horizontally)) {
          segment[0].x = map.tile_size.x - segment[0].x;
          segment[1].x = map.tile_size.x - segment[1].x;
        }

        if (cell.flip.test(gf::CellFlip::Vertically)) {
          segment[0].y = map.tile_size.y - segment[0].y;
          segment[1].y = map.tile_size.y - segment[1].y;
        }

        segment[0] += position * map.tile_size;
        segment[1] += position * map.tile_size;

        fences.push_back(segment);
      }
    }
  }

  void PhysicsRuntime::extract_collisions(const std::vector<gf::SegmentI>& fences, int32_t floor)
  {
    auto body = gf::PhysicsBody::make_static();
    world.add_body(body);

    auto collisions = gf::compute_lines(fences);

    for (auto& collision : collisions) {
      auto shapes = gf::make_polyline_shapes(&body, collision.points, ShapeRadius, collision.type);

      for (auto& shape : shapes) {
        shape.set_shape_filter(filter_from_floor(floor));
        world.add_shape(shape);
      }
    }
  }

  void PhysicsRuntime::compute_object_layer(const gf::MapLayerStructure& layer, int32_t floor, const gf::TiledMap& map)
  {
    assert(layer.type == gf::MapLayerType::Object);

    const auto& object_layer = map.object_layers[layer.layer_index];
    const auto& object_properties = map.properties[object_layer.layer.properties_index];

    const auto& kind_property = object_properties("kind");
    assert(kind_property.is_string());
    const std::string kind = kind_property.as_string();

    if (kind == "zones") {
      for (const auto& object : object_layer.objects) {
        extract_zone(object, floor, map);
      }

      return;
    }

    if (kind == "low_sprites" || kind == "high_sprites") {
      for (const auto& object : object_layer.objects) {
        extract_sprites(object, floor, map);
      }

      return;
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

    zone_handler.add_zone(body.id(), object.name, message, std::move(requirements));
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

    const gf::Vec2F object_size = tileset->tile_size;
    const gf::Vec2F object_center = object.location - gf::diry(object_size.h);
    const gf::Vec2F bottom_left = object.location;
    const gf::Vec2F location = gf::transform_point(gf::rotation(gf::degrees_to_radians(object.rotation), bottom_left), object_center);

    auto body = gf::PhysicsBody::make_static();
    body.set_location(location);
    body.set_rotation(gf::degrees_to_radians(object.rotation));
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
