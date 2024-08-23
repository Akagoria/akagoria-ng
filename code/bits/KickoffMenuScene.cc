#include "KickoffMenuScene.h"

#include "Akagoria.h"
#include "gf2/core/ActionSettings.h"
#include "ui/Widgets.h"

namespace akgr {

  namespace {

    constexpr gf::Vec2F MenuSceneWorldSize = { 1600, 900 };

  }

  KickoffMenuScene::KickoffMenuScene(Akagoria* game, const KickoffResources& resources)
  : m_game(game)
  , m_atlas({ 1024, 1024 }, game->render_manager())
  , m_action_group(compute_settings())
  , m_icon_arrow_text(&m_atlas, resources.icon_arrow_text, game->render_manager(), game->resource_manager())
  , m_icon_left_text(&m_atlas, resources.icon_left_text, game->render_manager(), game->resource_manager())
  , m_icon_right_text(&m_atlas, resources.icon_right_text, game->render_manager(), game->resource_manager())
  , m_start_text(&m_atlas, resources.start_text, game->render_manager(), game->resource_manager())
  , m_load_text(&m_atlas, resources.load_text, game->render_manager(), game->resource_manager())
  , m_quit_text(&m_atlas, resources.quit_text, game->render_manager(), game->resource_manager())
  , m_back_text(&m_atlas, resources.back_text, game->render_manager(), game->resource_manager())
  , m_frame_widget(nullptr, &m_frame_theme, game->render_manager())
  {
    set_clear_color(gf::White);

    set_world_size(MenuSceneWorldSize);
    set_world_center(MenuSceneWorldSize / 2.0f);

    m_frame_theme.color = { 0.015625f, 0.03125f, 0.515625f, 1.0f };
    // m_frame_theme.margin = { 10.0f, 10.0f };
    // m_frame_theme.outline_color = gf::White;
    // m_frame_theme.outline_thickness = 2.0f;
    // m_frame_theme.radius = 5.0f;

    m_menu_theme.padding = 15.0f;
    m_menu_theme.spacing = 10.0f;

    m_menu_index.count = 3;

    auto* arrow = new ui::LabelWidget(nullptr, &m_label_theme, &m_icon_arrow_text);
    auto* menu = m_frame_widget.add<ui::MenuWidget>(arrow, &m_menu_index, &m_menu_theme);
    menu->add<ui::LabelWidget>(&m_label_theme, &m_start_text);
    menu->add<ui::LabelWidget>(&m_label_theme, &m_load_text);
    menu->add<ui::LabelWidget>(&m_label_theme, &m_quit_text);

    m_frame_widget.compute_layout();

    add_world_entity(&m_frame_widget);
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
      m_menu_index.compute_next_choice();
      m_frame_widget.compute_layout();
    }

    if (m_action_group.active("up"_id)) {
      m_menu_index.compute_prev_choice();
      m_frame_widget.compute_layout();
    }

    if (m_action_group.active("use"_id)) {
      switch (m_menu_index.choice) {
        case 0:
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
