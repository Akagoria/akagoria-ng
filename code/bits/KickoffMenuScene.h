#ifndef AKGR_KICKOFF_MENU_SCENE_H
#define AKGR_KICKOFF_MENU_SCENE_H

#include <cstdint>

#include <gf2/core/ActionGroup.h>
#include <gf2/graphics/FontAtlas.h>
#include <gf2/graphics/Scene.h>

#include "KickoffResources.h"
#include "StartMenuRenderer.h"
#include "SlotSelectorRenderer.h"
#include "UiToolkit.h"

namespace akgr {

  class Akagoria;

  class KickoffMenuScene : public gf::Scene {
  public:
    KickoffMenuScene(Akagoria* game, const KickoffResources& resources);

  private:
    void do_process_event(const gf::Event& event) override;
    void do_handle_actions() override;

    Akagoria* m_game = nullptr;
    gf::FontAtlas m_atlas;

    gf::ActionGroup m_action_group;

    StartMenuRenderer m_start_menu;
    StartMenuElement m_start_menu_element;
    SlotSelectorRenderer m_slot_selector;
    SlotSelectorElement m_slot_selector_element;

    UiToolkit m_ui;
  };

}

#endif // AKGR_KICKOFF_MENU_SCENE_H
