#include <cassert>

#include <filesystem>
#include <fstream>

#include <nlohmann/json.hpp>

#include <gf2/core/Clock.h>
#include <gf2/core/Log.h>
#include <gf2/core/Polygon.h>
#include <gf2/core/StringUtils.h>

#include "bits/DataLabel.h"
#include "bits/DataLexicon.h"
#include "bits/WorldData.h"

#include "config.h"

using namespace gf::literals;

namespace gf {

  void from_json(const nlohmann::json& json, Time& time) {
    int32_t ms = 0;
    json.get_to(ms);
    time = gf::milliseconds(ms);
  }

}

namespace akgr {

  void from_json(const nlohmann::json& json, DataLabel& label) {
    if (json.is_null()) {
      label.tag = "";
      label.id = gf::InvalidId;
    } else {
      json.get_to(label.tag);
      label.id = gf::hash_string(label.tag);
    }
  }

  // Dialog

  void from_json(const nlohmann::json& j, DialogLine& data) {
    j.at("speaker").get_to(data.speaker);
    j.at("words").get_to(data.words);
  }

  NLOHMANN_JSON_SERIALIZE_ENUM(DialogType, {
    { DialogType::Simple, "Simple" },
    { DialogType::Quest, "Quest" },
    { DialogType::Story, "Story" },
  })

  void from_json(const nlohmann::json& j, DialogData& data) {
    j.at("label").get_to(data.label);
    j.at("type").get_to(data.type);
    j.at("content").get_to(data.content);
  }

  // Notification

  void from_json(const nlohmann::json& json, NotificationData& data) {
    json.at("label").get_to(data.label);
    json.at("message").get_to(data.message);
    json.at("duration").get_to(data.duration);
  }

}


namespace {

  void copy_textures(const gf::TiledMap& map, const std::filesystem::path& raw_directory, const std::filesystem::path& out_directory)
  {
    gf::Log::info("# Copying textures...");
    for (const auto& texture : map.textures) {
      const std::filesystem::path relative_path = std::filesystem::proximate(texture.string(), raw_directory);
      gf::Log::info("\t- Copying texture '{}'...", relative_path.string());

      std::filesystem::path relative_directory = relative_path;
      relative_directory.remove_filename();
      std::filesystem::create_directories(out_directory / relative_directory);

      std::filesystem::copy_file(raw_directory / relative_path, out_directory / relative_path, std::filesystem::copy_options::overwrite_existing);
    }
  }

  void compile_json_dialogs(const std::filesystem::path& filename, akgr::DataLexicon<akgr::DialogData>& data, std::vector<std::string>& strings) {
    gf::Log::info("Reading '{}'...", filename.string());
    std::ifstream ifs(filename);
    nlohmann::json::parse(ifs).get_to(data);

    for (const auto& dialog : data) {
      for (const auto& line : dialog.content) {
        strings.push_back(line.words);
      }
    }

    akgr::data_lexicon_sort(data);
  }

  void compile_json_notifications(const std::filesystem::path& filename, akgr::DataLexicon<akgr::NotificationData>& data, std::vector<std::string>& strings) {
    gf::Log::info("Reading '{}'...", filename.string());
    std::ifstream ifs(filename);
    nlohmann::json::parse(ifs).get_to(data);

    for (const auto& notification : data) {
      strings.push_back(notification.message);
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

  copy_textures(data.map, raw_directory, out_directory);
  compile_json_dialogs(raw_directory / "database/dialogs.json", data.dialogs, strings);
  compile_json_notifications(raw_directory / "database/notifications.json", data.notifications, strings);

  data.save_to_file(out_directory / "akagoria.dat");

  auto duration = clock.elapsed_time();
  gf::Log::info("Data successfully compiled in {} ms", duration.as_milliseconds());

  auto size = std::filesystem::file_size(out_directory / "akagoria.dat");
  const double size_in_kib = double(size) / 1024.0;
  const double size_in_mib = size_in_kib / 1024.0;
  gf::Log::info("Archive size: {} bytes, {:.3f} KiB, {:.3f} MiB", size, size_in_kib, size_in_mib);
}
