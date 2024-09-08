#include "KickoffLoadingScene.h"

#include "Akagoria.h"
#include "gf2/graphics/Scene.h"

namespace akgr {

  KickoffLoadingScene::KickoffLoadingScene(Akagoria* game, const KickoffResources& resources)
  : m_game(game)
  , m_atlas({ 1024, 1024 }, game->render_manager())
  , m_loading_text(&m_atlas, resources.loading_text, game->render_manager(), game->resource_manager())
  {
    set_clear_color(gf::White);
    set_world_size({ 1000.0f, 1000.0f });
    set_world_center({ 500.0f, 500.0f });

    m_loading_text.set_location({ 500.0f, 850.0f });
    m_loading_text.set_origin({ 0.5f, 0.5f }); // center
    add_world_entity(&m_loading_text);
  }

  void KickoffLoadingScene::do_update([[maybe_unused]] gf::Time time)
  {
    if (m_game->world_loaded()) {
      gf::BasicScene* scenes[] = { &m_game->world_act()->base_scene, &m_game->world_act()->travel_scene };
      m_game->replace_all_scenes(scenes);
    }
  }

}
