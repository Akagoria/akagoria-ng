#include <cassert>

#include <filesystem>

#include <gf2/core/Clock.h>
#include <gf2/core/Log.h>
#include <gf2/core/Polygon.h>
#include <gf2/core/StringUtils.h>

#include "bits/WorldData.h"

#include "config.h"

namespace {

  void copy_textures(const gf::TiledMapResource& map, const std::filesystem::path& raw_directory, const std::filesystem::path& out_directory)
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


}

int main() {
  gf::Clock clock;

  const std::filesystem::path assets_directory = akgr::AkagoriaDataDirectory;
  const std::filesystem::path raw_directory = assets_directory / "raw";
  const std::filesystem::path out_directory = assets_directory / "akagoria";

  akgr::WorldData data;

  gf::Log::info("# Reading map...");
  data.map = gf::TiledMapResource(raw_directory / "akagoria.tmx");

  copy_textures(data.map, raw_directory, out_directory);

  data.save_to_file(out_directory / "akagoria.dat");

  auto duration = clock.elapsed_time();
  gf::Log::info("Data successfully compiled in {:g} seconds", duration.as_seconds());

  auto size = std::filesystem::file_size(out_directory / "akagoria.dat");
  const double size_in_kib = double(size) / 1024.0;
  const double size_in_mib = size_in_kib / 1024.0;
  gf::Log::info("Archive size: {} bytes, {:.3f} KiB, {:.3f} MiB", size, size_in_kib, size_in_mib);
}
