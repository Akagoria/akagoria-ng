#include "UniverseRenderer.h"

namespace akgr {

  void UniverseRenderer::update(gf::Time time)
  {
    m_renderers.update(time);
  }

  void UniverseRenderer::render(gf::RenderRecorder& recorder)
  {
    const int32_t floor = 0; // TODO
    m_renderers.render_floor(recorder, floor);
  }

}
