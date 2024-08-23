#ifndef AKGR_MAP_RENDERER_H
#define AKGR_MAP_RENDERER_H

#include <map>

#include <gf2/graphics/RichMapRenderer.h>

#include "FloorRenderer.h"

namespace akgr {
  struct WorldState;

  enum class PlaneType {
    Tile,
    Object,
  };

  enum class Plane {
    High,
    Low,
    Ground,
  };

  class MapRenderer : public FloorRenderer {
  public:
    MapRenderer(PlaneType plane_type, Plane plane, WorldState* world_state, gf::RichMapRenderer* rich_map_renderer);

    void render_floor(gf::RenderRecorder& recorder, int32_t floor) override;

  private:
    PlaneType m_plane_type;
    Plane m_plane;
    WorldState* m_world_state = nullptr;
    gf::RichMapRenderer* m_rich_map_renderer = nullptr;
    std::map<int, gf::MapLayerStructure> m_layers;
  };

}

#endif // AKGR_MAP_RENDERER_H
