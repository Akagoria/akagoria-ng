#include "MenuData.h"

#include "Akagoria.h"
#include "gf2/core/ActionSettings.h"

namespace akgr {

  MenuData::MenuData()
  {
    using namespace gf::literals;

    action_group.actions.emplace("up"_id, gf::ActionSettings(gf::ActionType::Instantaneous).add_keycode_control(gf::Keycode::Up));
    action_group.actions.emplace("down"_id, gf::ActionSettings(gf::ActionType::Instantaneous).add_keycode_control(gf::Keycode::Down));
    action_group.actions.emplace("left"_id, gf::ActionSettings(gf::ActionType::Instantaneous).add_keycode_control(gf::Keycode::Left));
    action_group.actions.emplace("right"_id, gf::ActionSettings(gf::ActionType::Instantaneous).add_keycode_control(gf::Keycode::Right));

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
  }

  gf::ResourceBundle MenuData::bundle(Akagoria* game)
  {
    gf::ResourceBundle bundle([game, this](gf::ResourceBundle* bundle, gf::ResourceManager* resources, gf::ResourceAction action) {
      // fonts

      for (const gf::TextResource* resource : { &icon_arrow_text, &icon_left_text, &icon_right_text }) {
        bundle->handle<gf::FontFace>(resource->font, game->font_manager(), resources, action);
      }

    });

    return bundle;
  }

}
