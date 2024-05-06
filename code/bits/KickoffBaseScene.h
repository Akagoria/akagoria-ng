#ifndef AKGR_KICKOFF_BASE_SCENE_H
#define AKGR_KICKOFF_BASE_SCENE_H

#include <gf2/graphics/FontAtlas.h>
#include <gf2/graphics/Scene.h>
#include <gf2/graphics/ShapeEntity.h>
#include <gf2/graphics/SpriteEntity.h>
#include <gf2/graphics/TextEntity.h>

#include "KickoffData.h"

namespace akgr {
  class Akagoria;

  class KickoffBaseScene : public gf::StandardScene {
  public:
    KickoffBaseScene(Akagoria* game, const KickoffData& data);

  private:
    Akagoria* m_game = nullptr;
    gf::FontAtlas m_atlas;

    gf::SpriteEntity m_title_sprite;
    gf::TextEntity m_main_title_text;
    gf::TextEntity m_additional_title_text;
    gf::TextEntity m_subtitle_text;
    gf::ShapeEntity m_underline_shape;
  };

}

#endif // AKGR_KICKOFF_BASE_SCENE_H
