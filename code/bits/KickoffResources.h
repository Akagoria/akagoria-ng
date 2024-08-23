#ifndef AKGR_KICKOFF_DATA_H
#define AKGR_KICKOFF_DATA_H

#include <gf2/core/ResourceBundle.h>
#include <gf2/core/ShapeBuffer.h>
#include <gf2/core/SpriteData.h>
#include <gf2/core/TextData.h>

namespace akgr {
  class Akagoria;

  struct KickoffResources {
    KickoffResources();

    gf::ResourceBundle bundle(Akagoria* game);

    // title

    gf::SpriteResource title_sprite;
    gf::TextResource main_title_text;
    gf::TextResource additional_title_text;
    gf::TextResource subtitle_text;
    gf::ShapeResource underline_shape;

    // menu

    gf::TextResource icon_arrow_text;
    gf::TextResource icon_left_text;
    gf::TextResource icon_right_text;

    gf::TextResource start_text;
    gf::TextResource load_text;
    gf::TextResource quit_text;
    gf::TextResource back_text;

    // loading

    gf::TextResource loading_text;
  };

}

#endif // AKGR_KICKOFF_DATA_H
