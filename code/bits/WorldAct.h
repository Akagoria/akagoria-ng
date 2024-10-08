#ifndef AKGR_WORLD_ACT_H
#define AKGR_WORLD_ACT_H

#include "WorldResources.h"

#include "WorldBaseScene.h"
#include "WorldDialogScene.h"
#include "WorldTravelScene.h"

namespace akgr {
  class Akagoria;

  struct WorldAct {
    WorldAct(Akagoria* game, const WorldResources& resources);

    WorldBaseScene base_scene;
    WorldTravelScene travel_scene;
    WorldDialogScene dialog_scene;
  };

}

#endif // AKGR_WORLD_ACT_H
