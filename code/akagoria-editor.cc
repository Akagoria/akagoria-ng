// SPDX-License-Identifier: Zlib
// Copyright (c) 2023 Julien Bernard
#include <cstdlib>
#include <filesystem>

#include <imgui.h>

#include <gf2/graphics/Scene.h>
#include <gf2/graphics/SceneManager.h>
#include <gf2/imgui/ImguiEntity.h>
#include <gf2/imgui/ImguiInitializer.h>
#include <gf2/imgui/ImguiManager.h>

#include "config.h"

namespace {

  class ImguiScene : public gf::Scene {
  public:
    ImguiScene(gf::BasicSceneManager* scene_manager)
    : m_manager(scene_manager->window(), scene_manager->render_manager())
    {
      add_hud_entity(&m_entity);
    }

  private:

    bool do_early_process_event(const gf::Event& event) override
    {
      return m_manager.process_event(event);
    }

    void do_update(gf::Time time) override
    {
      m_manager.update(time);

      ImGui::NewFrame();

      update_entities(time);

      ImGui::EndFrame();
      ImGui::Render();

      m_entity.set_draw_data(ImGui::GetDrawData());
    }

    gf::ImguiManager m_manager;
    gf::ImguiEntity m_entity;
  };

  constexpr ImGuiWindowFlags DefaultWindowFlags = ImGuiWindowFlags_NoSavedSettings;

  class EditorEntity : public gf::Entity {
  public:
    EditorEntity(std::filesystem::path data_directory)
    : m_data_directory(std::move(data_directory))
    {
    }

    void update([[maybe_unused]] gf::Time time) override
    {
      if (ImGui::Begin("Akagoria Editor", nullptr, DefaultWindowFlags)) {
        if (ImGui::BeginTabBar("##Tabs")) {

          if (ImGui::BeginTabItem("Summary")) {
            summary();
            ImGui::EndTabItem();
          }

          if (ImGui::BeginTabItem("Data")) {
            data();
            ImGui::EndTabItem();
          }


          ImGui::EndTabBar();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("%s", m_status_text.c_str()); // NOLINT(cppcoreguidelines-pro-type-vararg)
      }

      ImGui::End();
    }

  private:
    void summary() {
      static constexpr const char* SaveLabel = "Save Data";

      if (m_modified) {
        if (ImGui::Button(SaveLabel)) {

          m_modified = false;
        }
      } else {
        ImGui::TextDisabled("%s", SaveLabel); // NOLINT(cppcoreguidelines-pro-type-vararg)
      }
    }

    void data() {
      if (ImGui::Button("Create Data Pack")) {
      }
    }

    std::filesystem::path m_data_directory;

    bool m_modified = false;

    std::string m_status_text = "Editor started...";
  };


}

int main()
{
  const std::filesystem::path assets_directory = akgr::AkagoriaDataDirectory;
  const std::filesystem::path font_file = assets_directory / "akagoria/fonts/DejaVuSans.ttf";

  gf::SingleSceneManager scene_manager("Akagoria Editor", gf::vec(1600, 900));
  const gf::ImguiInitializer imgui_initializer;

  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.IniFilename = nullptr;
  io.Fonts->AddFontFromFileTTF(font_file.string().c_str(), 24);

  EditorEntity editor(assets_directory);

  ImguiScene scene(&scene_manager);
  scene.add_hud_entity(&editor);

  return scene_manager.run(&scene);
}
