#ifndef AKGR_KICKOFF_DATA_H
#define AKGR_KICKOFF_DATA_H

#include <gf2/core/ActionSettings.h>
#include <gf2/core/ResourceBundle.h>
#include <gf2/core/ShapeBuffer.h>
#include <gf2/core/SpriteData.h>
#include <gf2/core/TextData.h>

namespace akgr {
  class Akagoria;

  struct KickoffData {
    KickoffData();

    gf::ResourceBundle bundle(Akagoria* game);

    gf::ActionGroupSettings base_action_group;

    gf::SpriteResource title_sprite;
    gf::TextResource main_title_text;
    gf::TextResource additional_title_text;
    gf::TextResource subtitle_text;
    gf::ShapeResource underline_shape;
  };

}

#endif // AKGR_KICKOFF_DATA_H
