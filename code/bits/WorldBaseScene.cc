#include "WorldBaseScene.h"

#include <gf2/core/Log.h>

#include "Akagoria.h"

namespace akgr {

  namespace {

    std::vector<gf::GpuTexture*> load_textures(const gf::TiledMap* tiled_map, gf::ResourceManager* resource_manager)
    {
      std::vector<gf::GpuTexture*> textures;
      textures.reserve(tiled_map->textures.size());

      for (const std::filesystem::path& filename : tiled_map->textures) {
        textures.push_back(resource_manager->get<gf::GpuTexture>(filename));
      }

      return textures;
    }

    gf::PhysicsDebugOptions compute_options()
    {
      gf::PhysicsDebugOptions options;
      return options;
    }

  }

  WorldBaseScene::WorldBaseScene(Akagoria* game, const WorldResources& resources)
  : m_game(game)
  , m_action_group(compute_settings())
  , m_zoom(world_camera())
  , m_rich_map(&game->world_model()->data.map, load_textures(&game->world_model()->data.map, game->resource_manager()))
  , m_rich_map_renderer(&m_rich_map, game->render_manager())
  , m_universe_renderer(game, resources, &m_rich_map_renderer)
  , m_physics_debug(compute_options(), &game->world_model()->runtime.physics.world, game->render_manager())
  {
    set_world_size({ 800.0f, 800.0f });

    add_model(game->world_model());

    add_world_entity(&m_universe_renderer);
    add_world_entity(&m_physics_debug);

    m_physics_debug.set_awake(false);
  }

  void WorldBaseScene::do_update([[maybe_unused]] gf::Time time)
  {
    update_entities(time);
    set_world_center(m_game->world_model()->state.hero.spot.location);
  }

  gf::ActionGroupSettings WorldBaseScene::compute_settings()
  {
    using namespace gf::literals;
    gf::ActionGroupSettings settings;

    settings.actions.emplace("fullscreen"_id, gf::instantaneous_action().add_keycode_control(gf::Keycode::F));
    // settings.actions.emplace("quit"_id, gf::instantaneous_action().add_scancode_control(gf::Scancode::Escape));

    settings.actions.emplace("physics_debug"_id, gf::instantaneous_action().add_scancode_control(gf::Scancode::F12));
    settings.actions.emplace("save_debug"_id, gf::instantaneous_action().add_scancode_control(gf::Scancode::F11));

    return settings;
  }

  void WorldBaseScene::do_process_event(const gf::Event& event)
  {
    m_zoom.set_surface_size(surface_size());
    m_zoom.process_event(event);

    m_action_group.process_event(event);
  }

  void WorldBaseScene::do_handle_actions()
  {
    using namespace gf::literals;

    if (m_action_group.active("fullscreen"_id)) {
      m_game->window()->toggle_fullscreen();
    }

    // if (m_action_group.active("quit"_id)) {
    //   m_game->window()->close();
    // }

    if (m_action_group.active("physics_debug"_id)) {
      m_physics_debug.set_awake(!m_physics_debug.awake());
    }

    if (m_action_group.active("save_debug"_id)) {
      gf::Log::debug("Save debug!");
      m_game->slot_manager()->save_slot_debug(m_game->world_state(), "Here");
    }

    m_action_group.reset();
  }

}
