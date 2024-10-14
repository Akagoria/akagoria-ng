#ifndef AKGR_QUEST_RENDERER_H
#define AKGR_QUEST_RENDERER_H

#include <array>

#include <gf2/graphics/Entity.h>
#include <gf2/graphics/TextEntity.h>

#include "WorldResources.h"

namespace akgr {

  class Akagoria;

  class QuestPinnedRenderer : public gf::Entity {
  public:
    QuestPinnedRenderer(Akagoria* game, const WorldResources& resources);

    void update(gf::Time time) override;
    void render(gf::RenderRecorder& recorder) override;

  private:
    gf::RichTextEntity initial_text(const WorldResources& resources);

    static constexpr std::size_t PinnedQuestCount = 5;

    Akagoria* m_game = nullptr;
    gf::FontAtlas m_atlas;
    gf::RichTextStyle m_style;

    gf::TextData m_default_text_data;

    std::size_t m_quest_count = 0;
    std::array<gf::RichTextEntity, PinnedQuestCount> m_quest_texts;
  };

}

#endif // AKGR_QUEST_RENDERER_H
