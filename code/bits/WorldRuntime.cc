#include "WorldRuntime.h"

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
