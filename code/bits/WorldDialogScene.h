#ifndef AKGR_WORLD_DIALOG_SCENE_H
#define AKGR_WORLD_DIALOG_SCENE_H

#include <gf2/core/ActionGroup.h>
#include <gf2/graphics/Scene.h>

#include "DialogRenderer.h"
#include "WorldResources.h"

namespace akgr {
  class Akagoria;

  class WorldDialogScene : public gf::Scene {
  public:
    WorldDialogScene(Akagoria* game, const WorldResources& resources);


  private:
    static gf::ActionGroupSettings compute_settings();

    void do_process_event(const gf::Event& event) override;
    void do_handle_actions() override;

    Akagoria* m_game = nullptr;

    gf::ActionGroup m_action_group;

    DialogRenderer m_dialog_renderer;
  };

}

#endif // AKGR_WORLD_DIALOG_SCENE_H
