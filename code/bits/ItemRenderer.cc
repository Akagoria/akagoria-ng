#include "ItemRenderer.h"

#include "Akagoria.h"

namespace akgr {

  ItemRenderer::ItemRenderer(Akagoria* game, const WorldResources& resources)
  : m_game(game)
  {

  }

  void ItemRenderer::update(gf::Time time, int32_t floor)
  {
    for (auto& item : m_game->world_state()->items) {
      if (item.spot.floor != floor) {
        continue;
      }



    }
  }

  void ItemRenderer::render_floor(gf::RenderRecorder& recorder, int32_t floor)
  {

  }

}
