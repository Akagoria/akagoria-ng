#include "SlotManager.h"

#include <cassert>

#include <gf2/core/Log.h>
#include <gf2/core/Range.h>
#include <gf2/core/Streams.h>
#include <gf2/core/Serialization.h>
#include <gf2/core/SerializationOps.h>

#include <gf2/graphics/GamePaths.h>

#include "WorldState.h"
#include "Version.h"

namespace akgr {

  /*
   * Slot
   */

  namespace {
    std::filesystem::path compute_header_path(std::filesystem::path path)
    {
      path.replace_extension(".h.dat");
      return path;
    }

  }

  void Slot::load_from_file(const std::filesystem::path& path)
  {
    saved_state_path = path;
    active = std::filesystem::exists(saved_state_path);

    if (active) {
      auto header_path = compute_header_path(path);

      if (std::filesystem::exists(header_path)) {
        gf::FileInputStream stream(header_path);
        gf::Deserializer ar(&stream);
        ar | header;
      } else {
        header.area = "?";
      }
    }
  }

  void Slot::save_header() const
  {
    auto header_path = compute_header_path(saved_state_path);
    gf::FileOutputStream stream(header_path);
    gf::Serializer ar(&stream, MetaVersion);
    ar | header;
  }

  /*
   * SlotManager
   */

  void SlotManager::load_slot_headers()
  {
    const std::filesystem::path save_directory = gf::user_data_path("akagoria", "kalista");
    gf::Log::info("Save directory: {}", save_directory.string());

    for (std::size_t i = 0; i < SlotCount; ++i) {
      const std::string filename = "slot" + std::to_string(i) + ".dat";
      const std::filesystem::path path = save_directory / filename;
      m_slots[i].load_from_file(path);
    }
  }

  void SlotManager::save_slot(const WorldState& state, const std::string& area, std::size_t index)
  {
    assert(index < SlotCount);
    Slot& current_slot = m_slots[index];
    state.save_to_file(current_slot.saved_state_path);

    current_slot.header.area = area.empty() ? "?" : area;
    current_slot.header.time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    current_slot.save_header();
  }

  std::string SlotManager::slot_description(std::size_t index) const
  {
    assert(index < SlotCount);
    const Slot& slot = m_slots[index];

    std::string description = "Slot #" + std::to_string(index + 1) + '\n';

    if (!slot.active) {
      description += "---\n---";
      return description;
    }

    description += slot.header.area + '\n'; // TODO: i18n

    std::array<char, 1024> time_description = {};
    std::strftime(time_description.data(), time_description.size(), "%F %T", std::localtime(&slot.header.time));
    description += time_description.data();

    return description;
  }

}
