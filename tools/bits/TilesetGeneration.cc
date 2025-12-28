#include "TilesetGeneration.h"

#include <queue>

#include <gf2/core/Math.h>
#include <gf2/core/ProcGen.h>

#include <gf2/core/Log.h>

namespace akgr {

  /*
   * Pixels
   */

  Pixels::Pixels(gf::Vec2I size, gf::Id biome)
  : data(size, biome)
  {
  }

  void Pixels::fill_from(gf::Vec2I start, gf::Id biome) {
    data(start) = biome;

    std::queue<gf::Vec2I> q;
    q.push(start);

    while (!q.empty()) {
      auto curr = q.front();
      assert(data(curr) == biome);

      for (auto next : data.compute_4_neighbors_range(curr)) {
        if (data(next) == gf::InvalidId) {
          data(next) = biome;
          q.push(next);
        }
      }

      q.pop();
    }
  }

  void Pixels::check_holes() {
    for (auto pos : data.position_range()) {
      gf::Id id = data(pos);

      if (id == gf::InvalidId) {
        auto& invalid = data(pos);

        for (auto next : data.compute_8_neighbors_range(pos)) {
          gf::Id next_biome = data(next);

          if (next_biome != gf::InvalidId) {
            invalid = next_biome;
          }
        }
      }

      assert(data(pos) != gf::InvalidId);
    }
  }


  /*
   * Tile
   */

  Tile::Tile(const TileSettings& settings, gf::Id biome)
  : pixels(settings.tile_size(), biome)
  {
  }

  /*
   * Tileset
   */

  Tileset::Tileset(gf::Vec2I size)
  : tiles(size)
  , position(-1, -1)
  {
  }

  /*
   * Tiles generators
   */

  namespace {

    constexpr gf::Vec2I top([[maybe_unused]] const TileSettings& settings, int i) {
      return { i, 0 };
    }

    constexpr gf::Vec2I fence_top(const TileSettings& settings, int i) {
      return top(settings, i);
    }

    constexpr gf::Vec2I bottom(const TileSettings& settings, int i) {
      return { i, settings.size - 1 };
    }

    constexpr gf::Vec2I fence_bottom(const TileSettings& settings, int i) {
      return { i, settings.size };
    }

    constexpr gf::Vec2I left([[maybe_unused]] const TileSettings& settings, int i) {
      return { 0, i };
    }

    constexpr gf::Vec2I fence_left(const TileSettings& settings, int i) {
      return left(settings, i);
    }

    constexpr gf::Vec2I right(const TileSettings& settings, int i) {
      return { settings.size - 1, i };
    }

    constexpr gf::Vec2I fence_right(const TileSettings& settings, int i) {
      return { settings.size, i };
    }

    constexpr gf::Vec2I corner_top_left([[maybe_unused]] const TileSettings& settings) {
      return { 0, 0 };
    }

    constexpr gf::Vec2I corner_top_right(const TileSettings& settings) {
      return { settings.size - 1, 0 };
    }

    constexpr gf::Vec2I corner_bottom_left(const TileSettings& settings) {
      return { 0, settings.size - 1 };
    }

    constexpr gf::Vec2I corner_bottom_right(const TileSettings& settings) {
      return { settings.size - 1, settings.size - 1 };
    }

    std::vector<gf::Vec2I> make_line(const TileSettings& settings, gf::Span<const gf::Vec2I> points, gf::Random& random, const Displacement& displacement) {
      // generate random line points

      std::vector<gf::Vec2I> tmp;

      for (std::size_t i = 0; i < points.size() - 1; ++i) {
        auto line = gf::midpoint_displacement_1d(points[i], points[i + 1], random, displacement.iterations, displacement.initial, displacement.reduction);
        tmp.insert(tmp.end(), line.begin(), line.end());
        tmp.pop_back();
      }

      tmp.push_back(points.back());

      // normalize

      for (auto& point : tmp) {
        point = gf::clamp(point, 0, settings.size - 1);
      }

      // compute final line

      std::vector<gf::Vec2I> out;

      for (std::size_t i = 0; i < tmp.size() - 1; ++i) {
        auto line = gf::generate_line(tmp[i], tmp[i + 1]);
        out.insert(out.end(), line.begin(), line.end());
      }

      out.push_back(tmp.back());

      return out;
    }

  }

  /*
   * Two Corner Wang Tileset generators
   */

