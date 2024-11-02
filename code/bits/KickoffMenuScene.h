#ifndef AKGR_KICKOFF_MENU_SCENE_H
#define AKGR_KICKOFF_MENU_SCENE_H

#include <gf2/graphics/ActionGroup.h>
#include <gf2/graphics/FontAtlas.h>
#include <gf2/graphics/Scene.h>

#include "KickoffResources.h"
#include "StartMenuRenderer.h"

namespace akgr {

  class Akagoria;

  class KickoffMenuScene : public gf::Scene {
  public:
    KickoffMenuScene(Akagoria* game, const KickoffResources& resources);

  private:
    static gf::ActionGroupSettings compute_settings();

    void do_process_event(const gf::Event& event) override;
    void do_handle_actions() override;

    Akagoria* m_game = nullptr;
    gf::FontAtlas m_atlas;

    gf::ActionGroup m_action_group;

    StartMenuRenderer m_start_menu;
  };

}

#endif // AKGR_KICKOFF_MENU_SCENE_H
