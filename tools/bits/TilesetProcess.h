#ifndef TILESET_PROCESS_H
#define TILESET_PROCESS_H

#include <filesystem>

#include <gf2/core/Array2D.h>
#include <gf2/core/Image.h>
#include <gf2/core/Random.h>
#include <gf2/core/Vec2.h>

#include "TilesetData.h"
#include "TilesetGeneration.h"

namespace akgr {

  struct Colors {
    gf::Array2D<gf::Color> data;

    Colors() = default;
    Colors(gf::Vec2I size);

    gf::Color& operator()(gf::Vec2I pos) { return data(pos); }
    gf::Color operator()(gf::Vec2I pos) const { return data(pos); }

    Colors extend(int space) const;
    void blit(const Colors& source, gf::Vec2I offset);
    gf::Image create_image() const;
  };


  Colors colorize_tile(const Tile& tile, gf::Random& random, const TilesetData& db);

  gf::Image generate_atom_preview(const Atom& atom, gf::Random& random, const TileSettings& settings);
  gf::Image generate_wang2_preview(const Wang2& wang, gf::Random& random, const TilesetData& db);
  gf::Image generate_wang3_preview(const Wang3& wang, gf::Random& random, const TilesetData& db);

  gf::Image generate_empty_atom(const TileSettings& settings);
  gf::Image generate_empty_wang2(const TileSettings& settings);
  gf::Image generate_empty_wang3(const TileSettings& settings);


  struct DecoratedTileset {
    std::vector<Tileset> atoms;
    std::vector<Tileset> wang2;
    std::vector<Tileset> wang3;

    gf::Vec2I find_terrain_position(gf::Id id) const;
  };

  DecoratedTileset generate_tilesets(gf::Random& random, const TilesetData& db);

  gf::Image generate_tileset_image(gf::Random& random, const TilesetData& db, const DecoratedTileset& tilesets);
  std::string generate_tileset_xml(const std::filesystem::path& image, const TilesetData& db, const DecoratedTileset& tilesets);

}

#endif // TILESET_PROCESS_H
