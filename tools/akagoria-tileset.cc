#include <imgui.h>

#include <gf2/core/Log.h>
#include <gf2/graphics/SceneManager.h>
#include <gf2/imgui/ImguiInitializer.h>

#include "bits/TilesetScene.h"

#include "config.h"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    gf::Log::error("Usage: akagoria-tileset <file.json>");
    return EXIT_FAILURE;
  }

  const std::filesystem::path path(argv[1]);

  if (!std::filesystem::exists(path)) {
    gf::Log::info("File does not exists. Creating an empty file: '{}'", path.string());
    akgr::TilesetData data;
    akgr::TilesetData::save(path, data);
  }

  const std::filesystem::path assets_directory = akgr::AkagoriaDataDirectory;
  const std::filesystem::path font_file = assets_directory / "tools/Oxanium-Regular.ttf";

  gf::SingleSceneManager scene_manager("akagoria-tileset", gf::vec(1920, 1080));
  const gf::ImguiInitializer imgui_initializer;

  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.IniFilename = nullptr;
  io.Fonts->AddFontFromFileTTF(font_file.string().c_str(), 32);

  akgr::TilesetScene scene(path, &scene_manager);
  return scene_manager.run(&scene);
}
