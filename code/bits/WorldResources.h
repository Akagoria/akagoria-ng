#ifndef AKGR_WORLD_RESOURCES_H
#define AKGR_WORLD_RESOURCES_H

#include <gf2/core/AnimationData.h>
#include <gf2/core/ResourceBundle.h>

namespace akgr {
  class Akagoria;
  struct WorldData;

  struct WorldResources {
    WorldResources();

    gf::AnimationGroupResource hero_animations;

    gf::ResourceBundle bundle(Akagoria* game, WorldData* data);
  };

}

#endif // AKGR_WORLD_RESOURCES_H
