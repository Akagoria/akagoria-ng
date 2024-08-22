#ifndef AKGR_WORLD_MODEL_H
#define AKGR_WORLD_MODEL_H

#include <gf2/core/Model.h>

#include "WorldData.h"
#include "WorldRuntime.h"
#include "WorldState.h"

namespace akgr {
  class Akagoria;

  struct WorldModel : public gf::Model {
    WorldModel(Akagoria* game);
    void update(gf::Time time) override;

    WorldData data;
    WorldState state;
    WorldRuntime runtime;
  };

}

#endif // AKGR_WORLD_MODEL_H