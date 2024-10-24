#include "TilesetData.h"

#include <cstdint>

#include <fstream>

#include <nlohmann/json.hpp>

#include <gf2/core/Log.h>

using JSON = nlohmann::basic_json<nlohmann::ordered_map, std::vector, std::string, bool, int32_t, uint32_t, float>;

NLOHMANN_JSON_NAMESPACE_BEGIN

template<>
struct adl_serializer<gf::Color> {
  static void to_json(JSON& j, const gf::Color& color) {
    j = JSON{ color.r, color.g, color.b, color.a };
  }

  static void from_json(const JSON& j, gf::Color& color) {
    j.at(0).get_to(color.r);
    j.at(1).get_to(color.g);
    j.at(2).get_to(color.b);
    j.at(3).get_to(color.a);

    if (color.r > 1.0f || color.g > 1.0f || color.b > 1.0f || color.a > 1.0f) {
      color.r /= 255.0f;
      color.g /= 255.0f;
      color.b /= 255.0f;
      color.a /= 255.0f;
    }
  }
};

NLOHMANN_JSON_NAMESPACE_END

namespace akgr {

  ImageFeatures Settings::image_features() const {
    auto size = tile.extended_size();
    int step = size * 12;

    for (int width = step; width <= 8192; width += step) {
      int height = 0;

      int atoms_per_line = width / (AtomsTilesetSize * size);

      if (atoms_per_line == 0) {
        continue;
      }

      int atoms_line_count = (max_atom_count / atoms_per_line) + ((max_atom_count % atoms_per_line == 0) ? 0 : 1);
      height += atoms_line_count * (AtomsTilesetSize * size);

      if (height > width) {
        continue;
      }

      int wang2_per_line = width / (Wang2TilesetSize * size);

      if (wang2_per_line == 0) {
        continue;
      }

      int wang2_line_count = (max_wang2_count / wang2_per_line) + ((max_wang2_count % wang2_per_line == 0) ? 0 : 1);
      height += wang2_line_count * (Wang2TilesetSize * size);

      if (height > width) {
        continue;
      }

      int wang3_per_line = width / (Wang3TilesetSize * size);

      if (wang3_per_line == 0) {
        continue;
      }

      int wang3_line_count = (max_wang3_count / wang3_per_line) + ((max_wang3_count % wang3_per_line == 0) ? 0 : 1);
      height += wang3_line_count * (Wang3TilesetSize * size);

      if (height > width) {
        continue;
      }

      gf::Log::debug("atoms: {} x {} ({})", atoms_per_line, atoms_line_count, atoms_per_line * atoms_line_count);
      gf::Log::debug("wang2: {} x {} ({})", wang2_per_line, wang2_line_count, wang2_per_line * wang2_line_count);
      gf::Log::debug("wang3: {} x {} ({})", wang3_per_line, wang3_line_count, wang3_per_line * wang3_line_count);

      ImageFeatures features = {};
      features.size = gf::vec(width, height);
      features.atoms_per_line = atoms_per_line;
      features.atoms_line_count = atoms_line_count;
      features.wang2_per_line = wang2_per_line;
      features.wang2_line_count = wang2_line_count;
      features.wang3_per_line = wang3_per_line;
      features.wang3_line_count = wang3_line_count;

      return features;
    }

    return ImageFeatures{};
  }

  gf::Vec2I Settings::image_size() const {
    return image_features().size;
  }

  Atom TilesetData::get_atom(gf::Id hash, Search search) const {
    if (search == Search::IncludeTemporary) {
      if (temporary.atom.id.hash == hash) {
        return temporary.atom;
      }
    }

    for (const auto& atom : atoms) {
      if (atom.id.hash == hash) {
        return atom;
      }
    }

    Atom void_atom;
    void_atom.id.hash = Void;
    void_atom.id.name = "-";
    void_atom.color = gf::Transparent;
    void_atom.pigment.style = PigmentStyle::Plain;

    if (hash != Void) {
      gf::Log::warning("Unknown atom hash: {:x}", static_cast<uint64_t>(hash));
    }

    return void_atom;
  }

