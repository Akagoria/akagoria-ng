#ifndef AKGR_UNIVERSES_RENDERER_H
#define AKGR_UNIVERSES_RENDERER_H

#include <gf2/graphics/Entity.h>

#include "FloorRenderer.h"

namespace akgr {

  class UniverseRenderer : public gf::Entity {
  public:

    void update(gf::Time time) override;
    void render(gf::RenderRecorder& recorder) override;

  private:
    FloorRendererContainer m_renderers;
  };

}

#endif // AKGR_UNIVERSES_RENDERER_H
