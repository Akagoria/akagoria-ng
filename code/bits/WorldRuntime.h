#ifndef AKGR_WORLD_RUNTIME_H
#define AKGR_WORLD_RUNTIME_H

#include <gf2/physics/PhysicsWorld.h>

#include "PhysicsRuntime.h"

namespace akgr {
  struct WorldData;

  struct WorldRuntime {
    gf::PhysicsWorld physics_world;
    PhysicsRuntime physics_runtime;

    // TODO: script

    void bind(const WorldData& data);
  };

}

#endif // AKGR_WORLD_RUNTIME_H