  Wang2 TilesetData::get_wang2(gf::Id id0, gf::Id id1, Search search) const {
    if (search == Search::IncludeTemporary) {
      if (temporary.wang2.borders[0].id.hash == id0 && temporary.wang2.borders[1].id.hash == id1) {
        return temporary.wang2;
      }

      if (temporary.wang2.borders[0].id.hash == id1 && temporary.wang2.borders[1].id.hash == id0) {
        return temporary.wang2;
      }
    }

    for (const auto& wang : wang2) {
      if (wang.borders[0].id.hash == id0 && wang.borders[1].id.hash == id1) {
        return wang;
      }

      if (wang.borders[0].id.hash == id1 && wang.borders[1].id.hash == id0) {
        return wang;
      }
    }

    Atom a0 = get_atom(id0);
    Atom a1 = get_atom(id1);

    gf::Log::warning("No wang2 for this pair of atoms: ({}, {})", a0.id.name, a1.id.name);

    Wang2 wang;
    wang.borders[0].id = a0.id;
    wang.borders[0].effect = BorderEffect::None;
    wang.borders[1].id = a1.id;
    wang.borders[1].effect = BorderEffect::None;

    if (id0 == Void) {
      std::swap(wang.borders[0].id, wang.borders[1].id);
    }

    return wang;
  }

  Edge TilesetData::get_edge(gf::Id id0, gf::Id id1, Search search) const {
    if (search == Search::IncludeTemporary) {
      if (temporary.wang2.borders[0].id.hash == id0 && temporary.wang2.borders[1].id.hash == id1) {
        return temporary.wang2.edge;
      }

      if (temporary.wang2.borders[0].id.hash == id1 && temporary.wang2.borders[1].id.hash == id0) {
        return temporary.wang2.edge.invert();
      }
    }

    for (const auto& wang : wang2) {
      if (wang.borders[0].id.hash == id0 && wang.borders[1].id.hash == id1) {
        return wang.edge;
      }

      if (wang.borders[0].id.hash == id1 && wang.borders[1].id.hash == id0) {
        return wang.edge.invert();
      }
    }

    Edge edge;
    return edge;
  }

  void TilesetData::update_atom(const Atom& old_atom, const Atom& new_atom) {
    for (auto& atom : atoms) {
      if (atom.id.hash == old_atom.id.hash) {
        atom = new_atom;
      }
    }

    for (auto& wang : wang2) {
      for (auto& border : wang.borders) {
        if (border.id.hash == old_atom.id.hash) {
          border.id = new_atom.id;
        }
      }
    }

    for (auto& wang : wang3) {
      for (auto& id : wang.ids) {
        if (id.hash == old_atom.id.hash) {
          id = new_atom.id;
        }
      }
    }
  }

  void TilesetData::delete_atom(gf::Id id) {
    atoms.erase(std::remove_if(atoms.begin(), atoms.end(), [id](auto& atom) {
      return atom.id.hash == id;
    }), atoms.end());

    wang2.erase(std::remove_if(wang2.begin(), wang2.end(), [id](auto& wang) {
      return wang.borders[0].id.hash == id || wang.borders[1].id.hash == id;
    }), wang2.end());

    wang3.erase(std::remove_if(wang3.begin(), wang3.end(), [id](auto& wang) {
      return wang.ids[0].hash == id || wang.ids[1].hash == id || wang.ids[2].hash == id;
    }), wang3.end());
  }

  void TilesetData::generate_all_wang3() {
    wang3.clear();
    std::size_t count = wang2.size();

    for (std::size_t i = 0; i < count; ++i) {
      auto & w0 = wang2[i];

      for (std::size_t j = i + 1; j < count; ++j) {
        auto & w1 = wang2[j];

        for (std::size_t k = j + 1; k < count; ++k) {
          auto & w2 = wang2[k];

          std::array<AtomId, 6> ids = {
            w0.borders[0].id, w0.borders[1].id,
            w1.borders[0].id, w1.borders[1].id,
            w2.borders[0].id, w2.borders[1].id,
          };

          std::sort(ids.begin(), ids.end(), [](const AtomId & lhs, const AtomId & rhs) { return lhs.hash < rhs.hash; });

          if (ids[0].hash == ids[1].hash && ids[2].hash == ids[3].hash && ids[4].hash == ids[5].hash) {
            Wang3 wang;
            wang.ids[0] = ids[0];
            wang.ids[1] = ids[2];
            wang.ids[2] = ids[4];

            if (wang.ids[0].hash == Void) {
              std::swap(wang.ids[0], wang.ids[2]);
            }

            if (wang.ids[1].hash == Void) {
              std::swap(wang.ids[1], wang.ids[2]);
            }

            wang3.push_back(wang);
          }
        }
      }
    }
  }

