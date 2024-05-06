#include "Akagoria.h"

namespace akgr {

  Akagoria::Akagoria(const std::filesystem::path& asset_directory)
  : gf::SceneSystem("Akagoria, the revenge of Kalista", gf::vec(1600, 900), asset_directory)
  {
    auto kickoff_bundle = m_kickoff_data.bundle(this);
    kickoff_bundle.load_from(resource_manager());

    m_kickoff_act = std::make_unique<KickoffAct>(this, m_kickoff_data);

    push_scene(&m_kickoff_act->base_scene);
  }

}
