#ifndef AKGR_AKAGORIA_H
#define AKGR_AKAGORIA_H

#include <filesystem>

#include <gf2/framework/SceneSystem.h>

#include "KickoffAct.h"
#include "KickoffResources.h"

namespace akgr {

  class Akagoria : public gf::SceneSystem {
  public:
    Akagoria(const std::filesystem::path& asset_directory);

    KickoffAct* kickoff_act()
    {
      return m_kickoff_act.get();
    }

  private:
    KickoffResources m_kickoff_resources;
    std::unique_ptr<KickoffAct> m_kickoff_act = nullptr;

  };

}

#endif // AKGR_AKAGORIA_H
