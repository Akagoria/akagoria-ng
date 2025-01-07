#include "WorldDebugScene.h"

#include <imgui.h>

#include "Akagoria.h"

namespace akgr {

  namespace {

    // constexpr gf::Vec2F WorldDebugSceneWorldSize = { 1600, 900 };
    // constexpr gf::Vec2F WorldDebugSceneWorldSize = { 1280, 720 };

  }

  WorldDebugScene::WorldDebugScene(Akagoria* game)
  : m_imgui_manager(game->window(), game->render_manager())
  , m_world_debug(game)
  {
    // set_world_size(WorldDebugSceneWorldSize);
    // set_world_center(WorldDebugSceneWorldSize / 2.0f);

    add_hud_entity(&m_imgui_entity);
    add_hud_entity(&m_world_debug);
  }

  bool WorldDebugScene::do_early_process_event(const gf::Event& event)
  {
    return m_imgui_manager.process_event(event);
  }

  void WorldDebugScene::do_update(gf::Time time)
  {
    m_imgui_manager.update(time);

    ImGui::NewFrame();

    update_entities(time);

    ImGui::EndFrame();
    ImGui::Render();

    m_imgui_entity.set_draw_data(ImGui::GetDrawData());
  }

}
