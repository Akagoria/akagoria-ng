#ifndef AKGR_WORLD_RUNTIME_H
#define AKGR_WORLD_RUNTIME_H

#include <gf2/physics/PhysicsWorld.h>

#include "PhysicsRuntime.h"

namespace akgr {
  struct WorldData;

  struct WorldRuntime {
    PhysicsRuntime physics;

    // TODO: script

    void bind(const WorldData& data);
  };

}

#endif // AKGR_WORLD_RUNTIME_H
