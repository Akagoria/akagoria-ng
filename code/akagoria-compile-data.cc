#include <cassert>

#include <filesystem>
#include <fstream>
#include <unordered_set>

#include <fmt/std.h>
#include <nlohmann/json.hpp>

#include <gf2/core/Clock.h>
#include <gf2/core/Log.h>
#include <gf2/core/Polygon.h>
#include <gf2/core/StringUtils.h>

#include "bits/DataLabel.h"
#include "bits/DataLexicon.h"
#include "bits/DataReference.h"
#include "bits/WorldData.h"

#include "config.h"

using namespace gf::literals;

namespace gf {

  void from_json(const nlohmann::json& json, Time& time)
  {
    int32_t ms = 0;
    json.get_to(ms);
    time = gf::milliseconds(ms);
  }

  void from_json(const nlohmann::json& json, SpriteResource& sprite)
  {
    json.at("texture").get_to(sprite.texture);

    std::array<int, 3> region = {}; // index, width, height
    json.at("region").get_to(region);

    Vec2F position = vec(region[0] % region[1], region[0] / region[1]);
    Vec2F size = vec(region[1], region[2]);

    sprite.data.texture_region = RectF::from_position_size(position / size, 1.0f / size);
  }

}

namespace akgr {

  void from_json(const nlohmann::json& json, DataLabel& label)
  {
    if (json.is_null()) {
      label.tag = "";
      label.id = gf::InvalidId;
    } else {
      json.get_to(label.tag);
      label.id = gf::hash_string(label.tag);
    }
  }

  template<typename T>
  void from_json(const nlohmann::json& json, DataReference<T>& reference)
  {
    std::string label;
    json.get_to(label);
    reference.id = gf::hash_string(label);
    reference.tag = std::move(label);
  }

  // Character

  void from_json(const nlohmann::json& json, CharacterData& data)
  {
    json.at("label").get_to(data.label);
    json.at("animations").get_to(data.animations);
  }

  // Dialog

  void from_json(const nlohmann::json& j, DialogLine& data)
  {
    j.at("speaker").get_to(data.speaker);
    j.at("words").get_to(data.words);
  }

  NLOHMANN_JSON_SERIALIZE_ENUM(DialogType, {
    { DialogType::Simple, "Simple" },
    { DialogType::Quest, "Quest" },
    { DialogType::Story, "Story" },
  })

  void from_json(const nlohmann::json& json, DialogData& data)
  {
    json.at("label").get_to(data.label);
    json.at("type").get_to(data.type);
    json.at("content").get_to(data.content);
  }

  // Item

  NLOHMANN_JSON_SERIALIZE_ENUM(ItemType, {
    { ItemType::None, "None" },
    { ItemType::HeadArmor, "HeadArmor" },
    { ItemType::TorsoArmor, "TorsoArmor" },
    { ItemType::ArmsArmor, "ArmsArmor" },
    { ItemType::LegsArmor, "LegsArmor" },
    { ItemType::CamouflageCloth, "CamouflageCloth" },
    { ItemType::MeleeWeapon, "MeleeWeapon" },
    { ItemType::RangedWeapon, "RangedWeapon" },
    { ItemType::Explosive, "Explosive" },
    { ItemType::Potion, "Potion" },
    { ItemType::Ingredient, "Ingredient" },
    { ItemType::Recipes, "Recipes" },
    { ItemType::Book, "Book" },
    { ItemType::Parchment, "Parchment" },
    { ItemType::Quest, "Quest" },
  })

  void from_json(const nlohmann::json& json, ItemData& data)
  {
    json.at("label").get_to(data.label);
    json.at("type").get_to(data.type);
    json.at("description").get_to(data.description);
    json.at("sprite").get_to(data.sprite);
    json.at("scale").get_to(data.scale);
  }

  // Notification

