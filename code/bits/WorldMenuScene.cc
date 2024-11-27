#include "WorldMenuScene.h"

#include "Akagoria.h"

namespace akgr {

  namespace {

    constexpr gf::Vec2F WorldMenuSceneSize = { 1600, 900 };

  }

  WorldMenuScene::WorldMenuScene(Akagoria* game, const WorldResources& resources)
  : m_game(game)
  , m_atlas({ 1024, 1024 }, game->render_manager())
  , m_action_group(compute_ui_settings())
  , m_menu(game, resources, &m_atlas)
  , m_menu_element(game, &m_menu)
  , m_inventory(game, resources, &m_atlas)
  , m_inventory_element(game, &m_inventory)
  {
    set_clear_color(gf::White);

    set_world_size(WorldMenuSceneSize);
    set_world_center(WorldMenuSceneSize / 2.0f);

    add_world_entity(&m_menu);
    add_world_entity(&m_inventory);

    using namespace gf::literals;

    m_ui.add_element("menu"_id, &m_menu_element);
    m_ui.add_element("inventory"_id, &m_inventory_element);

    m_ui.change_ui_element("menu"_id);
  }

  void WorldMenuScene::do_process_event(const gf::Event& event)
  {
    m_action_group.process_event(event);
  }

  void WorldMenuScene::do_handle_actions()
  {
    m_ui.handle_actions(m_action_group);
    m_action_group.reset();
  }

}