  /*
   * parsing and saving in JSON
   */

  NLOHMANN_JSON_SERIALIZE_ENUM( PigmentStyle, {
    { PigmentStyle::Plain, "plain" },
    { PigmentStyle::Randomize, "randomize"},
    { PigmentStyle::Striped, "striped"},
    { PigmentStyle::Paved, "paved"},
  })

  NLOHMANN_JSON_SERIALIZE_ENUM( BorderEffect, {
    { BorderEffect::None, "none" },
    { BorderEffect::Fade, "fade" },
    { BorderEffect::Outline, "outline" },
    { BorderEffect::Sharpen, "sharpen" },
    { BorderEffect::Lighten, "lighten" },
    { BorderEffect::Blur, "blur" },
    { BorderEffect::Blend, "blend" },
  })

  void to_json(JSON& j, const Settings& settings) {
    JSON tile = JSON{
      { "size", settings.tile.size },
      { "spacing", settings.tile.spacing }
    };

    j = JSON{
      { "locked", settings.locked },
      { "max_atom_count", settings.max_atom_count },
      { "max_wang2_count", settings.max_wang2_count },
      { "max_wang3_count", settings.max_wang3_count },
      { "tile", tile }
    };
  }

  void from_json(const JSON& j, Settings& settings) {
    j.at("locked").get_to(settings.locked);
    j.at("max_atom_count").get_to(settings.max_atom_count);
    j.at("max_wang2_count").get_to(settings.max_wang2_count);
    j.at("max_wang3_count").get_to(settings.max_wang3_count);
    j.at("tile").at("size").get_to(settings.tile.size);
    j.at("tile").at("spacing").get_to(settings.tile.spacing);
  }

  void to_json(JSON& j, const Pigment& pigment) {
    switch (pigment.style) {
      case PigmentStyle::Plain:
        j = JSON{
          { "style", pigment.style }
        };
        break;
      case PigmentStyle::Randomize:
        j = JSON{
          { "style", pigment.style },
          { "ratio", pigment.randomize.ratio },
          { "deviation", pigment.randomize.deviation },
          { "size", pigment.randomize.size },
        };
        break;
      case PigmentStyle::Striped:
        j = JSON{
          { "style", pigment.style },
          { "width", pigment.striped.width },
          { "stride", pigment.striped.stride }
        };
        break;
      case PigmentStyle::Paved:
        j = JSON{
          { "style", pigment.style },
          { "width", pigment.paved.width },
          { "length", pigment.paved.length },
          { "modulation", pigment.paved.modulation }
        };
    }
  }

  void from_json(const JSON& j, Pigment& pigment) {
    j.at("style").get_to(pigment.style);

    switch (pigment.style) {
      case PigmentStyle::Plain:
        break;
      case PigmentStyle::Randomize:
        j.at("ratio").get_to(pigment.randomize.ratio);
        j.at("deviation").get_to(pigment.randomize.deviation);
        j.at("size").get_to(pigment.randomize.size);
        break;
      case PigmentStyle::Striped:
        j.at("width").get_to(pigment.striped.width);
        j.at("stride").get_to(pigment.striped.stride);
        break;
      case PigmentStyle::Paved:
        j.at("width").get_to(pigment.paved.width);
        j.at("length").get_to(pigment.paved.length);
        j.at("modulation").get_to(pigment.paved.modulation);
        break;
    }
  }

  void to_json(JSON& j, const AtomId& id) {
    j = JSON(id.name);
  }

  void from_json(const JSON& j, AtomId& id) {
    j.get_to(id.name);
    id.hash = gf::hash_string(id.name);
  }

  void to_json(JSON& j, const Atom& atom) {
    j = JSON{
      { "id", atom.id },
      { "color", atom.color },
      { "pigment", atom.pigment }
    };
  }

  void from_json(const JSON& j, Atom& atom) {
    j.at("id").get_to(atom.id);
    j.at("color").get_to(atom.color);
    j.at("pigment").get_to(atom.pigment);
  }

