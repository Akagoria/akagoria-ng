#include "HeroRenderer.h"
#include <gf2/core/Transform.h>

#include "Akagoria.h"
#include "HeroState.h"
#include "WorldResources.h"

namespace akgr {

  HeroRenderer::HeroRenderer(Akagoria* game, const WorldResources& resources)
  : m_state(&game->world_state()->hero)
  , m_animations(resources.hero_animations, game->render_manager(), game->resource_manager())
  {
    m_animations.set_scale(0.8f);
    m_animations.set_origin({ 0.5f, 0.5f });
  }

  void HeroRenderer::update(gf::Time time, [[maybe_unused]] int32_t floor)
  {
    using namespace gf::literals;

    switch (m_state->move.linear) {
      case gf::LinearMove::None:
        m_animations.select("static"_id);
        break;
      case gf::LinearMove::Forward:
        m_animations.select("forward"_id);
        break;
      case gf::LinearMove::Backward:
        m_animations.select("backward"_id);
        break;
    }

    m_animations.update(time);
  }

  void HeroRenderer::render_floor(gf::RenderRecorder& recorder, [[maybe_unused]] int32_t floor)
  {
    m_animations.set_location(m_state->spot.location);
    m_animations.set_rotation(m_state->rotation + gf::Pi2);
    m_animations.render(recorder);
  }

}
