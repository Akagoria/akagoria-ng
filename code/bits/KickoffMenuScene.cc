#include "KickoffMenuScene.h"

#include "Akagoria.h"
#include "SlotSelectorRenderer.h"
#include "StartMenuRenderer.h"

namespace akgr {

  namespace {

    constexpr gf::Vec2F MenuSceneWorldSize = { 1600, 900 };



  }

  KickoffMenuScene::KickoffMenuScene(Akagoria* game, const KickoffResources& resources)
  : m_game(game)
  , m_atlas({ 1024, 1024 }, game->render_manager())
  , m_action_group(compute_ui_settings())
  , m_start_menu(game, resources, &m_atlas)
  , m_start_menu_element(game, &m_start_menu)
  , m_slot_selector(game, resources, &m_atlas)
  , m_slot_selector_element(game, &m_slot_selector)
  {
    set_clear_color(gf::White);

    set_world_size(MenuSceneWorldSize);
    set_world_center(MenuSceneWorldSize / 2.0f);

    add_world_entity(&m_start_menu);
    add_world_entity(&m_slot_selector);

    using namespace gf::literals;

    m_ui.add_element("start_menu"_id, &m_start_menu_element);
    m_ui.add_element("slot_selector"_id, &m_slot_selector_element);

    m_ui.change_ui_element("start_menu"_id);
  }

  void KickoffMenuScene::do_process_event(const gf::Event& event)
  {
    m_action_group.process_event(event);
  }

  void KickoffMenuScene::do_handle_actions()
  {
    m_ui.handle_actions(m_action_group);
    m_action_group.reset();
  }

}
