#include "WorldResources.h"

#include "Akagoria.h"

namespace akgr {

  WorldResources::WorldResources()
  {
    using namespace gf::literals;

    hero_animations.textures = { "sprites/kalista.png" };

    gf::AnimationData animation;
    animation.properties = gf::AnimationProperties::Loop;
    animation.color = gf::White;

    animation.add_tileset(0, gf::vec(16, 16), 1000_milliseconds, 1, 0);
    hero_animations.data.animations.emplace("static"_id, animation);

    animation.frames.clear();
    animation.add_tileset(0, gf::vec(16, 16), 25_milliseconds, 16, 0);
    hero_animations.data.animations.emplace("forward"_id, animation);

    animation.frames.clear();
    animation.add_tileset(0, gf::vec(16, 16), 50_milliseconds, 16, 15, -1);
    hero_animations.data.animations.emplace("backward"_id, animation);



  }

  gf::ResourceBundle WorldResources::bundle(Akagoria* game, WorldData* data)
  {
    gf::ResourceBundle bundle([game, data, this](gf::ResourceBundle* bundle, gf::ResourceManager* resource_manager, gf::ResourceAction action) {
      for (const std::filesystem::path& texture : data->map.textures) {
        bundle->handle<gf::Texture>(texture, game->render_manager(), resource_manager, action);
      }

      for (const std::filesystem::path& texture : hero_animations.textures) {
        bundle->handle<gf::Texture>(texture, game->render_manager(), resource_manager, action);
      }

    });

    return bundle;
  }


}
