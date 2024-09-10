#include "WorldState.h"

#include <gf2/core/Streams.h>
#include <gf2/core/SerializationOps.h>

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

  namespace {



  }

  void WorldState::bind(const WorldData& data)
  {
    hero.spot.location = { 10712.0f, 56792.0f }; // TODO: remove in the future
  }

}
