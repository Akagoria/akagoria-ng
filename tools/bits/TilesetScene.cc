#include "TilesetScene.h"

#include "imgui.h"

namespace akgr {

  TilesetScene::TilesetScene(const std::filesystem::path& path, gf::BasicSceneManager* scene_manager)
  : m_manager(scene_manager->window(), scene_manager->render_manager())
  , m_gui(path, scene_manager->render_manager())
  {
    add_hud_entity(&m_gui);
    add_hud_entity(&m_entity);
  }

  bool TilesetScene::do_early_process_event(const gf::Event& event)
  {
    return m_manager.process_event(event);
  }

  void TilesetScene::do_update(gf::Time time)
  {
    m_manager.update(time);

    ImGui::NewFrame();

    update_entities(time);

    ImGui::EndFrame();
    ImGui::Render();

    m_entity.set_draw_data(ImGui::GetDrawData());
  }

}
