#ifndef AKGR_WORLD_BASE_SCENE_H
#define AKGR_WORLD_BASE_SCENE_H

#include <gf2/graphics/ActionGroup.h>
#include <gf2/graphics/Scene.h>
#include <gf2/graphics/RichMapRenderer.h>
#include <gf2/graphics/Zoom.h>
#include <gf2/framework/GraphicsPhysicsDebug.h>

#include "UniverseRenderer.h"
#include "WorldResources.h"

namespace akgr {
  class Akagoria;

  class WorldBaseScene : public gf::Scene {
  public:
    WorldBaseScene(Akagoria* game, const WorldResources& resources);

  private:
    static gf::ActionGroupSettings compute_settings();

    void do_process_event(const gf::Event& event) override;
    void do_handle_actions() override;
    void do_update(gf::Time time) override;

    Akagoria* m_game = nullptr;

    gf::ActionGroup m_action_group;

    gf::Zoom m_zoom;

    gf::RichMap m_rich_map;
    gf::RichMapRenderer m_rich_map_renderer;
    UniverseRenderer m_universe_renderer;
    gf::GraphicsPhysicsDebugEntity m_physics_debug;
  };


}

#endif // AKGR_WORLD_BASE_SCENE_H
