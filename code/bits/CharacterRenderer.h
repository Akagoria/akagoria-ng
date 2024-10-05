#ifndef AKGR_CHARACTER_RENDERER_H
#define AKGR_CHARACTER_RENDERER_H

#include <map>

#include <gf2/core/Id.h>

#include <gf2/graphics/Shape.h>
#include <gf2/graphics/AnimationGroup.h>

#include "FloorRenderer.h"

namespace akgr {
  class Akagoria;
  struct WorldResources;

  class CharacterRenderer : public FloorRenderer {
  public:
    CharacterRenderer(Akagoria* game, const WorldResources& resources);

    void update(gf::Time time, int32_t floor) override;
    void render_floor(gf::RenderRecorder& recorder, int32_t floor) override;

  private:
    Akagoria* m_game = nullptr;
    std::map<gf::Id, gf::AnimationGroupState> m_animation_states;
    std::map<gf::Id, gf::AnimationGroupGraphics> m_animation_graphics;
    gf::ShapeGroup m_dialog_shape_group;
  };

}

#endif // AKGR_CHARACTER_RENDERER_H
