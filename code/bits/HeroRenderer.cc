#include "HeroRenderer.h"

#include "Akagoria.h"
#include "WorldResources.h"

namespace akgr {

  HeroRenderer::HeroRenderer(Akagoria* game, const WorldResources& resources)
  : m_animations(resources.hero_animations, game->render_manager(), game->resource_manager())
  {
  }

  void HeroRenderer::update(gf::Time time)
  {
    using namespace gf::literals;
    m_animations.select("static"_id);
  }

  void HeroRenderer::render_floor(gf::RenderRecorder& recorder, int32_t floor)
  {
    // m_animation_group.set_transform(
    m_animations.render(recorder);
  }

}
