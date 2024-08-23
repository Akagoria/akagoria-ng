#ifndef AKGR_WORLD_ACT_H
#define AKGR_WORLD_ACT_H

#include "WorldResources.h"

#include "WorldBaseScene.h"

namespace akgr {
  class Akagoria;

  struct WorldAct {
    WorldAct(Akagoria* game, const WorldResources& resources);

    WorldBaseScene base_scene;
  };

}

#endif // AKGR_WORLD_ACT_H
