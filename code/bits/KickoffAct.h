#ifndef AKGR_KICKOFF_ACT_H
#define AKGR_KICKOFF_ACT_H

#include "KickoffBaseScene.h"
#include "KickoffLoadingScene.h"
#include "KickoffMenuScene.h"
#include "KickoffResources.h"

namespace akgr {
  class Akagoria;

  struct KickoffAct {
    KickoffAct(Akagoria* game, const KickoffResources& resources);

    KickoffBaseScene base_scene;
    KickoffMenuScene menu_scene;
    KickoffLoadingScene loading_scene;
  };

}

#endif // AKGR_KICKOFF_ACT_H
