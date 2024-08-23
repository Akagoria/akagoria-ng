#include "KickoffAct.h"

namespace akgr {

  KickoffAct::KickoffAct(Akagoria* game, const KickoffResources& resources)
  : base_scene(game, resources)
  , menu_scene(game, resources)
  , loading_scene(game, resources)
  {
  }

}
