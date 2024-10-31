#ifndef AKGR_SLOT_MANAGER_H
#define AKGR_SLOT_MANAGER_H

#include <ctime>

#include <array>
#include <filesystem>
#include <string>

#include <gf2/core/TypeTraits.h>

namespace akgr {
  struct WorldState;

  struct SlotHeader {
    std::string area;
    std::time_t time;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<SlotHeader, Archive>& header) {
    return ar | header.area | header.time;
  }

  struct Slot {
    bool active = false;
    SlotHeader header;
    std::filesystem::path saved_state_path;

    void load_from_file(const std::filesystem::path& path);
    void save_header() const;
  };

  inline constexpr std::size_t SlotCount = 5;

  class SlotManager {
  public:
    void load_slot_headers();
    void save_slot(const WorldState& state, const std::string& area, std::size_t index);

    void save_slot_debug(const WorldState& state, const std::string& area)
    {
      save_slot(state, area, SlotCount - 1);
    }

    std::string slot_description(std::size_t index) const;

  private:
    std::array<Slot, SlotCount> m_slots;
  };

}

#endif // AKGR_SLOT_MANAGER_H
