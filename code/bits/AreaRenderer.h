#ifndef AKGR_AREA_RENDERER_H
#define AKGR_AREA_RENDERER_H

#include <gf2/graphics/Entity.h>
#include <gf2/graphics/TextEntity.h>

#include "AreaData.h"
#include "WorldResources.h"

namespace akgr {
  class Akagoria;

  class AreaRenderer : public gf::Entity {
  public:
    AreaRenderer(Akagoria* game, const WorldResources& resources);

    void update(gf::Time time) override;
    void render(gf::RenderRecorder& recorder) override;

  private:
    Akagoria* m_game = nullptr;
    const AreaData* m_current_area = nullptr;
    gf::TextData m_text_data;
    gf::FontAtlas m_atlas;
    gf::Time m_duration;
    gf::TextEntity m_text;
  };

}

#endif // AKGR_AREA_RENDERER_H
