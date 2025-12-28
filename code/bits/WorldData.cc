#include "WorldData.h"

#include <cassert>

#include <memory>

#include <gf2/core/Streams.h>
#include <gf2/core/Serialization.h>
#include <gf2/core/SerializationContainer.h>
#include <gf2/core/SerializationOps.h>
#include <gf2/core/SerializationUtilities.h>

#include "AreaData.h"
#include "Version.h"

namespace akgr {

  const AreaData* WorldData::compute_closest_area(gf::Vec2F location) const
  {
    auto distance = [location](const AreaData& area) {
      return gf::natural_distance(area.position, location);
    };

    auto iterator = std::min_element(areas.begin(), areas.end(), [&](const AreaData& lhs, const AreaData& rhs) {
      return distance(lhs) < distance(rhs);
    });

    if (iterator != areas.end()) {
      return std::addressof(*iterator);
    }

    return nullptr;
  }

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
    gf::HashedOutputStream hashed(&file);
    gf::CompressedOutputStream compressed(&hashed);
    gf::Serializer ar(&compressed, DataVersion);

    ar | *this;

    auto hash = hashed.hash();
    std::string digest;

    for (auto byte : hash) {
      digest += fmt::format("{:02X}", byte);
    }

    gf::Log::info("Hash: {}", digest);
  }

  void WorldData::bind()
  {
    bind_map();
    bind_quests();
  }

  void WorldData::bind_quests()
  {
    for (auto& quest : quests) {
      for (auto& step : quest.steps) {
        switch (step.type()) {
          case QuestType::None:
            break;
          case QuestType::Hunt:
            {
              auto& data  = std::get<HuntQuestData>(step.features);
              data.creature.bind_from(creatures);
              assert(data.creature.check());
            }
            break;
          case QuestType::Talk:
            {
              auto& data  = std::get<TalkQuestData>(step.features);
              data.dialog.bind_from(dialogs);
              assert(data.dialog.check());
            }
            break;
          case QuestType::Farm:
            {
              auto& data  = std::get<FarmQuestData>(step.features);
              data.item.bind_from(items);
              assert(data.item.check());
            }
            break;
          case QuestType::Explore:
            {
              auto& data  = std::get<ExploreQuestData>(step.features);
              data.location.bind_from(locations);
              assert(data.location.check());
            }
            break;
        }
      }
    }
  }

  void WorldData::bind_map()
  {
    for (const auto& layer : map.layers) {
      assert(layer.type == gf::MapLayerType::Group);

      const auto& group_layer = map.group_layers[layer.layer_index];
      const auto& group_properties = map.properties[group_layer.layer.properties_index];

      const auto& floor_property = group_properties("floor");
      assert(floor_property.is_int());
      const auto floor = static_cast<int32_t>(floor_property.as_int());

      for (const auto& sub_layer : group_layer.sub_layers) {
        switch (sub_layer.type) {
          case gf::MapLayerType::Object:
            bind_map_object_layer(sub_layer, floor, map);
            break;
          case gf::MapLayerType::Tile:
            break;
          case gf::MapLayerType::Group:
            assert(false);
            break;
        }
      }
    }

    data_lexicon_sort(areas);
    data_lexicon_sort(locations);
  }

  void WorldData::bind_map_object_layer(const gf::MapLayerStructure& layer, int32_t floor, const gf::TiledMap& map)
  {
    assert(layer.type == gf::MapLayerType::Object);

    const auto& object_layer = map.object_layers[layer.layer_index];
    const std::string& type = object_layer.layer.type;

    if (type == "areas") {
      for (const auto& object : object_layer.objects) {
        assert(object.type == gf::MapObjectType::Point);

        AreaData area;
        area.label = object.name;
        area.position = object.location;
        areas.push_back(std::move(area));
      }
    }

    if (type == "locations") {
      for (const auto& object : object_layer.objects) {
        assert(object.type == gf::MapObjectType::Point);

        LocationData location;
        location.label = object.name;
        location.spot.location = object.location;
        location.spot.floor = floor;
        locations.push_back(std::move(location));
      }
    }

  }

}
