#include "UniverseRenderer.h"
#include "MapRenderer.h"

namespace akgr {

  UniverseRenderer::UniverseRenderer(WorldState* world_state, gf::RichMapRenderer* rich_map_renderer)
  : m_ground_renderer(PlaneType::Tile, Plane::Ground, world_state, rich_map_renderer)
  , m_low_tile_renderer(PlaneType::Tile, Plane::Low, world_state, rich_map_renderer)
  , m_low_object_renderer(PlaneType::Object, Plane::Low, world_state, rich_map_renderer)
  , m_high_tile_renderer(PlaneType::Tile, Plane::High, world_state, rich_map_renderer)
  , m_high_object_renderer(PlaneType::Object, Plane::High, world_state, rich_map_renderer)
  {
    m_renderers.add_renderer(&m_ground_renderer);
    m_renderers.add_renderer(&m_low_tile_renderer);
    m_renderers.add_renderer(&m_low_object_renderer);
    m_renderers.add_renderer(&m_high_tile_renderer);
    m_renderers.add_renderer(&m_high_object_renderer);

    m_renderers.finish();
  }

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
