#ifndef AKGR_WORLD_RESOURCES_H
#define AKGR_WORLD_RESOURCES_H

#include <gf2/core/ResourceBundle.h>

namespace akgr {
  class Akagoria;

  struct WorldResources {
    WorldResources();

    gf::ResourceBundle bundle(Akagoria* game);



  };

}

#endif // AKGR_WORLD_RESOURCES_H
