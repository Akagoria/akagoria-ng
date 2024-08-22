#include "FloorRenderer.h"

#include <algorithm>

namespace akgr {

  /*
   * FloorRenderer
   */

  FloorRenderer::~FloorRenderer() = default;

  void FloorRenderer::update([[maybe_unused]] gf::Time time) {
  }

  /*
   * FloorRendererContainer
   */

  void FloorRendererContainer::add_renderer(FloorRenderer* renderer)
  {
    m_renderers.push_back(renderer);
  }

  void FloorRendererContainer::finish()
  {
    std::sort(m_renderers.begin(), m_renderers.end(), [](FloorRenderer* lhs, FloorRenderer* rhs) {
      return lhs->priority() < rhs->priority();
    });
  }

  void FloorRendererContainer::update(gf::Time time)
  {
    for (FloorRenderer* renderer : m_renderers) {
      renderer->update(time);
    }
  }

  void FloorRendererContainer::render_floor(gf::RenderRecorder& recorder, int32_t floor)
  {
    for (FloorRenderer* renderer : m_renderers) {
      renderer->render_floor(recorder, floor);
    }
  }

}