  Tile generate_full(const TileSettings& settings, gf::Id b0) {
    Tile tile(settings, b0);
    tile.origin = Origin(b0);
    tile.terrain[TerrainTopLeft] = tile.terrain[TerrainTopRight] = tile.terrain[TerrainBottomLeft] = tile.terrain[TerrainBottomRight] = b0;
    return tile;
  }


  // b0 is in the left|top, b1 is in the right|bottom
  Tile generate_split(const TileSettings& settings, gf::Id b0, gf::Id b1, Split s, gf::Random& random, const Edge& edge) {
    Tile tile(settings);
    tile.origin = Origin(b0, b1);

    gf::Vec2I end_points[2];
    int half = settings.size / 2;

    switch (s) {
      case Split::Horizontal:
        end_points[0] = left(settings, half + edge.offset);
        end_points[1] = right(settings, half + edge.offset);
        break;
      case Split::Vertical:
        end_points[0] = top(settings, half + edge.offset);
        end_points[1] = bottom(settings, half + edge.offset);
        break;
    }

    auto line = make_line(settings, end_points, random, edge.displacement);

    for (auto point : line) {
      tile.pixels(point) = b1;
    }

    tile.pixels.fill_from(corner_top_left(settings), b0);
    tile.pixels.fill_from(corner_bottom_right(settings), b1);

    switch (s) {
      case Split::Horizontal:
        tile.terrain[TerrainTopLeft] = tile.terrain[TerrainTopRight] = b0;
        tile.terrain[TerrainBottomLeft] = tile.terrain[TerrainBottomRight] = b1;
        break;

      case Split::Vertical:
        tile.terrain[TerrainTopLeft] = tile.terrain[TerrainBottomLeft] = b0;
        tile.terrain[TerrainTopRight] = tile.terrain[TerrainBottomRight] = b1;
        break;
    }

    if (edge.limit) {
      tile.fences.count = 1;

      switch (s) {
      case Split::Horizontal:
        tile.fences.segments[0].p0 = fence_left(settings, half + edge.offset);
        tile.fences.segments[0].p1 = fence_right(settings, half + edge.offset);
        break;
      case Split::Vertical:
        tile.fences.segments[0].p0 = fence_top(settings, half + edge.offset);
        tile.fences.segments[0].p1 = fence_bottom(settings, half + edge.offset);
        break;
      }
    }

    tile.check_holes();
    return tile;
  }

  // b0 is in the corner, b1 is in the rest
  Tile generate_corner(const TileSettings& settings, gf::Id b0, gf::Id b1, Corner c, gf::Random& random, const Edge& edge) {
    Tile tile(settings);
    tile.origin = Origin(b0, b1);

    gf::Vec2I end_points[2];
    int half = settings.size / 2;

    switch (c) {
      case Corner::TopLeft:
        end_points[0] = top(settings, half - 1 + edge.offset);
        end_points[1] = left(settings, half - 1 + edge.offset);
        break;
      case Corner::TopRight:
        end_points[0] = top(settings, half - edge.offset);
        end_points[1] = right(settings, half - 1 + edge.offset);
        break;
      case Corner::BottomLeft:
        end_points[0] = bottom(settings, half - 1 + edge.offset);
        end_points[1] = left(settings, half - edge.offset);
        break;
      case Corner::BottomRight:
        end_points[0] = bottom(settings, half - edge.offset);
        end_points[1] = right(settings, half - edge.offset);
        break;
    }

    auto line = make_line(settings, end_points, random, edge.displacement);

    for (auto point : line) {
      tile.pixels(point) = b0;
    }

    switch (c) {
      case Corner::TopLeft:
        tile.pixels.fill_from(corner_top_left(settings), b0);
        tile.pixels.fill_from(corner_bottom_right(settings), b1);
        break;
      case Corner::TopRight:
        tile.pixels.fill_from(corner_top_right(settings), b0);
        tile.pixels.fill_from(corner_bottom_left(settings), b1);
        break;
      case Corner::BottomLeft:
        tile.pixels.fill_from(corner_bottom_left(settings), b0);
        tile.pixels.fill_from(corner_top_right(settings), b1);
        break;
      case Corner::BottomRight:
        tile.pixels.fill_from(corner_bottom_right(settings), b0);
        tile.pixels.fill_from(corner_top_left(settings), b1);
        break;
    }

    tile.terrain[TerrainTopLeft] = tile.terrain[TerrainTopRight] = tile.terrain[TerrainBottomLeft] = tile.terrain[TerrainBottomRight] = b1;

    switch (c) {
      case Corner::TopLeft:
        tile.terrain[TerrainTopLeft] = b0;
        break;
      case Corner::TopRight:
        tile.terrain[TerrainTopRight] = b0;
        break;
      case Corner::BottomLeft:
        tile.terrain[TerrainBottomLeft] = b0;
        break;
      case Corner::BottomRight:
        tile.terrain[TerrainBottomRight] = b0;
        break;
    }

    if (edge.limit) {
      tile.fences.count = 1;

      switch (c) {
        case Corner::TopLeft:
          tile.fences.segments[0].p0 = fence_top(settings, half + edge.offset);
          tile.fences.segments[0].p1 = fence_left(settings, half + edge.offset);
          break;
        case Corner::TopRight:
          tile.fences.segments[0].p0 = fence_top(settings, half - edge.offset);
          tile.fences.segments[0].p1 = fence_right(settings, half + edge.offset);
          break;
        case Corner::BottomLeft:
          tile.fences.segments[0].p0 = fence_bottom(settings, half + edge.offset);
          tile.fences.segments[0].p1 = fence_left(settings, half - edge.offset);
          break;
        case Corner::BottomRight:
          tile.fences.segments[0].p0 = fence_bottom(settings, half - edge.offset);
          tile.fences.segments[0].p1 = fence_right(settings, half - edge.offset);
          break;
      }
    }

    tile.check_holes();
    return tile;
  }

