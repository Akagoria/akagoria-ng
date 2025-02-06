#ifndef AKGR_WORLD_MENU_SCENE_H
#define AKGR_WORLD_MENU_SCENE_H

#include <gf2/core/ActionGroup.h>
#include <gf2/graphics/FontAtlas.h>
#include <gf2/graphics/Scene.h>

#include "InventoryRenderer.h"
#include "UiToolkit.h"
#include "WorldMenuRenderer.h"
#include "WorldResources.h"

namespace akgr {

  class Akagoria;

  class WorldMenuScene : public gf::Scene {
  public:
    WorldMenuScene(Akagoria* game, const WorldResources& resources);


  private:
    void do_process_event(const gf::Event& event) override;
    void do_handle_actions() override;

    Akagoria* m_game = nullptr;
    gf::FontAtlas m_atlas;

    gf::ActionGroup m_action_group;

    WorldMenuRenderer m_menu;
    WorldMenuElement m_menu_element;
    InventoryRenderer m_inventory;
    InventoryElement m_inventory_element;

    UiToolkit m_ui;
  };

}

#endif // AKGR_WORLD_MENU_SCENE_H
