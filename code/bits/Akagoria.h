#ifndef AKGR_AKAGORIA_H
#define AKGR_AKAGORIA_H

#include <filesystem>

#include <gf2/framework/SceneSystem.h>

#include "KickoffAct.h"
#include "KickoffData.h"
#include "MenuData.h"

namespace akgr {

  class Akagoria : public gf::SceneSystem {
  public:
    Akagoria(const std::filesystem::path& asset_directory);

    KickoffAct* kickoff_act()
    {
      return m_kickoff_act.get();
    }

  private:
    MenuData m_menu_data;
    KickoffData m_kickoff_data;
    std::unique_ptr<KickoffAct> m_kickoff_act = nullptr;

  };

}

#endif // AKGR_AKAGORIA_H
