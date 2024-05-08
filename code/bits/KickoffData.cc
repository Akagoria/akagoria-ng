#include "KickoffData.h"

#include <gf2/core/Color.h>

#include "Akagoria.h"

namespace akgr {

  KickoffData::KickoffData()
  {
    using namespace gf::literals;

    base_action_group.actions.emplace("fullscreen"_id, gf::ActionData(gf::ActionType::Instantaneous).add_keycode_control(gf::Keycode::F));
    base_action_group.actions.emplace("quit"_id, gf::ActionData(gf::ActionType::Instantaneous).add_scancode_control(gf::Scancode::Escape));

    title_sprite.texture = "logo.png";
    title_sprite.data.color = gf::gray(0.8f);

    main_title_text.font = "fonts/Philosopher-Regular.ttf";
    main_title_text.data.content = "AKAGORIA";
    main_title_text.data.character_size = 150.0f;
    main_title_text.data.letter_spacing_factor = 1.3f;

    additional_title_text.font = "fonts/sawarabi-mincho-medium.ttf";
    additional_title_text.data.content = "アカゴリア";
    additional_title_text.data.character_size = 75.0f;

    subtitle_text.font = "fonts/Philosopher-Italic.ttf";
    subtitle_text.data.content = "The revenge of Kalista"; // TODO: i18n
    subtitle_text.data.character_size = 60.0f;

    underline_shape.data = gf::ShapeData::make_rectangle({ 900.0f, 5.0f });
    underline_shape.data.color = gf::Black;
  }

  gf::ResourceBundle KickoffData::bundle(Akagoria* game)
  {
    gf::ResourceBundle bundle([game, this](gf::ResourceBundle* bundle, gf::ResourceManager* resources, gf::ResourceAction action) {
      // fonts

      for (const gf::TextResource* resource : { &main_title_text, &additional_title_text, &subtitle_text }) {
        bundle->handle<gf::FontFace>(resource->font, game->font_manager(), resources, action);
      }

      // textures

      for (const gf::SpriteResource* resource : { &title_sprite }) {
        bundle->handle<gf::Texture>(resource->texture, game->render_manager(), resources, action);
      }
    });

    return bundle;
  }

}
