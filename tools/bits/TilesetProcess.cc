#include "TilesetProcess.h"
#include <cstdint>

#include <sstream>
#include <map>

#include <gf2/core/Color.h>
#include <gf2/core/Geometry.h>

#include <gf2/core/Log.h>

namespace akgr {

  /*
   * Colors
   */

  Colors::Colors(gf::Vec2I size)
  : data(size, gf::Transparent)
  {
  }

  Colors Colors::extend(int space) const {
    auto size = data.size();
    Colors result(size + 2 * space);

    for (auto pos : result.data.position_range()) {
      auto source_pos = gf::clamp(pos - space, gf::vec(0, 0), size - 1);
      result.data(pos) = data(source_pos);
    }

    return result;
  }

  void Colors::blit(const Colors& source, gf::Vec2I offset) {
    auto src_size = source.data.size();
    auto dst_size = data.size();

    assert(offset.x >= 0);
    assert(offset.y >= 0);
    assert(offset.x + src_size.w <= dst_size.w);
    assert(offset.y + src_size.h <= dst_size.h);

    for (auto pos : source.data.position_range()) {
      data(offset + pos) = source.data(pos);
    }
  }

  gf::Image Colors::create_image() const {
    gf::Image image(data.size(), gf::Transparent);

    for (auto pos : data.position_range()) {
      image.put_pixel(pos, data(pos));
    }

    return image;
  }

  namespace {

    void colorize_atom(Colors& colors, const Atom& atom, const Tile& tile, gf::Random& random) {
      if (atom.id.hash == Void) {
        return;
      }

      switch (atom.pigment.style) {
        case PigmentStyle::Plain:
          for (auto pos : tile.pixels.data.position_range()) {
            if (tile.pixels(pos) != atom.id.hash) {
              continue;
            }

            colors(pos) = atom.color;
          }
          break;

        case PigmentStyle::Randomize: {
          for (auto pos : tile.pixels.data.position_range()) {
            if (tile.pixels(pos) != atom.id.hash) {
              continue;
            }

            colors(pos) = atom.color;
          }

          auto size = tile.pixels.data.size();
          int anomalies = (atom.pigment.randomize.ratio * float(size.w * size.h) / gf::square(atom.pigment.randomize.size)) + 1;

          for (int i = 0; i < anomalies; ++i) {
            gf::Vec2I pos = random.compute_position(gf::RectI::from_size(size - atom.pigment.randomize.size));

            gf::Id id = tile.pixels(pos);

            if (id != atom.id.hash) {
              continue;
            }

            float change = gf::clamp(random.compute_normal_float(0.0f, atom.pigment.randomize.deviation), -0.5f, 0.5f);
            auto modified = (change > 0) ? gf::darker(atom.color, change) : gf::lighter(atom.color, -change);

            gf::Vec2I offset;

            for (offset.y = 0; offset.y < atom.pigment.randomize.size; ++offset.y) {
              for (offset.x = 0; offset.x < atom.pigment.randomize.size; ++offset.x) {
                auto neighbor = pos + offset;
                assert(tile.pixels.data.valid(neighbor));

                if (tile.pixels(neighbor) == id) {
                  colors(neighbor) = modified;
                }
              }
            }
          }

          break;
        }

        case PigmentStyle::Striped:
          for (auto pos : tile.pixels.data.position_range()) {
            if (tile.pixels(pos) != atom.id.hash) {
              continue;
            }

            if ((pos.x + pos.y ) % atom.pigment.striped.stride < atom.pigment.striped.width) {
              colors(pos) = atom.color;
            } else {
              colors(pos) = atom.color * gf::opaque(0.0f);
            }
          }
          break;

        case PigmentStyle::Paved: {
          auto modulate_color = [&atom]() {
            if (atom.pigment.paved.modulation < 0.0f) {
              return gf::lighter(atom.color, -atom.pigment.paved.modulation);
            }

            return gf::darker(atom.color, atom.pigment.paved.modulation);
          };

          for (auto pos : tile.pixels.data.position_range()) {
            if (tile.pixels(pos) != atom.id.hash) {
              continue;
            }

            colors(pos) = atom.color;

            int y = pos.y + (atom.pigment.paved.width / 2);

            if (y % atom.pigment.paved.width == 0) {
              colors(pos) = modulate_color();
            } else {
              int x = pos.x + (atom.pigment.paved.length / 4);

              if (y / atom.pigment.paved.width % 2 == 0) {
                if (x % atom.pigment.paved.length == 0) {
                  colors(pos) = modulate_color();
                }
              } else {
                if (x % atom.pigment.paved.length == atom.pigment.paved.length / 2) {
                  colors(pos) = modulate_color();
                }
              }
            }
          }
          break;
        }
      }
    }


