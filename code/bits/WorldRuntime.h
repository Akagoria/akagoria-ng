#ifndef AKGR_WORLD_RUNTIME_H
#define AKGR_WORLD_RUNTIME_H

#include <gf2/physics/PhysicsWorld.h>

#include "PhysicsRuntime.h"
#include "Script.h"

namespace akgr {
  class Akagoria;
  struct WorldData;

  struct WorldRuntime {
    WorldRuntime(Akagoria* game);

    Script script;
    PhysicsRuntime physics;

    // TODO: script

    void bind(const WorldData& data);
  };

}

#endif // AKGR_WORLD_RUNTIME_H
