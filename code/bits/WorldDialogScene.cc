#include "WorldDialogScene.h"

#include "Akagoria.h"
#include "DialogRenderer.h"
#include "HeroState.h"
#include "gf2/core/ActionSettings.h"

namespace akgr {

  namespace {

    constexpr gf::Vec2F WorldDialogSceneWorldSize = { 1600, 900 };

  }

  WorldDialogScene::WorldDialogScene(Akagoria* game, const WorldResources& resources)
  : m_game(game)
  , m_action_group(compute_settings())
  , m_dialog_renderer(game, resources)
  {
    set_world_size(WorldDialogSceneWorldSize);
    set_world_center(WorldDialogSceneWorldSize / 2.0f);

    add_world_entity(&m_dialog_renderer);
  }

  gf::ActionGroupSettings WorldDialogScene::compute_settings()
  {
    using namespace gf::literals;
    gf::ActionGroupSettings settings;

    settings.actions.emplace("next"_id, gf::instantaneous_action().add_scancode_control(gf::Scancode::Return));

    return settings;
  }

  void WorldDialogScene::do_process_event(const gf::Event& event)
  {
    m_action_group.process_event(event);
  }

  void WorldDialogScene::do_handle_actions()
  {
    using namespace gf::literals;
    auto& hero = m_game->world_state()->hero;

    hero.move.angular = gf::AngularMove::None;
    hero.move.linear = gf::LinearMove::None;

    if (m_action_group.active("next"_id)) {
      auto& dialog = hero.dialog;
      assert(dialog.data);

      ++dialog.current_line;

      if (dialog.current_line >= dialog.data->content.size()) {
        // end of the dialog
        const std::string& label = dialog.data->label.tag;
        dialog.data.reset();
        m_game->world_model()->runtime.script.on_dialog(label);

        m_game->replace_scene(&m_game->world_act()->travel_scene);
      }
    }

    m_action_group.reset();
  }

}