    void colorize_border(Colors& colors, const Colors& original_colors, const Wang2& wang, const Tile& tile, gf::Random& random, const TilesetData& db) {
      for (int i = 0; i < 2; ++i) {
        const auto& border = wang.borders[i];

        gf::Id id = border.id.hash;

        if (id == Void) {
          continue;
        }

        if (border.effect == BorderEffect::None) {
          continue;
        }

        Atom atom = db.get_atom(id);

        gf::Id other = wang.borders[1 - i].id.hash;

        for (auto pos : tile.pixels.data.position_range()) {
          if (tile.pixels(pos) != id) {
            continue;
          }

          int min_distance = 1'000'000;
          gf::Vec2I min_neighbor(-1, -1);

          for (auto neighbor : tile.pixels.data.position_range()) {
            if (tile.pixels(neighbor) != other) {
              continue;
            }

            int distance = gf::manhattan_distance(pos, neighbor);

            if (distance < min_distance) {
              min_distance = distance;
              min_neighbor = neighbor;
            }
          }

          auto color = original_colors(pos);
          bool changed = false;

          switch (border.effect) {
            case BorderEffect::Fade:
              if (min_distance <= border.fade.distance) {
                changed = true;
                color.a = gf::lerp(color.a, 0.0f, float(border.fade.distance - min_distance) / float(border.fade.distance));
              }
              break;

            case BorderEffect::Outline:
              if (min_distance <= border.outline.distance) {
                changed = true;
                color = gf::darker(atom.color, border.outline.factor);
              }
              break;

            case BorderEffect::Sharpen:
              if (min_distance <= border.sharpen.distance) {
                changed = true;
                color = gf::darker(color, float(border.sharpen.distance - min_distance) * 0.5f / float(border.sharpen.distance));
              }
              break;

            case BorderEffect::Lighten:
              if (min_distance <= border.sharpen.distance) {
                changed = true;
                color = gf::lighter(color, float(border.lighten.distance - min_distance) * 0.5f / float(border.lighten.distance));
              }
              break;

            case BorderEffect::Blur:
              if (min_distance < 5) {
                changed = true;

                // see https://en.wikipedia.org/wiki/Kernel_(image_processing)

                float final_coeff = 36.0f;
                gf::Color final_color = 36.0f * original_colors(pos);

                for (auto next : original_colors.data.compute_24_neighbors_range(pos)) {
                  gf::Color next_color = original_colors(next);
                  gf::Vec2I diff = gf::abs(pos - next);

                  if (diff == gf::Vec2I(1, 0) || diff == gf::Vec2I(0, 1)) {
                    final_color += 24.0f * next_color;
                    final_coeff += 24.0f;
                  } else if (diff == gf::Vec2I(1, 1)) {
                    final_color += 16.0f * next_color;
                    final_coeff += 16.0f;
                  } else if (diff == gf::Vec2I(2, 0) || diff == gf::Vec2I(0, 2)) {
                    final_color += 6.0f * next_color;
                    final_coeff += 6.0f;
                  } else if (diff == gf::Vec2I(2, 1) || diff == gf::Vec2I(1, 2)) {
                    final_color += 4.0f * next_color;
                    final_coeff += 4.0f;
                  } else if (diff == gf::Vec2I(2, 2)) {
                    final_color += 1.0f * next_color;
                    final_coeff += 1.0f;
                  } else {
                    assert(false);
                  }
                }

                color = final_color / final_coeff;
              }
              break;

            case BorderEffect::Blend:
              if (min_distance <= border.blend.distance) {
                float stop = 1.0f;

                if (wang.borders[1 - i].effect == BorderEffect::Blend) {
                  stop = 0.5f;
                }

                changed = true;
                color = gf::lerp(color, original_colors(min_neighbor), (stop * float(border.blend.distance - min_distance) / float(border.blend.distance)) + random.compute_uniform_float(0.0f, 0.05f));
              }
              break;

            default:
              assert(false);
              break;
          }

          if (changed) {
            colors(pos) = color;
          }
        }

      }
    }


