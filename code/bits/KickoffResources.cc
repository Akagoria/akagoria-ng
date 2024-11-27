#include "KickoffResources.h"

#include <gf2/core/Color.h>
#include <gf2/framework/BundleBuilder.h>

#include "Akagoria.h"

namespace akgr {

  KickoffResources::KickoffResources()
  {
    // title

    title_sprite.data.color = gf::gray(0.8f);
    title_sprite.texture = "logo.png";

    main_title_text.data.content = "AKAGORIA";
    main_title_text.data.character_size = 150.0f;
    main_title_text.data.letter_spacing_factor = 1.2f;
    main_title_text.font = "fonts/Philosopher-Regular.ttf";

    additional_title_text.data.content = "アカゴリア";
    additional_title_text.data.character_size = 75.0f;
    additional_title_text.font = "fonts/sawarabi-mincho-medium.ttf";

    subtitle_text.data.content = "The revenge of Kalista"; // TODO: i18n
    subtitle_text.data.character_size = 75.0f;
    subtitle_text.data. color = gf::gray(0.3f);
    subtitle_text.font = "fonts/Philosopher-Bold.ttf";

    underline_shape.buffer = gf::ShapeBuffer::make_rectangle({ 900.0f, 5.0f });
    underline_shape.buffer.color = gf::Black;

    // menu

    icon_arrow_text.data.content = "→";
    icon_arrow_text.data.character_size = 30.0f;
    icon_arrow_text.data.color = gf::White;
    icon_arrow_text.font = "fonts/DejaVuSans.ttf";

    icon_left_text.data.content = "«";
    icon_left_text.data.character_size = 1.0f;
    icon_left_text.font = "fonts/DejaVuSans.ttf";

    icon_right_text.data.content = "»";
    icon_right_text.data.character_size = 1.0f;
    icon_right_text.font = "fonts/DejaVuSans.ttf";

    start_text.data.content = "Start an adventure"; // TODO: i18n
    start_text.data.character_size = 30.0f;
    start_text.data.color = gf::White;
    start_text.font = "fonts/DejaVuSans.ttf";

    load_text.data.content = "Load an adventure"; // TODO: i18n
    load_text.data.character_size = 30.0f;
    load_text.data.color = gf::White;
    load_text.font = "fonts/DejaVuSans.ttf";

    quit_text.data.content = "Quit"; // TODO: i18n
    quit_text.data.character_size = 30.0f;
    quit_text.data.color = gf::White;
    quit_text.font = "fonts/DejaVuSans.ttf";

    back_text.data.content = "Back"; // TODO: i18n
    back_text.data.character_size = 30.0f;
    back_text.data.color = gf::White;
    back_text.font = "fonts/DejaVuSans.ttf";

    slot_default_text.data.content = "-\n-\n-";
    slot_default_text.data.character_size = 28.0f;
    slot_default_text.data.color = gf::White;
    slot_default_text.font = "fonts/DejaVuSans.ttf";

    // loading

    loading_text.data.content = "Adventure is coming..."; // TODO: i18n
    loading_text.data.character_size = 50.0f;
    loading_text.font = "fonts/Philosopher-Regular.ttf";

  }

  gf::ResourceBundle KickoffResources::bundle(Akagoria* game) const
  {
    gf::BundleBuilder builder(game);

    builder.add_in_bundle(title_sprite);
    builder.add_in_bundle(main_title_text);
    builder.add_in_bundle(additional_title_text);
    builder.add_in_bundle(subtitle_text);

    builder.add_in_bundle(icon_arrow_text);
    builder.add_in_bundle(icon_left_text);
    builder.add_in_bundle(icon_right_text);

    builder.add_in_bundle(start_text);
    builder.add_in_bundle(load_text);
    builder.add_in_bundle(quit_text);
    builder.add_in_bundle(back_text);

    builder.add_in_bundle(slot_default_text);

    builder.add_in_bundle(load_text);

    return builder.make_bundle();
  }

}
