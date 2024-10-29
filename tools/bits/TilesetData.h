#ifndef AKGR_TILESET_DATA_H
#define AKGR_TILESET_DATA_H

#include <filesystem>
#include <string>
#include <vector>

#include <gf2/core/Color.h>
#include <gf2/core/Id.h>
#include <gf2/core/Vec2.h>

using namespace gf::literals;

namespace akgr {

  constexpr gf::Id Void = "Void"_id;

  struct TileSettings {
    int size = 32;
    int spacing = 1;

    gf::Vec2I tile_size() const {
      return { size, size };
    }

    int extended_size() const {
      return size + (2 * spacing);
    }

    gf::Vec2I extended_tile_size() const {
      return { extended_size(), extended_size() };
    }
  };

  struct ImageFeatures {
    gf::Vec2I size;
    int atoms_per_line;
    int atoms_line_count;
    int wang2_per_line;
    int wang2_line_count;
    int wang3_per_line;
    int wang3_line_count;
  };

  struct Settings {
    bool locked = false;
    int max_atom_count = 64;
    int max_wang2_count = 48;
    int max_wang3_count = 32;
    TileSettings tile;

    gf::Vec2I image_size() const;
    ImageFeatures image_features() const;
  };

  enum class PigmentStyle : uint8_t {
    Plain,
    Randomize,
    Striped,
    Paved,
  };

  struct Pigment {
    PigmentStyle style = PigmentStyle::Plain;

    union {
      struct {
      } plain;
      struct {
        float ratio;
        float deviation;
        int size;
      } randomize;
      struct {
        int width;
        int stride;
      } striped;
      struct {
        int width;
        int length;
        float modulation;
      } paved;
    };
  };

  struct AtomId {
    gf::Id hash = Void;
    std::string name;
  };

  struct Atom {
    AtomId id;
    gf::Color color = gf::Transparent;
    Pigment pigment;
  };


  enum class BorderEffect : uint8_t {
    None,
    Fade,
    Outline,
    Sharpen,
    Lighten,
    Blur,
    Blend,
  };

  struct Border {
    AtomId id;
    BorderEffect effect = BorderEffect::None;

    union {
      struct {
      } none;
      struct {
        int distance;
      } fade;
      struct {
        int distance;
        float factor;
      } outline;
      struct {
        int distance;
        float max;
      } sharpen;
      struct {
        int distance;
        float max;
      } lighten;
      struct {
      } blur;
      struct {
        int distance;
      } blend;
    };
  };

  struct Displacement {
    int iterations = 2;
    float initial = 0.5f;
    float reduction = 0.5f;
  };

  struct Edge {
    int offset = 0;
    Displacement displacement;
    bool limit = false;

    Edge invert() const {
      return { -offset, displacement, limit };
    }
  };

  struct Wang2 {
    Border borders[2];
    Edge edge;

    bool is_overlay() const {
      return borders[1].id.hash == Void;
    }
  };

  struct Wang3 {
    AtomId ids[3];

    bool is_overlay() const {
      return ids[2].hash == Void;
    }
  };

  constexpr int AtomsTilesetSize = 4;
  constexpr int Wang2TilesetSize = 4;
  constexpr int Wang3TilesetSize = 6;

  enum class Search : uint8_t {
    UseDatabaseOnly,
    IncludeTemporary
  };


  struct TilesetData {
    Settings settings;
    std::vector<Atom> atoms;
    std::vector<Wang2> wang2;
    std::vector<Wang3> wang3;

    struct {
      Atom atom;
      Wang2 wang2;
    } temporary;

    Atom get_atom(gf::Id hash, Search search = Search::UseDatabaseOnly) const;
    Wang2 get_wang2(gf::Id id0, gf::Id id1, Search search = Search::UseDatabaseOnly) const;
    Edge get_edge(gf::Id id0, gf::Id id1, Search search = Search::UseDatabaseOnly) const;

    void update_atom(const Atom& old_atom, const Atom& new_atom);
    void delete_atom(gf::Id id);

    void generate_all_wang3();

    static TilesetData load(const std::filesystem::path& filename);
    static void save(const std::filesystem::path& filename, const TilesetData& data);
  };

}

#endif // AKGR_TILESET_DATA_H
