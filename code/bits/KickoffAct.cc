#include "KickoffAct.h"

namespace akgr {

  KickoffAct::KickoffAct(Akagoria* game, const KickoffResources& kickoff_resources)
  : base_scene(game, kickoff_resources)
  , menu_scene(game, kickoff_resources)
  {
  }

}
