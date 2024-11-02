#include "KickoffMenuScene.h"

#include "Akagoria.h"

namespace akgr {

  namespace {

    constexpr gf::Vec2F MenuSceneWorldSize = { 1600, 900 };

  }

  KickoffMenuScene::KickoffMenuScene(Akagoria* game, const KickoffResources& resources)
  : m_game(game)
  , m_atlas({ 1024, 1024 }, game->render_manager())
  , m_action_group(compute_settings())
  , m_start_menu(game, resources, &m_atlas)
  {
    set_clear_color(gf::White);

    set_world_size(MenuSceneWorldSize);
    set_world_center(MenuSceneWorldSize / 2.0f);

    add_world_entity(&m_start_menu);
  }

  gf::ActionGroupSettings KickoffMenuScene::compute_settings()
  {
    using namespace gf::literals;
    gf::ActionGroupSettings settings;

    settings.actions.emplace("up"_id, gf::instantaneous_action().add_keycode_control(gf::Keycode::Up));
    settings.actions.emplace("down"_id, gf::instantaneous_action().add_keycode_control(gf::Keycode::Down));
    settings.actions.emplace("left"_id, gf::instantaneous_action().add_keycode_control(gf::Keycode::Left));
    settings.actions.emplace("right"_id, gf::instantaneous_action().add_keycode_control(gf::Keycode::Right));
    settings.actions.emplace("use"_id, gf::instantaneous_action().add_scancode_control(gf::Scancode::Return));

    return settings;
  }

  void KickoffMenuScene::do_process_event(const gf::Event& event)
  {
    m_action_group.process_event(event);
  }

  void KickoffMenuScene::do_handle_actions()
  {
    using namespace gf::literals;

    if (m_action_group.active("down"_id)) {
      m_start_menu.compute_next_choice();
    }

    if (m_action_group.active("up"_id)) {
      m_start_menu.compute_prev_choice();
    }

    if (m_action_group.active("use"_id)) {
      switch (m_start_menu.choice()) {
        case StartMenuChoice::StartAdventure:
          m_game->load_world(AdventureChoice::New);
          m_game->replace_scene(&m_game->kickoff_act()->loading_scene);
          break;
        default:
          break;
      }
    }

    m_action_group.reset();
  }


}
