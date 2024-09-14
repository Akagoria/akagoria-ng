#ifndef AKGR_WORLD_TRAVEL_SCENE_H
#define AKGR_WORLD_TRAVEL_SCENE_H

#include <gf2/graphics/ActionGroup.h>
#include <gf2/graphics/MotionGroup.h>
#include <gf2/graphics/Scene.h>

#include "AspectRenderer.h"
#include "NotificationRenderer.h"
#include "WorldResources.h"

namespace akgr {
  class Akagoria;

  class WorldTravelScene : public gf::Scene {
  public:
    WorldTravelScene(Akagoria* game, const WorldResources& resources);


  private:
    static gf::ActionGroupSettings compute_settings();

    void on_rank_change(gf::SceneRank old_rank, gf::SceneRank new_rank) override;

    void do_process_event(const gf::Event& event) override;
    void do_handle_actions() override;

    Akagoria* m_game = nullptr;

    gf::ActionGroup m_action_group;
    gf::MotionGroup m_motion_group;

    NotificationRenderer m_notification_renderer;
    AspectRenderer m_aspect_renderer;
  };

}

#endif // AKGR_WORLD_TRAVEL_SCENE_H
