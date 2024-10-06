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
    WorldRuntime runtime;
    WorldState state;

  private:
    void update_hero(gf::Time time);
    void update_characters(gf::Time time);
    void update_physics(gf::Time time);
    void update_notifications(gf::Time time);
  };

}

#endif // AKGR_WORLD_MODEL_H
