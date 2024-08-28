#include "WorldRuntime.h"

namespace akgr {

  WorldRuntime::WorldRuntime(Akagoria* game)
  : script(game)
  {
  }

  void WorldRuntime::bind(const WorldData& data)
  {
    script.bind();
    physics.bind(data);
  }

}
