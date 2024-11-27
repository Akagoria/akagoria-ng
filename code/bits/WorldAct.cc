#include "WorldAct.h"

namespace akgr {

  WorldAct::WorldAct(Akagoria* game, const WorldResources& resources)
  : base_scene(game, resources)
  , travel_scene(game, resources)
  , dialog_scene(game, resources)
  , menu_scene(game, resources)
  {
  }

}