  // b0 is in top-left and bottom-right, b1 is in top-right and bottom-left
  Tile generate_cross(const TileSettings& settings, gf::Id b0, gf::Id b1, gf::Random& random, const Edge& edge) {
    Tile tile(settings);
    tile.origin = Origin(b0, b1);

    int half = settings.size / 2;

    gf::Vec2I limit_top_right[] = { top(settings, half + edge.offset), { half, half - 1 }, right(settings, half - 1 - edge.offset) };

    auto line_top_right = make_line(settings, limit_top_right, random, edge.displacement);

    for (auto point : line_top_right) {
      tile.pixels(point) = b1;
    }

    gf::Vec2I limit_bottom_left[] = { bottom(settings, half - 1 - edge.offset), { half - 1, half }, left(settings, half + edge.offset) };

    auto line_bottom_left = make_line(settings, limit_bottom_left, random, edge.displacement);

    for (auto point : line_bottom_left) {
      tile.pixels(point) = b1;
    }

    tile.pixels.fill_from(corner_top_left(settings), b0);
    tile.pixels.fill_from(corner_bottom_right(settings), b0);
    tile.pixels.fill_from(corner_top_right(settings), b1);
    tile.pixels.fill_from(corner_bottom_left(settings), b1);

    tile.terrain[TerrainTopLeft] = tile.terrain[TerrainBottomRight] = b0;
    tile.terrain[TerrainTopRight] = tile.terrain[TerrainBottomLeft] = b1;

    if (edge.limit) {
      tile.fences.count = 2;
      tile.fences.segments[0].p0 = fence_top(settings, half + edge.offset);
      tile.fences.segments[0].p1 = fence_right(settings, half - edge.offset);
      tile.fences.segments[1].p0 = fence_bottom(settings, half - edge.offset);
      tile.fences.segments[1].p1 = fence_left(settings, half + edge.offset);
    }

    tile.check_holes();
    return tile;
  }

  /*
   * Three Corner Wang Tileset generators
   */

  namespace {

    bool check_edges(const Edge& e01, const Edge& e12, const Edge& e20) {
      int count = 0;
      count += e01.limit ? 1 : 0;
      count += e12.limit ? 1 : 0;
      count += e20.limit ? 1 : 0;
      return count == 0 || count == 2;
    }

  }

