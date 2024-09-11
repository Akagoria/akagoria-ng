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

  void WorldResources::bind(const WorldData& data)
  {
    // map_textures

    map_textures.clear();
    map_textures.reserve(data.map.textures.size());

    for (const std::filesystem::path& texture : data.map.textures) {
      map_textures.push_back(texture);
    }

    // notifications

    notifications.clear();

    for (const auto& notification : data.notifications) {
      gf::RichTextResource resource;
      resource.data.content = notification.message;
      resource.data.color = gf::White;
      resource.data.alignment = gf::Alignment::Left;
      resource.default_font = "fonts/DejaVuSans.ttf";
      resource.bold_font = "fonts/DejaVuSans-Bold.ttf";
      resource.italic_font = "fonts/DejaVuSans-Oblique.ttf";
      resource.bold_italic_font = "fonts/DejaVuSans-BoldOblique.ttf";

      notifications.emplace(notification.label.id, std::move(resource));
    }

  }

  gf::ResourceBundle WorldResources::bundle(Akagoria* game)
  {
    gf::ResourceBundle bundle([game, this](gf::ResourceBundle* bundle, gf::ResourceManager* resource_manager, gf::ResourceAction action) {
      for (const std::filesystem::path& texture : map_textures) {
        bundle->handle<gf::Texture>(texture, game->render_manager(), resource_manager, action);
      }

      for (const std::filesystem::path& texture : hero_animations.textures) {
        bundle->handle<gf::Texture>(texture, game->render_manager(), resource_manager, action);
      }

      for (const auto& [ id, notification ] : notifications) {
        bundle->handle<gf::FontFace>(notification.default_font, game->font_manager(), resource_manager, action);
        bundle->handle<gf::FontFace>(notification.bold_font, game->font_manager(), resource_manager, action);
        bundle->handle<gf::FontFace>(notification.italic_font, game->font_manager(), resource_manager, action);
        bundle->handle<gf::FontFace>(notification.bold_italic_font, game->font_manager(), resource_manager, action);
      }

    });

    return bundle;
  }


}
