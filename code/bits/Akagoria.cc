#include "Akagoria.h"

#include <imgui.h>

namespace akgr {

  Akagoria::Akagoria(const std::filesystem::path& asset_directory)
  : gf::SceneSystem("Akagoria, the revenge of Kalista", gf::vec(1600, 900), asset_directory)
  , m_world_async(render_manager())
  , m_world_model(this)
  {
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.IniFilename = nullptr;

    auto font_file = resource_manager()->search("fonts/DejaVuSans.ttf");
    assert(!font_file.empty());
    [[maybe_unused]] auto* font = io.Fonts->AddFontFromFileTTF(font_file.string().c_str(), 32.0f);
    assert(font != nullptr);

    m_slot_manager.load_slot_headers();

    auto kickoff_bundle = m_kickoff_resources.bundle(this);
    kickoff_bundle.load_from(resource_manager());

    m_kickoff_act = std::make_unique<KickoffAct>(this, m_kickoff_resources);

    gf::BasicScene* scenes[] = { &m_kickoff_act->base_scene, &m_kickoff_act->menu_scene };
    push_scenes(scenes);
  }

  void Akagoria::load_world(AdventureChoice choice, std::size_t index)
  {
    m_world_async.run_async([this, choice, index]() {
      m_world_model.data.load_from_file(resource_manager()->search("akagoria.dat"));
      m_world_model.data.bind();

      if (choice == AdventureChoice::Saved) {
        m_world_model.state.load_from_file(m_slot_manager.slot(index).saved_state_path);
      }

      m_world_model.state.bind(m_world_model.data);
      m_world_model.runtime.bind(m_world_model.data, m_world_model.state);

      m_world_resources.bind(m_world_model.data);
      auto world_bundle = m_world_resources.bundle(this);
      world_bundle.load_from(resource_manager());
      m_world_act = std::make_unique<WorldAct>(this, m_world_resources);

      m_world_model.runtime.script.initialize();

      if (choice == AdventureChoice::New) {
        m_world_model.runtime.script.start();
      }
    });
  }

  bool Akagoria::world_loaded()
  {
    return m_world_async.finished();
  }

  void Akagoria::start_world()
  {
    gf::BasicScene* scenes[] = { &m_world_act->base_scene, &m_world_act->debug_scene, &m_world_act->travel_scene };
    replace_all_scenes(scenes);
  }

}
