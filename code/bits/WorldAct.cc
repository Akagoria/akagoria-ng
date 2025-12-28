#include "WorldAct.h"

#include <imgui.h>

#include <gf2/core/Log.h>

#include "Akagoria.h"

namespace akgr {

  WorldAct::WorldAct(Akagoria* game, const WorldResources& resources)
  : base_scene(game, resources)
  , travel_scene(game, resources)
  , dialog_scene(game, resources)
  , menu_scene(game, resources)
  // , debug_scene(game)
  {
  }

}
