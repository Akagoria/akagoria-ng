#ifndef AKGR_WORLD_DEBUG_SCENE_H
#define AKGR_WORLD_DEBUG_SCENE_H

#include <gf2/imgui/ImguiEntity.h>
#include <gf2/imgui/ImguiManager.h>
#include <gf2/graphics/Scene.h>

#include "WorldDebug.h"

namespace akgr {
  class Akagoria;

  class WorldDebugScene : public gf::Scene {
  public:
    WorldDebugScene(Akagoria* game);


  private:
    bool do_early_process_event(const gf::Event& event) override;
    void do_update(gf::Time time) override;

    gf::ImguiManager m_imgui_manager;
    gf::ImguiEntity m_imgui_entity;
    WorldDebug m_world_debug;
  };

}

#endif // AKGR_WORLD_DEBUG_SCENE_H