  void from_json(const nlohmann::json& json, NotificationData& data)
  {
    json.at("label").get_to(data.label);
    json.at("message").get_to(data.message);
    json.at("duration").get_to(data.duration);
  }

  // Quest

  NLOHMANN_JSON_SERIALIZE_ENUM(QuestScope, {
    { QuestScope::History, "History" },
    { QuestScope::Long, "Long" },
    { QuestScope::Medium, "Medium" },
    { QuestScope::Short, "Short" },
  })

  NLOHMANN_JSON_SERIALIZE_ENUM(QuestType, {
    { QuestType::None, "None" },
    { QuestType::Hunt, "Hunt" },
    { QuestType::Talk, "Talk" },
    { QuestType::Farm, "Farm" },
    { QuestType::Explore, "Explore" },
  })

  void from_json(const nlohmann::json& json, HuntQuestData& data)
  {
    json.at("creature").get_to(data.creature);
    json.at("count").get_to(data.count);
  }

  void from_json(const nlohmann::json& json, TalkQuestData& data)
  {
    json.at("dialog").get_to(data.dialog);
  }

  void from_json(const nlohmann::json& json, FarmQuestData& data)
  {
    json.at("item").get_to(data.item);
    json.at("count").get_to(data.count);
  }

  void from_json(const nlohmann::json& json, ExploreQuestData& data)
  {
    json.at("location").get_to(data.location);
  }

  void from_json(const nlohmann::json& json, QuestStepData& data)
  {
    json.at("description").get_to(data.description);

    QuestType type = QuestType::None;
    json.at("type").get_to(type);

    switch (type) {
      case QuestType::None:
        data.features = {};
        break;
      case QuestType::Hunt:
        {
          HuntQuestData features_data;
          from_json(json, features_data);
          data.features = features_data;
        }
        break;
      case QuestType::Talk:
        {
          TalkQuestData features_data;
          from_json(json, features_data);
          data.features = features_data;
        }
        break;
      case QuestType::Farm:
        {
          FarmQuestData features_data;
          from_json(json, features_data);
          data.features = features_data;
        }
        break;
      case QuestType::Explore:
        {
          ExploreQuestData features_data;
          from_json(json, features_data);
          data.features = features_data;
        }
        break;
    }
  }

  void from_json(const nlohmann::json& json, QuestData& data)
  {
    json.at("label").get_to(data.label);
    json.at("title").get_to(data.title);
    json.at("description").get_to(data.description);
    json.at("scope").get_to(data.scope);
    json.at("steps").get_to(data.steps);
  }

}


namespace {

  void copy_textures(const std::filesystem::path& path, const std::filesystem::path& raw_directory, const std::filesystem::path& out_directory)
  {
    gf::Log::info("\tCopying texture '{}'...", path.string());

    std::filesystem::path relative_directory = path;
    relative_directory.remove_filename();
    std::filesystem::create_directories(out_directory / relative_directory);

    std::filesystem::copy_file(raw_directory / path, out_directory / path, std::filesystem::copy_options::overwrite_existing);
  }

  void copy_textures(const gf::TiledMap& map, const std::filesystem::path& raw_directory, const std::filesystem::path& out_directory)
  {
    for (const auto& texture : map.textures) {
      copy_textures(texture, raw_directory, out_directory);
    }
  }

  void copy_textures(const akgr::DataLexicon<akgr::ItemData>& data, const std::filesystem::path& raw_directory, const std::filesystem::path& out_directory)
  {
    std::unordered_set<std::filesystem::path> paths;

    for (const auto& item : data) {
      paths.insert(item.sprite.texture);
    }

    for (const auto& path : paths) {
      copy_textures(path, raw_directory, out_directory);
    }
  }

  void sanitize_paths(gf::TiledMap& map, const std::filesystem::path& raw_directory)
  {
    for (auto& texture : map.textures) {
      std::filesystem::path relative_path = std::filesystem::proximate(texture.string(), raw_directory);
      texture = std::move(relative_path);
    }
  }


