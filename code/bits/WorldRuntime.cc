#include "WorldRuntime.h"

namespace akgr {

  void WorldRuntime::bind(const WorldData& data)
  {
    physics_runtime.bind(data, physics_world);


  }

}
