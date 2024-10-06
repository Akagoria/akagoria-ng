#ifndef AKGR_PHYSICS_RUNTIME_H
#define AKGR_PHYSICS_RUNTIME_H

#include <map>
#include <set>
#include <string>

#include <gf2/core/Geometry.h>
#include <gf2/core/Id.h>
#include <gf2/core/Signal.h>
#include <gf2/core/TiledMap.h>

#include <gf2/physics/PhysicsBody.h>
#include <gf2/physics/PhysicsCollisionHandler.h>
#include <gf2/physics/PhysicsWorld.h>

#include "PhysicsMovableRuntime.h"
#include "WorldData.h"
#include "WorldState.h"

namespace akgr {

  class ZoneHandler : public gf::PhysicsCollisionHandler {
  public:
    void add_zone(gf::PhysicsId id, std::string name, std::string message, std::set<gf::Id> requirements);
    void clear_zones();

    bool begin(gf::PhysicsArbiter arbiter, gf::PhysicsWorld world) override;

    gf::Signal<void(const std::string& message, const std::set<gf::Id>& requirements)> on_message; // NOLINT

  private:
    struct Zone {
      std::string name;
      std::string message;
      std::set<gf::Id> requirements;
    };

    std::map<gf::PhysicsId, Zone> m_zones;
  };


  struct PhysicsRuntime {
    gf::PhysicsWorld world;
    ZoneHandler zone_handler;

    PhysicsMovableRuntime hero;
    std::map<gf::Id, PhysicsMovableRuntime> characters;


    PhysicsMovableRuntime create_character(Spot spot, float rotation, uintptr_t collision_type = 0);

    void bind(const WorldData& data, const WorldState& state);

  private:
    void bind_hero(const HeroState& state);
    void bind_characters(const std::vector<CharacterState>& states);

    void compute_tile_layer(const gf::MapLayerStructure& layer, const gf::TiledMap& map, std::vector<gf::SegmentI>& fences);
    void extract_collisions(const std::vector<gf::SegmentI>& fences, int32_t floor);

    void compute_object_layer(const gf::MapLayerStructure& layer, int32_t floor, const gf::TiledMap& map);
    void extract_zone(const gf::MapObject& object, int32_t floor, const gf::TiledMap& map);
    void extract_sprites(const gf::MapObject& object, int32_t floor, const gf::TiledMap& map);
  };

}

#endif // AKGR_PHYSICS_RUNTIME_H