    Colors colorize_raw_tile(const Tile& tile, gf::Random& random, const TilesetData& db, Search search) {
      Colors colors(tile.pixels.data.size());
      const auto& origin = tile.origin;

      // first pass: base biome color

      for (auto biome : origin.ids) {
        if (biome == Void || biome == gf::InvalidId) {
          continue;
        }

        auto atom = db.get_atom(biome, search);
        colorize_atom(colors, atom, tile, random);
      }

      // second pass: borders

      Colors original(colors);

      if (origin.count == 2) {
        colorize_border(colors, original, db.get_wang2(origin.ids[0], origin.ids[1], search), tile, random, db);
      } else if (origin.count == 3) {
        colorize_border(colors, original, db.get_wang2(origin.ids[0], origin.ids[1], search), tile, random, db);
        colorize_border(colors, original, db.get_wang2(origin.ids[1], origin.ids[2], search), tile, random, db);
        colorize_border(colors, original, db.get_wang2(origin.ids[2], origin.ids[0], search), tile, random, db);
      } else {
        assert(origin.count == 1);
      }

      return colors;
    }

  }

  Colors colorize_tile(const Tile& tile, gf::Random& random, const TilesetData& db) {
    return colorize_raw_tile(tile, random, db, Search::UseDatabaseOnly).extend(db.settings.tile.spacing);
  }

  gf::Image generate_atom_preview(const Atom& atom, gf::Random& random, const TileSettings& settings) {
    Tile tile = generate_full(settings, atom.id.hash);
    Colors colors(tile.pixels.data.size());
    colorize_atom(colors, atom, tile, random);
    return colors.create_image();
  }

  gf::Image generate_wang2_preview(const Wang2& wang, gf::Random& random, const TilesetData& db) {
    Tileset tileset = generate_two_corners_wang_tileset(wang, random, db);
    Colors colors(tileset.tiles.size() * (db.settings.tile.tile_size() + 1) - 1);

    for (auto pos : tileset.tiles.position_range()) {
      Colors tile_colors = colorize_raw_tile(tileset(pos), random, db, Search::IncludeTemporary);
      gf::Vec2I offset = pos * (db.settings.tile.tile_size() + 1);
      colors.blit(tile_colors, offset);
    }

    return colors.create_image();
  }

  gf::Image generate_wang3_preview(const Wang3& wang, gf::Random& random, const TilesetData& db) {
    Tileset tileset = generate_three_corners_wang_tileset(wang, random, db);
    Colors colors(tileset.tiles.size() * (db.settings.tile.tile_size() + 1) - 1);

    for (auto pos : tileset.tiles.position_range()) {
      Colors tile_colors = colorize_raw_tile(tileset(pos), random, db, Search::UseDatabaseOnly);
      gf::Vec2I offset = pos * (db.settings.tile.tile_size() + 1);
      colors.blit(tile_colors, offset);
    }

    return colors.create_image();
  }

  gf::Image generate_empty_atom(const TileSettings& settings)
  {
    return { settings.tile_size(), gf::Transparent };
  }

  gf::Image generate_empty_wang2(const TileSettings& settings)
  {
    return { Wang2TilesetSize * (settings.tile_size() + 1) - 1, gf::Transparent };
  }

  gf::Image generate_empty_wang3(const TileSettings& settings)
  {
    return { Wang3TilesetSize * (settings.tile_size() + 1) - 1, gf::Transparent };
  }