  // b0 is given by split, b1 is at the left, b2 is at the right
  Tile generate_horizontal_split(const TileSettings& settings, gf::Id b0, gf::Id b1, gf::Id b2, HSplit split, gf::Random& random, const Edge& e01, const Edge& e12, const Edge& e20) {
    Tile tile(settings);
    tile.origin = Origin(b0, b1, b2);

    int half = settings.size / 2;

    gf::Vec2I p0;
    gf::Vec2I p1;
    gf::Vec2I p2;
    gf::Vec2I p3;

    if (split == HSplit::Top) {
      p0 = left(settings, half - 1 + e01.offset);
      p1 = right(settings, half - 1 - e20.offset);
      p2 = gf::vec(half, half -1 + ((e01.offset - e20.offset) / 2));
      p3 = bottom(settings, half + e12.offset);
    } else {
      p0 = left(settings, half - e01.offset);
      p1 = right(settings, half + e20.offset);
      p2 = gf::vec(half, half + ((e20.offset - e01.offset) / 2));
      p3 = top(settings, half + e12.offset);
    }

    gf::Vec2I segment_middle[] = { p2, p3 };
    auto line_middle = make_line(settings, segment_middle, random, e12.displacement);

    for (auto point : line_middle) {
      tile.pixels(point) = b2;
    }

    gf::Vec2I segment_left[] = { p0, p2 };
    auto line_left = make_line(settings, segment_left, random, e01.displacement);

    for (auto point : line_left) {
      tile.pixels(point) = b0;
    }

    gf::Vec2I segment_right[] = { p1, p2 };
    auto line_right = make_line(settings, segment_right, random, e20.displacement);

    for (auto point : line_right) {
      tile.pixels(point) = b0;
    }

    if (split == HSplit::Top) {
      tile.pixels.fill_from(top(settings, half), b0);
      tile.pixels.fill_from(corner_bottom_left(settings), b1);
      tile.pixels.fill_from(corner_bottom_right(settings), b2);
    } else {
      tile.pixels.fill_from(bottom(settings, half), b0);
      tile.pixels.fill_from(corner_top_left(settings), b1);
      tile.pixels.fill_from(corner_top_right(settings), b2);
    }

    if (split == HSplit::Top) {
      tile.terrain[TerrainTopLeft] = tile.terrain[TerrainTopRight] = b0;
      tile.terrain[TerrainBottomLeft] = b1;
      tile.terrain[TerrainBottomRight] = b2;
    } else {
      tile.terrain[TerrainBottomLeft] = tile.terrain[TerrainBottomRight] = b0;
      tile.terrain[TerrainTopLeft] = b1;
      tile.terrain[TerrainTopRight] = b2;
    }

    if (check_edges(e01, e12, e20)) {

      if (e01.limit && e12.limit) {
        if (split == HSplit::Top) {
          tile.fences.segments[tile.fences.count].p0 = fence_left(settings, half + e01.offset);
          tile.fences.segments[tile.fences.count].p1 = fence_bottom(settings, half + e12.offset);
        } else {
          tile.fences.segments[tile.fences.count].p0 = fence_left(settings, half - e01.offset);
          tile.fences.segments[tile.fences.count].p1 = fence_top(settings, half + e12.offset);
        }

        ++tile.fences.count;
      }

      if (e12.limit && e20.limit) {
        if (split == HSplit::Top) {
          tile.fences.segments[tile.fences.count].p0 = fence_right(settings, half - e20.offset);
          tile.fences.segments[tile.fences.count].p1 = fence_bottom(settings, half + e12.offset);
        } else {
          tile.fences.segments[tile.fences.count].p0 = fence_right(settings, half + e20.offset);
          tile.fences.segments[tile.fences.count].p1 = fence_top(settings, half + e12.offset);
        }

        ++tile.fences.count;
      }

      if (e20.limit && e01.limit) {
        if (split == HSplit::Top) {
          tile.fences.segments[tile.fences.count].p0 = fence_left(settings, half + e01.offset);
          tile.fences.segments[tile.fences.count].p1 = fence_right(settings, half - e20.offset);
        } else {
          tile.fences.segments[tile.fences.count].p0 = fence_left(settings, half - e01.offset);
          tile.fences.segments[tile.fences.count].p1 = fence_right(settings, half + e20.offset);
        }

        ++tile.fences.count;
      }

    }

    tile.check_holes();
    return tile;
  }

