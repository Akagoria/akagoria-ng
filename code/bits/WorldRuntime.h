#ifndef AKGR_WORLD_RUNTIME_H
#define AKGR_WORLD_RUNTIME_H

#include <gf2/core/TiledMap.h>
#include <gf2/physics/PhysicsWorld.h>

#include "DataLexicon.h"
#include "PhysicsRuntime.h"
#include "Script.h"



namespace akgr {
  class Akagoria;
  struct WorldData;
  struct WorldState;

  struct WorldRuntime {
    WorldRuntime(Akagoria* game);

    Script script;
    PhysicsRuntime physics;

    void bind(const WorldData& data, const WorldState& state);
  };

}

#endif // AKGR_WORLD_RUNTIME_H
