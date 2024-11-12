#include "WorldTravelScene.h"

#include <gf2/core/Log.h>

#include "Akagoria.h"
#include "HeroState.h"

namespace akgr {

  namespace {

    constexpr gf::Vec2F WorldTravelSceneWorldSize = { 1600, 900 };

    constexpr float DialogDistance = 100.0f;
    constexpr float ItemDistance = 100.0f;

  }

  WorldTravelScene::WorldTravelScene(Akagoria* game, const WorldResources& resources)
  : m_game(game)
  , m_action_group(compute_settings())
  , m_notification_renderer(game, resources)
  , m_aspect_renderer(game, resources)
  , m_area_renderer(game, resources)
  , m_quest_renderer(game, resources)
  {
    set_world_size(WorldTravelSceneWorldSize);
    set_world_center(WorldTravelSceneWorldSize / 2.0f);

    add_world_entity(&m_notification_renderer);
    add_world_entity(&m_aspect_renderer);
    add_world_entity(&m_area_renderer);
    add_world_entity(&m_quest_renderer);
  }

  gf::ActionGroupSettings WorldTravelScene::compute_settings()
  {
    using namespace gf::literals;
    gf::ActionGroupSettings settings;

    settings.actions.emplace("up"_id, gf::continuous_action().add_scancode_control(gf::Scancode::Up));
    settings.actions.emplace("down"_id, gf::continuous_action().add_scancode_control(gf::Scancode::Down));
    settings.actions.emplace("left"_id, gf::continuous_action().add_scancode_control(gf::Scancode::Left));
    settings.actions.emplace("right"_id, gf::continuous_action().add_scancode_control(gf::Scancode::Right));
    settings.actions.emplace("operate"_id, gf::instantaneous_action().add_scancode_control(gf::Scancode::Return));

    return settings;
  }

  void WorldTravelScene::on_rank_change(gf::SceneRank old_rank, gf::SceneRank new_rank)
  {
    if (old_rank == gf::SceneRank::Top && new_rank == gf::SceneRank::None) {
      m_action_group.hard_reset();
    }
  }

  void WorldTravelScene::do_process_event(const gf::Event& event)
  {
    m_action_group.process_event(event);
    m_motion_group.process_event(event);
  }

  void WorldTravelScene::do_handle_actions()
  {
    using namespace gf::literals;
    auto& hero = m_game->world_state()->hero;

    if (m_action_group.active("up"_id)) {
      hero.move.method = HeroMoveMethod::Relative;
      hero.move.linear = gf::LinearMove::Forward;
    } else if (m_action_group.active("down"_id)) {
      hero.move.method = HeroMoveMethod::Relative;
      hero.move.linear = gf::LinearMove::Backward;
    } else {
      hero.move.linear = gf::LinearMove::None;
    }

    if (m_action_group.active("left"_id)) {
      hero.move.method = HeroMoveMethod::Relative;
      hero.move.angular = gf::AngularMove::Left;
    } else if (m_action_group.active("right"_id)) {
      hero.move.method = HeroMoveMethod::Relative;
      hero.move.angular = gf::AngularMove::Right;
    } else {
      hero.move.angular = gf::AngularMove::None;
    }

    if (m_action_group.active("operate"_id)) {
      handle_hero_operation();
    }

    auto stick = m_motion_group.last_gamepad_stick_location(gf::GamepadStick::Left);

    if (gf::square_length(stick) > gf::square(0.5f)) {
      hero.move.method = HeroMoveMethod::Absolute;
      hero.move.linear = gf::LinearMove::Forward;
      hero.move.angular = gf::AngularMove::None;
      hero.rotation = gf::angle(stick);
    } else {
      if (hero.move.method == HeroMoveMethod::Absolute) {
        hero.move.linear = gf::LinearMove::None;
      }
    }

    m_action_group.reset();
  }

  void WorldTravelScene::handle_hero_operation()
  {
    if (handle_hero_dialogs()) {
      return;
    }

    if (handle_hero_items()) {
      return;
    }
  }

  bool WorldTravelScene::handle_hero_dialogs()
  {
    auto& hero = m_game->world_state()->hero;

    for (auto& character : m_game->world_state()->characters) {
      if (!character.dialog) {
        continue;
      }

      if (character.spot.floor != hero.spot.floor) {
        continue;
      }

      if (gf::square_distance(character.spot.location, hero.spot.location) < gf::square(DialogDistance)) {
        hero.dialog.data = character.dialog;
        hero.dialog.current_line = 0;
        character.dialog.reset();
        m_game->replace_scene(&m_game->world_act()->dialog_scene);
        return true;
      }
    }

    return false;
  }

  bool WorldTravelScene::handle_hero_items()
  {
    auto& hero = m_game->world_state()->hero;
    auto& items = m_game->world_state()->items;

    for (auto& item : items) {
      if (item.spot.floor != hero.spot.floor) {
        continue;
      }

      if (gf::square_distance(item.spot.location, hero.spot.location) < gf::square(ItemDistance)) {
        item.picked = true;
        // item is actually removed in WorldModel
        return true;
      }
    }

    return false;
  }

}
