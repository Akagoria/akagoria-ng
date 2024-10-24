#ifndef TILESET_GENERATION_H
#define TILESET_GENERATION_H

#include <gf2/core/Array2D.h>
#include <gf2/core/Geometry.h>
#include <gf2/core//Id.h>
#include <gf2/core//Image.h>
#include <gf2/core//Random.h>
#include <gf2/core//Vec2.h>

#include "TilesetData.h"

namespace akgr {

  struct Pixels {
    gf::Array2D<gf::Id> data;

    Pixels() = default;
    Pixels(gf::Vec2I size, gf::Id biome);

    gf::Id& operator()(gf::Vec2I pos) { return data(pos); }
    gf::Id operator()(gf::Vec2I pos) const { return data(pos); }

    void fill_from(gf::Vec2I start, gf::Id biome);
    void check_holes();
  };

  struct Origin {
    Origin() = default;

    Origin(gf::Id id0)
    : count(1)
    , ids{ id0, gf::InvalidId, gf::InvalidId }
    {
    }

    Origin(gf::Id id0, gf::Id id1)
    : count(2)
    , ids{ id0, id1, gf::InvalidId }
    {
    }

    Origin(gf::Id id0, gf::Id id1, gf::Id id2)
    : count(3)
    , ids{ id0, id1, id2 }
    {
    }

    int count = 0;
    gf::Id ids[3] = { gf::InvalidId, gf::InvalidId, gf::InvalidId };
  };

  struct Fences {
    gf::SegmentI segments[2];
    int count = 0;
  };

  constexpr std::size_t TerrainTopLeft = 0;
  constexpr std::size_t TerrainTopRight = 1;
  constexpr std::size_t TerrainBottomLeft = 2;
  constexpr std::size_t TerrainBottomRight = 3;

  struct Tile {
    Tile() = default;
    Tile(const TileSettings& settings, gf::Id biome = gf::InvalidId);

    void check_holes() {
      pixels.check_holes();
    }

    Origin origin;
    Pixels pixels;
    Fences fences;
    std::vector<gf::Polyline> limits;
    std::array<gf::Id, 4> terrain = { gf::InvalidId, gf::InvalidId, gf::InvalidId, gf::InvalidId };
  };

  struct Tileset {
    gf::Array2D<Tile> tiles;
    gf::Vec2I position;

    Tileset(gf::Vec2I size);

    Tile& operator()(gf::Vec2I pos) { return tiles(pos); }
    const Tile& operator()(gf::Vec2I pos) const { return tiles(pos); }
  };

  Tile generate_full(const TileSettings& settings, gf::Id b0);

  enum class Split : uint8_t {
    Horizontal, // left + right
    Vertical,   // top + bottom
  };

  // b0 is in the left|top, b1 is in the right|bottom
  Tile generate_split(const TileSettings& settings, gf::Id b0, gf::Id b1, Split s, gf::Random& random, const Edge& edge);

  enum class Corner : uint8_t {
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
  };

  // b0 is in the corner, b1 is in the rest
  Tile generate_corner(const TileSettings& settings, gf::Id b0, gf::Id b1, Corner c, gf::Random& random, const Edge& edge);

  // b0 is in top-left and bottom-right, b1 is in top-right and bottom-left
  Tile generate_cross(const TileSettings& settings, gf::Id b0, gf::Id b1, gf::Random& random, const Edge& edge);

  enum class HSplit : uint8_t {
    Top,
    Bottom,
  };

  // b0 is given by split, b1 is at the left, b2 is at the right
  Tile generate_horizontal_split(const TileSettings& settings, gf::Id b0, gf::Id b1, gf::Id b2, HSplit split, gf::Random& random, const Edge& e01, const Edge& e12, const Edge& e20);

  enum class VSplit : uint8_t {
    Left,
    Right,
  };

  // b0 is given by split, b1 is at the top, b2 is at the bottom
  Tile generate_vertical_split(const TileSettings& settings, gf::Id b0, gf::Id b1, gf::Id b2, VSplit split, gf::Random& random, const Edge& e01, const Edge& e12, const Edge& e20);

  enum class Oblique : uint8_t {
    Up,
    Down,
  };

  // b0 is given by oblique, b1 is at the left and b2 is at the right
  Tile generate_oblique(const TileSettings& settings, gf::Id b0, gf::Id b1, gf::Id b2, Oblique oblique, gf::Random& random, const Edge& e01, const Edge& e12, const Edge& e20);


  Tileset generate_plain_tileset(gf::Id b0, const TilesetData& db);
  Tileset generate_two_corners_wang_tileset(const Wang2& wang, gf::Random& random, const TilesetData& db);
  Tileset generate_three_corners_wang_tileset(const Wang3& wang, gf::Random& random, const TilesetData& db);


}

#endif // TILESET_GENERATION_H