  void to_json(JSON& j, const Border& border) {
    switch (border.effect) {
      case BorderEffect::None:
        j = JSON{
          { "id", border.id },
          { "effect", border.effect }
        };
        break;
      case BorderEffect::Fade:
        j = JSON{
          { "id", border.id },
          { "effect", border.effect },
          { "distance", border.fade.distance }
        };
        break;
      case BorderEffect::Outline:
        j = JSON{
          { "id", border.id },
          { "effect", border.effect },
          { "distance", border.outline.distance },
          { "factor", border.outline.factor }
        };
        break;
      case BorderEffect::Sharpen:
        j = JSON{
          { "id", border.id },
          { "effect", border.effect },
          { "distance", border.sharpen.distance }
        };
        break;
      case BorderEffect::Lighten:
        j = JSON{
          { "id", border.id },
          { "effect", border.effect },
          { "distance", border.lighten.distance }
        };
        break;
      case BorderEffect::Blur:
        j = JSON{
          { "id", border.id },
          { "effect", border.effect }
        };
        break;
      case BorderEffect::Blend:
        j = JSON{
          { "id", border.id },
          { "effect", border.effect },
          { "distance", border.blend.distance }
        };
        break;
    }
  }

  void from_json(const JSON& j, Border& border) {
    j.at("id").get_to(border.id);
    j.at("effect").get_to(border.effect);

    switch (border.effect) {
      case BorderEffect::None:
        break;
      case BorderEffect::Fade:
        j.at("distance").get_to(border.fade.distance);
        break;
      case BorderEffect::Outline:
        j.at("distance").get_to(border.outline.distance);
        j.at("factor").get_to(border.outline.factor);
        break;
      case BorderEffect::Sharpen:
        j.at("distance").get_to(border.sharpen.distance);
        break;
      case BorderEffect::Lighten:
        j.at("distance").get_to(border.lighten.distance);
        break;
      case BorderEffect::Blur:
        break;
      case BorderEffect::Blend:
        j.at("distance").get_to(border.blend.distance);
        break;
    }
  }

  void to_json(JSON& j, const Displacement& displacement) {
    j = JSON{
      { "iterations", displacement.iterations },
      { "initialFactor", displacement.initial },
      { "reductionFactor", displacement.reduction }
    };
  }

  void from_json(const JSON& j, Displacement& displacement) {
    j.at("iterations").get_to(displacement.iterations);
    j.at("initialFactor").get_to(displacement.initial);
    j.at("reductionFactor").get_to(displacement.reduction);
  }

  void to_json(JSON& j, const Wang2& wang) {
    j = JSON{
      { "borders", wang.borders },
      { "offset", wang.edge.offset },
      { "displacement", wang.edge.displacement },
      { "limit", wang.edge.limit }
    };
  }

  void from_json(const JSON& j, Wang2& wang) {
    j.at("borders").get_to(wang.borders);
    j.at("offset").get_to(wang.edge.offset);
    j.at("displacement").get_to(wang.edge.displacement);
    j.at("limit").get_to(wang.edge.limit);
  }

  void to_json(JSON& j, const Wang3& wang) {
    j = JSON(wang.ids);
  }

  void from_json(const JSON& j, Wang3& wang) {
    j.get_to(wang.ids);
  }

  void to_json(JSON& j, const TilesetData& data) {
    j = JSON{
      { "settings", data.settings },
      { "atoms", data.atoms },
      { "wang2", data.wang2 },
      { "wang3", data.wang3 }
    };
  }

  void from_json(const JSON& j, TilesetData& data) {
    j.at("settings").get_to(data.settings);
    j.at("atoms").get_to(data.atoms);
    j.at("wang2").get_to(data.wang2);
    j.at("wang3").get_to(data.wang3);
  }

  TilesetData TilesetData::load(const std::filesystem::path& filename) {
    TilesetData data;

    try {
      std::ifstream ifs(filename);
      auto j = JSON::parse(ifs, nullptr, true, true);
      data = j.get<TilesetData>();
    } catch (std::exception& ex) {
      gf::Log::error("An error occurred while parsing file '{}': {}", filename.string(), ex.what());
    }

    return data;
  }

  void TilesetData::save(const std::filesystem::path& filename, const TilesetData& data) {
    try {
      JSON j = data;
      std::ofstream ofs(filename);
      ofs << j.dump(1, '\t') << '\n';
      gf::Log::info("Project successfully saved in '{}'", filename.string());
    } catch (std::exception& ex) {
      gf::Log::error("An error occurred while saving file '{}': {}", filename.string(), ex.what());
    }
  }


}
