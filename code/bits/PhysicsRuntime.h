#ifndef AKGR_PHYSICS_RUNTIME_H
#define AKGR_PHYSICS_RUNTIME_H

#include <map>
#include <set>
#include <string>

#include <gf2/core/Id.h>
#include <gf2/physics/PhysicsBody.h>
#include <gf2/physics/PhysicsCollisionHandler.h>
#include <gf2/physics/PhysicsWorld.h>

#include "WorldData.h"

namespace akgr {

  class PhysicsRuntime : public gf::PhysicsCollisionHandler {
  public:

    bool begin(gf::PhysicsArbiter arbiter, gf::PhysicsWorld world) override;

    void bind(const WorldData& data, gf::PhysicsWorld& physics_world);

  private:
    void extract_zone(const gf::ObjectData& object, int32_t floor, const gf::TiledMapResource& map, gf::PhysicsWorld& physics_world);
    void extract_sprites(const gf::ObjectData& object, int32_t floor, const gf::TiledMapResource& map, gf::PhysicsWorld& physics_world);

    struct Zone {
      std::string name;
      std::string message;
      std::set<gf::Id> requirements;
    };

    std::map<gf::PhysicsId, Zone> m_zones;
  };


}

#endif // AKGR_PHYSICS_RUNTIME_H
