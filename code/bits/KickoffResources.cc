#include "KickoffResources.h"

#include <gf2/core/Color.h>

#include "Akagoria.h"

namespace akgr {

  KickoffResources::KickoffResources()
  {
    // title

    title_sprite.texture = "logo.png";
    title_sprite.data.color = gf::gray(0.8f);

    main_title_text.font = "fonts/Philosopher-Regular.ttf";
    main_title_text.data.content = "AKAGORIA";
    main_title_text.data.character_size = 150.0f;
    main_title_text.data.letter_spacing_factor = 1.2f;

    additional_title_text.font = "fonts/sawarabi-mincho-medium.ttf";
    additional_title_text.data.content = "アカゴリア";
    additional_title_text.data.character_size = 75.0f;

    subtitle_text.font = "fonts/Philosopher-Bold.ttf";
    subtitle_text.data.content = "The revenge of Kalista"; // TODO: i18n
    subtitle_text.data.character_size = 75.0f;
    subtitle_text.data. color = gf::gray(0.3f);

    underline_shape.buffer = gf::ShapeBuffer::make_rectangle({ 900.0f, 5.0f });
    underline_shape.buffer.color = gf::Black;

    // menu

    icon_arrow_text.font = "fonts/DejaVuSans.ttf";
    icon_arrow_text.data.content = "→";
    icon_arrow_text.data.character_size = 30.0f;
    icon_arrow_text.data.color = gf::White;

    icon_left_text.font = "fonts/DejaVuSans.ttf";
    icon_left_text.data.content = "«";
    icon_left_text.data.character_size = 1.0f;

    icon_right_text.font = "fonts/DejaVuSans.ttf";
    icon_right_text.data.content = "»";
    icon_right_text.data.character_size = 1.0f;

    start_text.font = "fonts/DejaVuSans.ttf";
    start_text.data.content = "Start an adventure"; // TODO: i18n
    start_text.data.character_size = 30.0f;
    start_text.data.color = gf::White;

    load_text.font = "fonts/DejaVuSans.ttf";
    load_text.data.content = "Load an adventure"; // TODO: i18n
    load_text.data.character_size = 30.0f;
    load_text.data.color = gf::White;

    quit_text.font = "fonts/DejaVuSans.ttf";
    quit_text.data.content = "Quit"; // TODO: i18n
    quit_text.data.character_size = 30.0f;
    quit_text.data.color = gf::White;

    back_text.font = "fonts/DejaVuSans.ttf";
    back_text.data.content = "Back"; // TODO: i18n
    back_text.data.character_size = 1.0f;

    // loading

    loading_text.font = "fonts/Philosopher-Regular.ttf";
    loading_text.data.content = "Adventure is coming..."; // TODO: i18n
    loading_text.data.character_size = 50.0f;
  }

  gf::ResourceBundle KickoffResources::bundle(Akagoria* game)
  {
    gf::ResourceBundle bundle([game, this](gf::ResourceBundle* bundle, gf::ResourceManager* resources, gf::ResourceAction action) {
      // fonts

      for (const gf::TextResource* resource : { &main_title_text, &additional_title_text, &subtitle_text, &icon_arrow_text, &icon_left_text, &icon_right_text, &start_text, &load_text, &quit_text, &back_text, &loading_text }) {
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
