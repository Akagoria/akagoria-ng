#include "WorldRuntime.h"

#include <gf2/core/Log.h>

#include "Akagoria.h"

namespace akgr {

  WorldRuntime::WorldRuntime(Akagoria* game)
  : script(game)
  {
  }

  void WorldRuntime::bind(const WorldData& data, const WorldState& state)
  {
    script.bind();
    physics.bind(data, state);
  }

}
