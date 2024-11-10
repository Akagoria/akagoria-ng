#include "UniverseRenderer.h"

#include "Akagoria.h"
#include "MapRenderer.h"

namespace akgr {

  UniverseRenderer::UniverseRenderer(Akagoria* game, const WorldResources& resources, gf::RichMapRenderer* rich_map_renderer)
  : m_game(game)
  , m_ground_renderer(PlaneType::Tile, Plane::Ground, game->world_state(), rich_map_renderer)
  , m_low_tile_renderer(PlaneType::Tile, Plane::Low, game->world_state(), rich_map_renderer)
  , m_low_object_renderer(PlaneType::Object, Plane::Low, game->world_state(), rich_map_renderer)
  , m_high_tile_renderer(PlaneType::Tile, Plane::High, game->world_state(), rich_map_renderer)
  , m_high_object_renderer(PlaneType::Object, Plane::High, game->world_state(), rich_map_renderer)
  , m_hero_renderer(game, resources)
  , m_character_renderer(game, resources)
  , m_item_renderer(game, resources)
  {
    m_renderers.add_renderer(&m_ground_renderer);
    m_renderers.add_renderer(&m_low_tile_renderer);
    m_renderers.add_renderer(&m_low_object_renderer);
    m_renderers.add_renderer(&m_high_tile_renderer);
    m_renderers.add_renderer(&m_high_object_renderer);
    m_renderers.add_renderer(&m_item_renderer);
    m_renderers.add_renderer(&m_character_renderer);
    m_renderers.add_renderer(&m_hero_renderer);

    m_renderers.finish();
  }

  void UniverseRenderer::update(gf::Time time)
  {
    const int32_t floor = m_game->world_state()->hero.spot.floor;
    m_renderers.update(time, floor);
  }

  void UniverseRenderer::render(gf::RenderRecorder& recorder)
  {
    const int32_t floor = m_game->world_state()->hero.spot.floor;
    m_renderers.render_floor(recorder, floor);
  }

}