  // b0 is given by split, b1 is at the top, b2 is at the bottom
  Tile generate_vertical_split(const TileSettings& settings, gf::Id b0, gf::Id b1, gf::Id b2, VSplit split, gf::Random& random, const Edge& e01, const Edge& e12, const Edge& e20) {
    Tile tile(settings);
    tile.origin = Origin(b0, b1, b2);

    int half = settings.size / 2;

    gf::Vec2I p0;
    gf::Vec2I p1;
    gf::Vec2I p2;
    gf::Vec2I p3;

    if (split == VSplit::Left) {
      p0 = top(settings, half - 1 + e01.offset);
      p1 = bottom(settings, half - 1 - e20.offset);
      p2 = gf::vec(half - 1 + ((e01.offset - e20.offset) / 2), half);
      p3 = right(settings, half + e12.offset);
    } else {
      p0 = top(settings, half - e01.offset);
      p1 = bottom(settings, half + e20.offset);
      p2 = gf::vec(half + ((e20.offset - e01.offset) / 2), half);
      p3 = left(settings, half + e12.offset);
    }

    gf::Vec2I segment_middle[] = { p2, p3 };
    auto line_middle = make_line(settings, segment_middle, random, e12.displacement);

    for (auto point : line_middle) {
      tile.pixels(point) = b2;
    }

    gf::Vec2I segment_top[] = { p0, p2 };
    auto line_top = make_line(settings, segment_top, random, e01.displacement);

    for (auto point : line_top) {
      tile.pixels(point) = b0;
    }

    gf::Vec2I segment_bottom[] = { p1, p2 };
    auto line_bottom = make_line(settings, segment_bottom, random, e20.displacement);

    for (auto point : line_bottom) {
      tile.pixels(point) = b0;
    }

    if (split == VSplit::Left) {
      tile.pixels.fill_from(left(settings, half), b0);
      tile.pixels.fill_from(corner_top_right(settings), b1);
      tile.pixels.fill_from(corner_bottom_right(settings), b2);
    } else {
      tile.pixels.fill_from(right(settings, half), b0);
      tile.pixels.fill_from(corner_top_left(settings), b1);
      tile.pixels.fill_from(corner_bottom_left(settings), b2);
    }

    if (split == VSplit::Left) {
      tile.terrain[TerrainTopLeft] = tile.terrain[TerrainBottomLeft] = b0;
      tile.terrain[TerrainTopRight] = b1;
      tile.terrain[TerrainBottomRight] = b2;
    } else {
      tile.terrain[TerrainTopRight] = tile.terrain[TerrainBottomRight] = b0;
      tile.terrain[TerrainTopLeft] = b1;
      tile.terrain[TerrainBottomLeft] = b2;
    }

    if (check_edges(e01, e12, e20)) {

      if (e01.limit && e12.limit) {
        if (split == VSplit::Left) {
          tile.fences.segments[tile.fences.count].p0 = fence_top(settings, half + e01.offset);
          tile.fences.segments[tile.fences.count].p1 = fence_right(settings, half + e12.offset);
        } else {
          tile.fences.segments[tile.fences.count].p0 = fence_top(settings, half - e01.offset);
          tile.fences.segments[tile.fences.count].p1 = fence_left(settings, half + e12.offset);
        }

        ++tile.fences.count;
      }

      if (e12.limit && e20.limit) {
        if (split == VSplit::Left) {
          tile.fences.segments[tile.fences.count].p0 = fence_bottom(settings, half - e20.offset);
          tile.fences.segments[tile.fences.count].p1 = fence_right(settings, half + e12.offset);
        } else {
          tile.fences.segments[tile.fences.count].p0 = fence_bottom(settings, half + e20.offset);
          tile.fences.segments[tile.fences.count].p1 = fence_left(settings, half + e12.offset);
        }

        ++tile.fences.count;
      }

      if (e20.limit && e01.limit) {
        if (split == VSplit::Left) {
          tile.fences.segments[tile.fences.count].p0 = fence_top(settings, half + e01.offset);
          tile.fences.segments[tile.fences.count].p1 = fence_bottom(settings, half - e20.offset);
        } else {
          tile.fences.segments[tile.fences.count].p0 = fence_top(settings, half - e01.offset);
          tile.fences.segments[tile.fences.count].p1 = fence_bottom(settings, half + e20.offset);
        }

        ++tile.fences.count;
      }

    }

    tile.check_holes();
    return tile;
  }

