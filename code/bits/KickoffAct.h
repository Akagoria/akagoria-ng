#ifndef AKGR_KICKOFF_ACT_H
#define AKGR_KICKOFF_ACT_H

#include "KickoffBaseScene.h"
#include "KickoffData.h"

namespace akgr {
  class Akagoria;

  struct KickoffAct {
    KickoffAct(Akagoria* game, const KickoffData& data);

    KickoffBaseScene base_scene;

  };

}

#endif // AKGR_KICKOFF_ACT_H
