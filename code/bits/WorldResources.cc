#include "WorldResources.h"

namespace akgr {

  WorldResources::WorldResources()
  {

  }

  gf::ResourceBundle WorldResources::bundle(Akagoria* game)
  {
    gf::ResourceBundle bundle([game, this](gf::ResourceBundle* bundle, gf::ResourceManager* resources, gf::ResourceAction action) {


    });

    return bundle;
  }


}
