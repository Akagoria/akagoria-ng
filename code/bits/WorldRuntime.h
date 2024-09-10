#ifndef AKGR_WORLD_RUNTIME_H
#define AKGR_WORLD_RUNTIME_H

#include <gf2/core/TiledMap.h>
#include <gf2/physics/PhysicsWorld.h>

#include "DataLexicon.h"
#include "LocationRuntime.h"
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

    DataLexicon<LocationRuntime> locations;


    void bind(const WorldData& data, const WorldState& state);

  private:
    void bind_map(const WorldData& data);
    void bind_map_object_layer(const gf::MapLayerStructure& layer, int32_t floor, const gf::TiledMap& map);
  };

}

#endif // AKGR_WORLD_RUNTIME_H
