#include "WorldData.h"

#include <gf2/core/Streams.h>
#include <gf2/core/Serialization.h>

#include "Version.h"

namespace akgr {

  void WorldData::load_from_file(const std::filesystem::path& filename)
  {
    gf::FileInputStream file(filename);
    gf::CompressedInputStream compressed(&file);
    gf::Deserializer ar(&compressed);

    ar | *this;
  }

  void WorldData::save_to_file(const std::filesystem::path& filename) const
  {
    gf::FileOutputStream file(filename);
    gf::CompressedOutputStream compressed(&file);
    gf::Serializer ar(&compressed, DataVersion);

    ar | *this;
  }

}
