#ifndef AKGR_MENU_DATA_H
#define AKGR_MENU_DATA_H

#include <gf2/core/ActionSettings.h>
#include <gf2/core/ResourceBundle.h>
#include <gf2/core/ShapeBuffer.h>
#include <gf2/core/SpriteData.h>
#include <gf2/core/TextData.h>

namespace akgr {
  class Akagoria;

  struct MenuData {
    MenuData();

    gf::ResourceBundle bundle(Akagoria* game);

    gf::ActionGroupSettings action_group;

    gf::TextResource icon_arrow_text;
    gf::TextResource icon_left_text;
    gf::TextResource icon_right_text;

    gf::TextResource start_text;
    gf::TextResource load_text;
    gf::TextResource quit_text;
    gf::TextResource back_text;

  };

}

#endif // AKGR_MENU_DATA_H
