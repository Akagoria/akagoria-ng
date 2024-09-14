#ifndef AKGR_ASPECT_RENDERER_H
#define AKGR_ASPECT_RENDERER_H

#include <cstdint>
#include <gf2/core/Id.h>
#include <gf2/core/TextStyle.h>

#include <gf2/graphics/Entity.h>
#include <gf2/graphics/ShapeEntity.h>
#include <gf2/graphics/TextEntity.h>

#include "WorldResources.h"
#include "gf2/core/ShapeBuffer.h"

namespace akgr {
  class Akagoria;

  class AspectRenderer : public gf::Entity {
  public:
    AspectRenderer(Akagoria* game, const WorldResources& resources);

    void update(gf::Time time) override;
    void render(gf::RenderRecorder& recorder) override;

  private:

    gf::ShapeGroupBuffer compute_shape_buffer();

    Akagoria* m_game = nullptr;
    gf::FontAtlas m_atlas;
    gf::TextData m_default_text_data;
    gf::TextEntity m_hp_entity;
    gf::TextEntity m_mp_entity;
    gf::TextEntity m_vp_entity;
    gf::ShapeGroupEntity m_background_shape;
  };

}

#endif // AKGR_ASPECT_RENDERER_H