  // b0 is given by oblique, b1 is at the left and b2 is at the right
  Tile generate_oblique(const TileSettings& settings, gf::Id b0, gf::Id b1, gf::Id b2, Oblique oblique, gf::Random& random, const Edge& e01, const Edge& e20) {
    Tile tile(settings);
    tile.origin = Origin(b0, b1, b2);

    int half = settings.size / 2;

    gf::Vec2I p0;
    gf::Vec2I p1;
    gf::Vec2I p2;
    gf::Vec2I p3;

    if (oblique == Oblique::Up) {
      p0 = left(settings, half - e01.offset);
      p1 = top(settings, half - e01.offset);
      p2 = right(settings, half - 1 - e20.offset);
      p3 = bottom(settings, half - 1 - e20.offset);
    } else {
      p0 = left(settings, half - 1 + e01.offset);
      p1 = bottom(settings, half - 1 - e01.offset);
      p2 = right(settings, half + e20.offset);
      p3 = top(settings, half - e20.offset);
    }

    gf::Vec2I segment_left[] = { p0, p1 };
    auto line_left = make_line(settings, segment_left, random, e01.displacement);

    for (auto point : line_left) {
      tile.pixels(point) = b0;
    }

    gf::Vec2I segment_right[] = { p2, p3 };
    auto line_right = make_line(settings, segment_right, random, e20.displacement);

    for (auto point : line_right) {
      tile.pixels(point) = b0;
    }

    if (oblique == Oblique::Up) {
      tile.pixels.fill_from(corner_bottom_left(settings), b0);
      tile.pixels.fill_from(corner_top_left(settings), b1);
      tile.pixels.fill_from(corner_bottom_right(settings), b2);
    } else {
      tile.pixels.fill_from(corner_top_left(settings), b0);
      tile.pixels.fill_from(corner_bottom_left(settings), b1);
      tile.pixels.fill_from(corner_top_right(settings), b2);
    }

    if (oblique == Oblique::Up) {
      tile.terrain[TerrainBottomLeft] = tile.terrain[TerrainTopRight] = b0;
      tile.terrain[TerrainTopLeft] = b1;
      tile.terrain[TerrainBottomRight] = b2;
    } else {
      tile.terrain[TerrainTopLeft] = tile.terrain[TerrainBottomRight] = b0;
      tile.terrain[TerrainBottomLeft] = b1;
      tile.terrain[TerrainTopRight] = b2;
    }

    if (e01.limit) {
      if (oblique == Oblique::Up) {
        tile.fences.segments[tile.fences.count].p0 = fence_left(settings, half - e01.offset);
        tile.fences.segments[tile.fences.count].p1 = fence_top(settings, half - e01.offset);
      } else {
        tile.fences.segments[tile.fences.count].p0 = fence_left(settings, half + e01.offset);
        tile.fences.segments[tile.fences.count].p1 = fence_bottom(settings, half - e01.offset);
      }

      ++tile.fences.count;
    }

    if (e20.limit) {
      if (oblique == Oblique::Up) {
        tile.fences.segments[tile.fences.count].p0 = fence_right(settings, half - e20.offset);
        tile.fences.segments[tile.fences.count].p1 = fence_bottom(settings, half - e20.offset);
      } else {
        tile.fences.segments[tile.fences.count].p0 = fence_right(settings, half + e20.offset);
        tile.fences.segments[tile.fences.count].p1 = fence_top(settings, half - e20.offset);
      }

      ++tile.fences.count;
    }

    tile.check_holes();
    return tile;
  }

  /*
   * Plain
   */

  Tileset generate_plain_tileset(gf::Id b0, const TilesetData& db) {
    Tileset tileset({ AtomsTilesetSize, AtomsTilesetSize });

    for (int i = 0; i < AtomsTilesetSize; ++i) {
      for (int j = 0; j < AtomsTilesetSize; ++j) {
        tileset({ i, j }) = generate_full(db.settings.tile, b0);
      }
    }

    return tileset;
  }

  /*
   *    0    1    2    3
   *  0 +----+----+----+----+
   *    |    |  ##|##  |    |
   *    |##  |  ##|####|####|
   *  1 +----+----+----+----+
   *    |##  |  ##|####|####|
   *    |  ##|####|####|##  |
   *  2 +----+----+----+----+
   *    |  ##|####|####|##  |
   *    |    |    |  ##|##  |
   *  3 +----+----+----+----+
   *    |    |    |  ##|##  |
   *    |    |  ##|##  |    |
   *    +----+----+----+----+
   *
   *    b0 = ' '
   *    b1 = '#'
   */

