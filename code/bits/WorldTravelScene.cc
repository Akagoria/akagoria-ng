#include "WorldTravelScene.h"

#include "Akagoria.h"
#include "HeroState.h"

namespace akgr {

  WorldTravelScene::WorldTravelScene(Akagoria* game, const WorldResources& resources)
  : m_game(game)
  , m_action_group(compute_settings())
  {
  }

  gf::ActionGroupSettings WorldTravelScene::compute_settings()
  {
    using namespace gf::literals;
    gf::ActionGroupSettings settings;

    settings.actions.emplace("up"_id, gf::continuous_action().add_scancode_control(gf::Scancode::Up));
    settings.actions.emplace("down"_id, gf::continuous_action().add_scancode_control(gf::Scancode::Down));
    settings.actions.emplace("left"_id, gf::continuous_action().add_scancode_control(gf::Scancode::Left));
    settings.actions.emplace("right"_id, gf::continuous_action().add_scancode_control(gf::Scancode::Right));

    return settings;
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

    auto stick = m_motion_group.last_gamepad_stick_location(gf::GamepadStick::Left);

    if (gf::square_length(stick) > gf::square(0.5f)) {
      hero.move.method = HeroMoveMethod::Absolute;
      hero.move.linear = gf::LinearMove::Forward;
      hero.move.angular = gf::AngularMove::None;
      hero.physics.rotation = gf::angle(stick);
    } else {
      if (hero.move.method == HeroMoveMethod::Absolute) {
        hero.move.linear = gf::LinearMove::None;
      }
    }

    m_action_group.reset();
  }

}