  void compile_json_characters(const std::filesystem::path& filename, akgr::DataLexicon<akgr::CharacterData>& data) {
    gf::Log::info("\tReading '{}'...", filename.string());
    std::ifstream ifs(filename);
    nlohmann::json::parse(ifs).get_to(data);
    akgr::data_lexicon_sort(data);
  }

  void compile_json_dialogs(const std::filesystem::path& filename, akgr::DataLexicon<akgr::DialogData>& data, std::vector<std::string>& strings) {
    gf::Log::info("\tReading '{}'...", filename.string());
    std::ifstream ifs(filename);
    nlohmann::json::parse(ifs).get_to(data);

    for (const auto& dialog : data) {
      for (const auto& line : dialog.content) {
        strings.push_back(line.words);
      }
    }

    akgr::data_lexicon_sort(data);
  }

  void compile_json_items(const std::filesystem::path& filename, akgr::DataLexicon<akgr::ItemData>& data) {
    gf::Log::info("\tReading '{}'...", filename.string());
    std::ifstream ifs(filename);
    nlohmann::json::parse(ifs).get_to(data);
    akgr::data_lexicon_sort(data);
  }

  void compile_json_notifications(const std::filesystem::path& filename, akgr::DataLexicon<akgr::NotificationData>& data, std::vector<std::string>& strings) {
    gf::Log::info("\tReading '{}'...", filename.string());
    std::ifstream ifs(filename);
    nlohmann::json::parse(ifs).get_to(data);

    for (const auto& notification : data) {
      strings.push_back(notification.message);
    }

    akgr::data_lexicon_sort(data);
  }

  void compile_json_quests(const std::filesystem::path& filename, akgr::DataLexicon<akgr::QuestData>& data, std::vector<std::string>& strings) {
    gf::Log::info("\tReading '{}'...", filename.string());
    std::ifstream ifs(filename);
    nlohmann::json::parse(ifs).get_to(data);

    for (const auto& quest : data) {
      strings.push_back(quest.title);
      strings.push_back(quest.description);

      for (const auto& step : quest.steps) {
        strings.push_back(step.description);
      }
    }

    akgr::data_lexicon_sort(data);
  }

}

int main() {
  gf::Clock clock;

  const std::filesystem::path assets_directory = akgr::AkagoriaDataDirectory;
  const std::filesystem::path raw_directory = assets_directory / "raw";
  const std::filesystem::path out_directory = assets_directory / "akagoria";

  akgr::WorldData data;
  std::vector<std::string> strings;

  gf::Log::info("# Reading map...");

  data.map = gf::TiledMap(raw_directory / "akagoria.tmx");
  sanitize_paths(data.map, raw_directory);

  gf::Log::info("# Creating database...");

  compile_json_dialogs(raw_directory / "database/dialogs.json", data.dialogs, strings);
  compile_json_items(raw_directory / "database/items.json", data.items);
  compile_json_notifications(raw_directory / "database/notifications.json", data.notifications, strings);
  compile_json_characters(raw_directory / "database/characters.json", data.characters);
  compile_json_quests(raw_directory / "database/quests.json", data.quests, strings);

  data.save_to_file(out_directory / "akagoria.dat");

  gf::Log::info("# Copying textures...");

  copy_textures(data.map, raw_directory, out_directory);
  copy_textures(data.items, raw_directory, out_directory);

  auto duration = clock.elapsed_time();
  gf::Log::info("Data successfully compiled in {} ms", duration.as_milliseconds());

  auto size = std::filesystem::file_size(out_directory / "akagoria.dat");
  const double size_in_kib = double(size) / 1024.0;
  const double size_in_mib = size_in_kib / 1024.0;
  gf::Log::info("Archive size: {} bytes, {:.3f} KiB, {:.3f} MiB", size, size_in_kib, size_in_mib);
}
