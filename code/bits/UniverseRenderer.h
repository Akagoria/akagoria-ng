#ifndef AKGR_UNIVERSES_RENDERER_H
#define AKGR_UNIVERSES_RENDERER_H

#include <gf2/graphics/Entity.h>

#include "FloorRenderer.h"
#include "HeroRenderer.h"
#include "MapRenderer.h"

namespace akgr {
  class Akagoria;
  struct WorldResources;

  class UniverseRenderer : public gf::Entity {
  public:
    UniverseRenderer(Akagoria* game, const WorldResources& resources, gf::RichMapRenderer* rich_map_renderer);

    void update(gf::Time time) override;
    void render(gf::RenderRecorder& recorder) override;

  private:
    MapRenderer m_ground_renderer;
    MapRenderer m_low_tile_renderer;
    MapRenderer m_low_object_renderer;
    MapRenderer m_high_tile_renderer;
    MapRenderer m_high_object_renderer;

    HeroRenderer m_hero_renderer;

    FloorRendererContainer m_renderers;
  };

}

#endif // AKGR_UNIVERSES_RENDERER_H
