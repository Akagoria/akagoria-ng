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
    if (hero.dialog.data) {
      hero.dialog.data.bind_from(data.dialogs);
      assert(hero.dialog.data.check());
    }

    for (auto& notification : notifications) {
      notification.data.bind_from(data.notifications);
      assert(notification.data.check());
    }

    for (auto& item : items) {
      item.data.bind_from(data.items);
      assert(item.data.check());
    }

    for (auto& character : characters) {
      character.data.bind_from(data.characters);
      assert(character.data.check());

      if (character.dialog) {
        character.dialog.bind_from(data.dialogs);
        assert(character.dialog.check());
      }
    }
  }

}
