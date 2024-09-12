#ifndef AKGR_DIALOG_RENDERER_H
#define AKGR_DIALOG_RENDERER_H

#include <gf2/core/Id.h>
#include <gf2/core/TextStyle.h>

#include <gf2/graphics/Entity.h>
#include <gf2/graphics/ShapeEntity.h>
#include <gf2/graphics/TextEntity.h>

#include "WorldResources.h"

namespace akgr {
  class Akagoria;

  class DialogRenderer : public gf::Entity {
  public:
    DialogRenderer(Akagoria* game, const WorldResources& resources);

    void update(gf::Time time) override;
    void render(gf::RenderRecorder& recorder) override;

  private:
    Akagoria* m_game = nullptr;
    gf::FontAtlas m_atlas;
    gf::RichTextStyle m_style;

    gf::TextData m_default_speaker_text_data;
    gf::TextEntity m_speaker_text_entity;
    gf::ShapeEntity m_speaker_background_shape;

    gf::TextData m_default_words_text_data;
    gf::RichTextEntity m_words_text_entity;
    gf::ShapeEntity m_words_background_shape;

    gf::Id m_last_dialog_id = gf::InvalidId;
    std::size_t m_last_line = 0;
  };

}

#endif // AKGR_DIALOG_RENDERER_H