  /*
   * DecoratedTileset
   */

  gf::Vec2I DecoratedTileset::find_terrain_position(gf::Id id) const {
    for (const auto& tileset : atoms) {
      for (auto tile_position : tileset.tiles.position_range()) {
        const auto& tile = tileset(tile_position);

        if (tile.origin.count == 1 && tile.origin.ids[0] == id) {
          return tileset.position + tile_position;
        }
      }
    }

    gf::Log::error("Could not find a terrain for {:x}", static_cast<uint64_t>(id));
    return gf::vec(-1, -1);
  }


  DecoratedTileset generate_tilesets(gf::Random& random, const TilesetData& db) {
    DecoratedTileset tilesets;

    auto features = db.settings.image_features();

    gf::Vec2I global_position(0, 0);
    gf::Vec2I tileset_position(0, 0);

    for (const auto& atom : db.atoms) {
      auto tileset = generate_plain_tileset(atom.id.hash, db);
      tileset.position = global_position + tileset_position * AtomsTilesetSize;
      tilesets.atoms.push_back(std::move(tileset));

      ++tileset_position.x;

      if (tileset_position.x == features.atoms_per_line) {
        tileset_position.x = 0;
        ++tileset_position.y;
      }
    }

    global_position.y += features.atoms_line_count * AtomsTilesetSize;

    // wang2

    tileset_position = gf::vec(0, 0);

    for (const auto& wang : db.wang2) {
      auto tileset = generate_two_corners_wang_tileset(wang, random, db);
      tileset.position = global_position + tileset_position * Wang2TilesetSize;
      tilesets.wang2.push_back(std::move(tileset));

      ++tileset_position.x;

      if (tileset_position.x == features.wang2_per_line) {
        tileset_position.x = 0;
        ++tileset_position.y;
      }
    }

    global_position.y += features.wang2_line_count * Wang2TilesetSize;

    // wang3

    tileset_position = gf::vec(0, 0);

    for (const auto& wang : db.wang3) {
      auto tileset = generate_three_corners_wang_tileset(wang, random, db);
      tileset.position = global_position + tileset_position * Wang3TilesetSize;
      tilesets.wang3.push_back(std::move(tileset));

      ++tileset_position.x;

      if (tileset_position.x == features.wang3_per_line) {
        tileset_position.x = 0;
        ++tileset_position.y;
      }
    }

    return tilesets;
  }


  gf::Image generate_tileset_image(gf::Random& random, const TilesetData& db, const DecoratedTileset& tilesets) {
    auto features = db.settings.image_features();
    Colors main_colors(features.size);

    for (const auto& container : { tilesets.atoms, tilesets.wang2, tilesets.wang3 }) {
      for (const auto& tileset : container) {
        for (auto tile_position : tileset.tiles.position_range()) {
          Colors tile_colors = colorize_tile(tileset(tile_position), random, db);
          main_colors.blit(tile_colors, (tileset.position + tile_position) * db.settings.tile.extended_tile_size());
        }
      }
    }

    return main_colors.create_image();
  }


  namespace {

    template<typename T>
    struct KV {
      const char *key;
      T value;
    };

    template<typename T>
    KV<T> kv(const char * key, T value) {
      return { key, std::move(value) };
    }

    template<typename T>
    std::ostream& operator<<(std::ostream& os, const KV<T>& kv) {
      return os << kv.key << '=' << '"' << kv.value << '"';
    }

    std::string to_string(gf::Color color) {
      auto rgba32 = color.to_rgba32();
      return fmt::format("#{:x}{:x}{:x}", rgba32[0], rgba32[1], rgba32[2]);
    }

  }