  Tileset generate_two_corners_wang_tileset(const Wang2& wang, gf::Random& random, const TilesetData& db) {
    Tileset tileset({ Wang2TilesetSize, Wang2TilesetSize });

    auto b0 = wang.borders[0].id.hash;
    auto b1 = wang.borders[1].id.hash;
    auto edge = wang.edge;
    const auto& settings = db.settings.tile;

    tileset({ 0, 0 }) = generate_corner(settings, b1, b0, Corner::BottomLeft, random, edge.invert());
    tileset({ 0, 1 }) = generate_cross(settings, b1, b0, random, edge.invert());
    tileset({ 0, 2 }) = generate_corner(settings, b1, b0, Corner::TopRight, random, edge.invert());
    tileset({ 0, 3 }) = generate_full(settings, b0);

    tileset({ 1, 0 }) = generate_split(settings, b0, b1, Split::Vertical, random, edge);
    tileset({ 1, 1 }) = generate_corner(settings, b0, b1, Corner::TopLeft, random, edge);
    tileset({ 1, 2 }) = generate_split(settings, b1, b0, Split::Horizontal, random, edge.invert());
    tileset({ 1, 3 }) = generate_corner(settings, b1, b0, Corner::BottomRight, random, edge.invert());

    tileset({ 2, 0 }) = generate_corner(settings, b0, b1, Corner::TopRight, random, edge);
    tileset({ 2, 1 }) = generate_full(settings, b1);
    tileset({ 2, 2 }) = generate_corner(settings, b0, b1, Corner::BottomLeft, random, edge);
    tileset({ 2, 3 }) = generate_cross(settings, b0, b1, random, edge);

    tileset({ 3, 0 }) = generate_split(settings, b0, b1, Split::Horizontal, random, edge);
    tileset({ 3, 1 }) = generate_corner(settings, b0, b1, Corner::BottomRight, random, edge);
    tileset({ 3, 2 }) = generate_split(settings, b1, b0, Split::Vertical, random, edge.invert());
    tileset({ 3, 3 }) = generate_corner(settings, b1, b0, Corner::TopLeft, random, edge.invert());

    return tileset;
  }

  /*
   *   0    1    2    3    4    5
   * 0 +----+----+----+----+----+----+
   *   |####|####|##::|::::|::  |  ##|
   *   |::  |  ::|::  |  ##|##  |  ::|
   * 1 +----+----+----+----+----+----+
   *   |::  |  ::|::  |  ##|##  |  ::|
   *   |::##|##  |  ##|##::|::##|##::|
   * 2 +----+----+----+----+----+----+
   *   |::##|##  |  ##|##::|::##|##::|
   *   |  ::|::::|::##|##  |    |    |
   * 3 +----+----+----+----+----+----+
   *   |  ::|::::|::##|##  |    |    |
   *   |::##|##  |  ##|##::|::##|##::|
   * 4 +----+----+----+----+----+----+
   *   |::##|##  |  ##|##::|::##|##::|
   *   |::  |  ::|::  |  ##|##  |  ::|
   * 5 +----+----+----+----+----+----+
   *   |::  |  ::|::  |  ##|##  |  ::|
   *   |####|####|##::|::::|::  |  ##|
   *   +----+----+----+----+----+----+
   *
   *   This is the layout that is valid and minimizes the number of areas.
   *   b0 = ' '
   *   b1 = ':'
   *   b2 = '#'
   */

