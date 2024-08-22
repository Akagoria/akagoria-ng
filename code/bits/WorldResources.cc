#include "WorldResources.h"

#include "Akagoria.h"

namespace akgr {

  WorldResources::WorldResources()
  {

  }

  gf::ResourceBundle WorldResources::bundle(Akagoria* game, WorldData* data)
  {
    gf::ResourceBundle bundle([game, data, this](gf::ResourceBundle* bundle, gf::ResourceManager* resource_manager, gf::ResourceAction action) {
      for (const std::filesystem::path& texture : data->map.textures) {
        bundle->handle<gf::Texture>(texture, game->render_manager(), resource_manager, action);
      }

    });

    return bundle;
  }


}
