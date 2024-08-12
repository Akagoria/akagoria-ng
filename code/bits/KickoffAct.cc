#include "KickoffAct.h"

namespace akgr {

  KickoffAct::KickoffAct(Akagoria* game, const KickoffData& kickoff_data, const MenuData& menu_data)
  : base_scene(game, kickoff_data)
  , menu_scene(game, menu_data)
  {
  }

}
