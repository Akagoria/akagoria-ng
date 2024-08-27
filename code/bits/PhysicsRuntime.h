#ifndef AKGR_PHYSICS_RUNTIME_H
#define AKGR_PHYSICS_RUNTIME_H

#include <map>
#include <set>
#include <string>

#include <gf2/core/Id.h>
#include <gf2/core/TiledMap.h>

#include <gf2/physics/PhysicsBody.h>
#include <gf2/physics/PhysicsCollisionHandler.h>
#include <gf2/physics/PhysicsWorld.h>

#include "WorldData.h"

namespace akgr {

  class ZoneHandler : public gf::PhysicsCollisionHandler {
  public:
    void add_zone(gf::PhysicsId id, std::string name, std::string message, std::set<gf::Id> requirements);

    bool begin(gf::PhysicsArbiter arbiter, gf::PhysicsWorld world) override;

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

    void bind(const WorldData& data);

  private:
    void extract_zone(const gf::MapObject& object, int32_t floor, const gf::TiledMap& map);
    void extract_sprites(const gf::MapObject& object, int32_t floor, const gf::TiledMap& map);
  };

}

#endif // AKGR_PHYSICS_RUNTIME_H