  Tileset generate_three_corners_wang_tileset(const Wang3& wang, gf::Random& random, const TilesetData& db) {
    Tileset tileset({ Wang3TilesetSize, Wang3TilesetSize });

    gf::Id b0 = wang.ids[0].hash;
    gf::Id b1 = wang.ids[1].hash;
    gf::Id b2 = wang.ids[2].hash;

    Edge edge01 = db.get_edge(b0, b1);
    Edge edge12 = db.get_edge(b1, b2);
    Edge edge20 = db.get_edge(b2, b0);

    tileset({ 0, 0 }) = generate_horizontal_split(db.settings.tile, b2, b1, b0, HSplit::Top, random, edge12.invert(), edge01.invert(), edge20.invert());
    tileset({ 0, 1 }) = generate_vertical_split(db.settings.tile, b1, b0, b2, VSplit::Left, random, edge01.invert(), edge20.invert(), edge12.invert());
    tileset({ 0, 2 }) = generate_oblique(db.settings.tile, b1, b0, b2, Oblique::Down, random, edge01.invert(), edge12.invert());
    tileset({ 0, 3 }) = generate_oblique(db.settings.tile, b1, b0, b2, Oblique::Up, random, edge01.invert(), edge12.invert());
    tileset({ 0, 4 }) = generate_vertical_split(db.settings.tile, b1, b2, b0, VSplit::Left, random, edge12, edge20, edge01);
    tileset({ 0, 5 }) = generate_horizontal_split(db.settings.tile, b2, b1, b0, HSplit::Bottom, random, edge12.invert(), edge01.invert(), edge20.invert());

    tileset({ 1, 0 }) = generate_horizontal_split(db.settings.tile, b2, b0, b1, HSplit::Top, random, edge20, edge01, edge12);
    tileset({ 1, 1 }) = generate_oblique(db.settings.tile, b0, b2, b1, Oblique::Down, random, edge20.invert(), edge01.invert());
    tileset({ 1, 2 }) = generate_horizontal_split(db.settings.tile, b1, b2, b0, HSplit::Bottom, random, edge12, edge20, edge01);
    tileset({ 1, 3 }) = generate_horizontal_split(db.settings.tile, b1, b2, b0, HSplit::Top, random, edge12, edge20, edge01);
    tileset({ 1, 4 }) = generate_oblique(db.settings.tile, b0, b2, b1, Oblique::Up, random, edge20.invert(), edge01.invert());
    tileset({ 1, 5 }) = generate_horizontal_split(db.settings.tile, b2, b0, b1, HSplit::Bottom, random, edge20, edge01, edge12);

    tileset({ 2, 0 }) = generate_oblique(db.settings.tile, b1, b2, b0, Oblique::Up, random, edge12, edge01);
    tileset({ 2, 1 }) = generate_oblique(db.settings.tile, b0, b1, b2, Oblique::Up, random, edge01, edge20);
    tileset({ 2, 2 }) = generate_vertical_split(db.settings.tile, b2, b0, b1, VSplit::Right, random, edge20, edge01, edge12);
    tileset({ 2, 3 }) = generate_vertical_split(db.settings.tile, b2, b1, b0, VSplit::Right, random, edge12.invert(), edge01.invert(), edge20.invert());
    tileset({ 2, 4 }) = generate_oblique(db.settings.tile, b0, b1, b2, Oblique::Down, random, edge01, edge20);
    tileset({ 2, 5 }) = generate_oblique(db.settings.tile, b1, b2, b0, Oblique::Down, random, edge12, edge01);

    tileset({ 3, 0 }) = generate_horizontal_split(db.settings.tile, b1, b0, b2, HSplit::Top, random, edge01.invert(), edge20.invert(), edge12.invert());
    tileset({ 3, 1 }) = generate_oblique(db.settings.tile, b2, b0, b1, Oblique::Up, random, edge20, edge12);
    tileset({ 3, 2 }) = generate_vertical_split(db.settings.tile, b2, b1, b0, VSplit::Left, random, edge12.invert(), edge01.invert(), edge20.invert());
    tileset({ 3, 3 }) = generate_vertical_split(db.settings.tile, b2, b0, b1, VSplit::Left, random, edge20, edge01, edge12);
    tileset({ 3, 4 }) = generate_oblique(db.settings.tile, b2, b0, b1, Oblique::Down, random, edge20, edge12);
    tileset({ 3, 5 }) = generate_horizontal_split(db.settings.tile, b1, b0, b2, HSplit::Bottom, random, edge01.invert(), edge20.invert(), edge12.invert());

    tileset({ 4, 0 }) = generate_vertical_split(db.settings.tile, b0, b1, b2, VSplit::Right, random, edge01, edge12, edge20);
    tileset({ 4, 1 }) = generate_oblique(db.settings.tile, b2, b1, b0, Oblique::Down, random, edge12.invert(), edge20.invert());
    tileset({ 4, 2 }) = generate_horizontal_split(db.settings.tile, b0, b1, b2, HSplit::Bottom, random, edge01, edge12, edge20);
    tileset({ 4, 3 }) = generate_horizontal_split(db.settings.tile, b0, b1, b2, HSplit::Top, random, edge01, edge12, edge20);
    tileset({ 4, 4 }) = generate_oblique(db.settings.tile, b2, b1, b0, Oblique::Up, random, edge12.invert(), edge20.invert());
    tileset({ 4, 5 }) = generate_vertical_split(db.settings.tile, b0, b2, b1, VSplit::Right, random, edge20.invert(), edge12.invert(), edge01.invert());

    tileset({ 5, 0 }) = generate_vertical_split(db.settings.tile, b0, b2, b1, VSplit::Left, random, edge20.invert(), edge12.invert(), edge01.invert());
    tileset({ 5, 1 }) = generate_vertical_split(db.settings.tile, b1, b0, b2, VSplit::Right, random, edge01.invert(), edge20.invert(), edge12.invert());
    tileset({ 5, 2 }) = generate_horizontal_split(db.settings.tile, b0, b2, b1, HSplit::Bottom, random, edge20.invert(), edge12.invert(), edge01.invert());
    tileset({ 5, 3 }) = generate_horizontal_split(db.settings.tile, b0, b2, b1, HSplit::Top, random, edge20.invert(), edge12.invert(), edge01.invert());
    tileset({ 5, 4 }) = generate_vertical_split(db.settings.tile, b1, b2, b0, VSplit::Right, random, edge12, edge20, edge01);
    tileset({ 5, 5 }) = generate_vertical_split(db.settings.tile, b0, b1, b2, VSplit::Left, random, edge01, edge12, edge20);

    return tileset;
  }

}
