#ifndef AKGR_KICKOFF_ACT_H
#define AKGR_KICKOFF_ACT_H

#include "KickoffBaseScene.h"
#include "KickoffResources.h"
#include "KickoffMenuScene.h"

namespace akgr {
  class Akagoria;

  struct KickoffAct {
    KickoffAct(Akagoria* game, const KickoffResources& kickoff_resources);

    KickoffBaseScene base_scene;
    KickoffMenuScene menu_scene;
  };

}

#endif // AKGR_KICKOFF_ACT_H
