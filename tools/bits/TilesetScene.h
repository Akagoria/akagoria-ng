#ifndef AKGR_TILESET_SCENE_H
#define AKGR_TILESET_SCENE_H

#include <gf2/graphics/Scene.h>
#include <gf2/graphics/SceneManager.h>
#include <gf2/imgui/ImguiEntity.h>
#include <gf2/imgui/ImguiManager.h>

#include "TilesetGui.h"

namespace akgr {

  class TilesetScene : public gf::Scene {
  public:
    TilesetScene(const std::filesystem::path& path, gf::BasicSceneManager* scene_manager);


  private:
    bool do_early_process_event(const gf::Event& event) override;
    void do_update(gf::Time time) override;

    gf::ImguiManager m_manager;
    TilesetGui m_gui;
    gf::ImguiEntity m_entity;
  };

}

#endif // AKGR_TILESET_SCENE_H
