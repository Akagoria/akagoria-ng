#ifndef AKGR_HERO_RENDERER_H
#define AKGR_HERO_RENDERER_H

#include <gf2/graphics/AnimationEntity.h>

#include "FloorRenderer.h"

namespace akgr {
  class Akagoria;
  struct HeroState;
  struct WorldResources;

  class HeroRenderer : public FloorRenderer {
  public:
    HeroRenderer(Akagoria* game, const WorldResources& resources);

    void update(gf::Time time) override;
    void render_floor(gf::RenderRecorder& recorder, int32_t floor) override;

  private:
    HeroState* m_state = nullptr;
    gf::AnimationGroupEntity m_animations;
  };

}

#endif // AKGR_HERO_RENDERER_H
