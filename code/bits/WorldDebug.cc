#include "WorldDebug.h"

#include <imgui.h>
#include <fmt/core.h>

#include "Akagoria.h"

namespace akgr {

  WorldDebug::WorldDebug(Akagoria* game)
  : m_game(game)
  {
  }

  void WorldDebug::update([[maybe_unused]] gf::Time time)
  {
    ImGui::SetNextWindowSize(ImVec2(200.0f, 400.0f));

    if (ImGui::Begin("World Debug", nullptr, ImGuiWindowFlags_NoSavedSettings)) {
      if (ImGui::BeginTabBar("##Tabs")) {
        items();
        notifications();
        ImGui::EndTabBar();
      }
    }

    ImGui::End();
  }

  void WorldDebug::items()
  {
    if (ImGui::BeginTabItem("Items")) {
      int id = 0;

      for (auto& item : m_game->world_state()->items) {
        ImGui::PushID(id);
        ImGui::TextUnformatted(item.data->description.c_str());


        ImGui::Separator();
        ImGui::PopID();
        ++id;
      }

      ImGui::EndTabItem();
    }
  }

  void WorldDebug::notifications()
  {
    if (ImGui::BeginTabItem("Notifications")) {
      // int id = 0;

      for (auto& notification : m_game->world_state()->notifications) {
        // ImGui::PushID(id);
        ImGui::TextUnformatted(notification.data->label.tag.c_str());
        ImGui::TextUnformatted(notification.data->message.c_str());
        auto time_string = fmt::format("{:.1f}/{:.1f}", notification.elapsed.as_seconds(), notification.data->duration.as_seconds());
        ImGui::TextUnformatted(time_string.c_str());
        ImGui::Separator();
        // ImGui::PopID();
        // ++id;
      }

      ImGui::EndTabItem();
    }
  }

}
