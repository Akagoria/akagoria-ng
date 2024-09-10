#include "WorldRuntime.h"

#include <gf2/core/Log.h>

#include "Akagoria.h"

namespace akgr {

  WorldRuntime::WorldRuntime(Akagoria* game)
  : script(game)
  {
    physics.zone_handler.on_message.connect([this, game](const std::string& message, const std::set<gf::Id>& requirements) {
      auto& hero = game->world_state()->hero;

      if (std::includes(hero.requirements.begin(), hero.requirements.end(), requirements.begin(), requirements.end())) {
        script.on_message_deferred(message);
      }
    });
  }

  void WorldRuntime::bind(const WorldData& data, const WorldState& state)
  {
    script.bind();
    physics.bind(data, state);

    bind_map(data);
  }

  void WorldRuntime::bind_map(const WorldData& data)
  {
    const auto& map = data.map;
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

    data_lexicon_sort(locations);
  }

  void WorldRuntime::bind_map_object_layer(const gf::MapLayerStructure& layer, int32_t floor, const gf::TiledMap& map)
  {
    assert(layer.type == gf::MapLayerType::Object);

    const auto& object_layer = map.object_layers[layer.layer_index];
    const auto& object_properties = map.properties[object_layer.layer.properties_index];

    const auto& kind_property = object_properties("kind");
    assert(kind_property.is_string());
    const std::string kind = kind_property.as_string();

    if (kind == "locations") {
      for (const auto& object : object_layer.objects) {
        assert(object.type == gf::MapObjectType::Point);

        LocationRuntime location;
        location.label = object.name;
        location.spot.location = object.location;
        location.spot.floor = floor;
        locations.push_back(std::move(location));
      }
    }

  }

}
