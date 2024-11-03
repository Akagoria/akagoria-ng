#ifndef AKGR_AKAGORIA_H
#define AKGR_AKAGORIA_H

#include <cstdint>

#include <filesystem>

#include <gf2/framework/SceneSystem.h>

#include <gf2/graphics/RenderAsync.h>

#include "KickoffAct.h"
#include "KickoffResources.h"
#include "SlotManager.h"
#include "WorldAct.h"
#include "WorldModel.h"
#include "WorldResources.h"

namespace akgr {

  enum class AdventureChoice : uint8_t {
    New,
    Saved,
  };

  class Akagoria : public gf::SceneSystem {
  public:
    Akagoria(const std::filesystem::path& asset_directory);

    SlotManager* slot_manager()
    {
      return &m_slot_manager;
    }

    KickoffAct* kickoff_act()
    {
      return m_kickoff_act.get();
    }

    WorldModel* world_model()
    {
      return &m_world_model;
    }

    WorldData* world_data()
    {
      return &m_world_model.data;
    }

    WorldState* world_state()
    {
      return &m_world_model.state;
    }

    WorldAct* world_act()
    {
      return m_world_act.get();
    }

    void load_world(AdventureChoice choice, std::size_t index = 0);
    bool world_loaded();

  private:
    SlotManager m_slot_manager;

    KickoffResources m_kickoff_resources;
    std::unique_ptr<KickoffAct> m_kickoff_act = nullptr;

    gf::RenderAsync m_world_async;
    WorldModel m_world_model;
    WorldResources m_world_resources;
    std::unique_ptr<WorldAct> m_world_act = nullptr;
  };

}

#endif // AKGR_AKAGORIA_H
