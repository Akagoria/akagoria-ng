#include "WorldState.h"

#include <gf2/core/Streams.h>
#include <gf2/core/Serialization.h>

#include "Version.h"
#include "WorldData.h"

namespace akgr {

  void WorldState::load_from_file(const std::filesystem::path& filename)
  {
    gf::FileInputStream file(filename);
    gf::CompressedInputStream compressed(&file);
    gf::Deserializer ar(&compressed);

    ar | *this;
  }

  void WorldState::save_to_file(const std::filesystem::path& filename) const
  {
    gf::FileOutputStream file(filename);
    gf::CompressedOutputStream compressed(&file);
    gf::Serializer ar(&compressed, StateVersion);

    ar | *this;
  }

  void WorldState::bind(const WorldData& data)
  {
  }

}
