#include "Akagoria.h"

namespace akgr {

  Akagoria::Akagoria(const std::filesystem::path& asset_directory)
  : gf::SceneSystem("Akagoria, the revenge of Kalista", gf::vec(1600, 900), asset_directory)
  , m_world_async(render_manager())
  , m_world_model(this)
  {
    auto kickoff_bundle = m_kickoff_resources.bundle(this);
    kickoff_bundle.load_from(resource_manager());

    m_kickoff_act = std::make_unique<KickoffAct>(this, m_kickoff_resources);

    gf::BasicScene* scenes[] = { &m_kickoff_act->base_scene, &m_kickoff_act->menu_scene };
    push_scenes(scenes);
  }

  void Akagoria::load_world(AdventureChoice choice)
  {
    m_world_async.run_async([this]() {
      m_world_model.data.load_from_file(resource_manager()->search("akagoria.dat"));
      m_world_model.runtime.bind(m_world_model.data);
      m_world_model.state.bind(m_world_model.data, m_world_model.runtime);
      auto world_bundle = m_world_resources.bundle(this, &m_world_model.data);
      world_bundle.load_from(resource_manager());
      m_world_act = std::make_unique<WorldAct>(this, m_world_resources);
    });
  }

  bool Akagoria::world_loaded()
  {
    return m_world_async.finished();
  }

}