  std::string generate_tileset_xml(const std::filesystem::path& image, const TilesetData& db, const DecoratedTileset& tilesets) {
    using namespace std::literals;

    std::map<gf::Id, std::size_t> mapping;

    for (std::size_t i = 0; i < db.atoms.size(); ++i) {
      mapping.emplace(db.atoms[i].id.hash, i + 1);
    }

    auto get_terrain_index = [&mapping](gf::Id id) {
      auto it = mapping.find(id);

      if (it != mapping.end()) {
        return std::to_string(it->second);
      }

      return "0"s;
    };

    auto features = db.settings.image_features();

    gf::Vec2I tile_count = features.size / db.settings.tile.extended_tile_size();

    auto position_to_index = [tile_count](gf::Vec2I position) {
      return (position.y * tile_count.w) + position.x;
    };

    std::ostringstream os;

    os << "<?xml " << kv("version", "1.0") << ' ' << kv("encoding", "UTF-8") << "?>\n";
    os << "<tileset " << kv("version", "1.5") << ' ' << kv("name", image.stem().string()) << ' '
        << kv("tilewidth", db.settings.tile.size) << ' ' << kv("tileheight", db.settings.tile.size) << ' '
        << kv("tilecount", tile_count.w * tile_count.h) << ' ' << kv("columns", tile_count.w) << ' '
        << kv("spacing", db.settings.tile.spacing * 2) << ' ' << kv("margin", db.settings.tile.spacing)
        << ">\n";

    os << " <transformations " << kv("hflip", 1) << ' ' << kv("vflip", 1) << ' ' << kv("rotate", 1) << ' ' << kv("preferuntransformed", 0) << " />\n";

    os << " <image " << kv("source", image.string()) << ' '
        << kv("width", features.size.w) << ' ' << kv("height", features.size.h)
        << "/>\n";

    for (const auto& container : { tilesets.atoms, tilesets.wang2, tilesets.wang3 }) {
      for (const auto& tileset : container) {
        for (auto tile_position : tileset.tiles.position_range()) {
          const auto& tile = tileset(tile_position);

          os << " <tile " << kv("id", position_to_index(tileset.position + tile_position));

          if (tile.fences.count > 0) {
            os << ">\n";
            os << "  <properties>\n";
            os << "   <property " << kv("name", "fence_count") << ' ' << kv("value", tile.fences.count) << ' ' << kv("type", "int") << "/>\n";

            char name[] = "fence#";

            for (int i = 0; i < tile.fences.count; ++i) {
              static constexpr char Sep = ',';

              name[5] = char('0' + i);
              os << "   <property name=\"" << std::begin(name) << "\" value=\""
                << tile.fences.segments[i][0].x << Sep
                << tile.fences.segments[i][0].y << Sep
                << tile.fences.segments[i][1].x << Sep
                << tile.fences.segments[i][1].y << "\" />\n";
            }

            os << "  </properties>\n";
            os << " </tile>\n";
          } else {
            os << "/>\n";
          }

        }
      }
    }


    os << " <wangsets>\n";
    os << "  <wangset " << kv("name", "Biomes") << ' ' << kv("type", "corner") << ' ' << kv("tile", -1) << ">\n";

    for (const auto& atom : db.atoms) {
      os << "   <wangcolor " << kv("name", atom.id.name) << ' '
          << kv("color", to_string(atom.color)) << ' '
          << kv("tile", position_to_index(tilesets.find_terrain_position(atom.id.hash))) << ' '
          << kv("probability", 1)
          << "/>\n";
    }

    for (const auto& container : { tilesets.atoms, tilesets.wang2, tilesets.wang3 }) {
      for (const auto& tileset : container) {
        for (auto tile_position : tileset.tiles.position_range()) {
          const auto& tile = tileset(tile_position);

          os << "   <wangtile tileid=\"" << position_to_index(tileset.position + tile_position) << "\" wangid=\""
            << "0," << get_terrain_index(tile.terrain[1]) << ',' // top right
            << "0," << get_terrain_index(tile.terrain[3]) << ',' // bottom right
            << "0," << get_terrain_index(tile.terrain[2]) << ',' // bottom left
            << "0," << get_terrain_index(tile.terrain[0])        // top left
            << "\"/>\n";
        }
      }
    }

    os << "  </wangset>\n";
    os << " </wangsets>\n";

    os << "</tileset>\n";

    return os.str();
  }

}
