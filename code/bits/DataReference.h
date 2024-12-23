#ifndef AKGR_DATA_REFERENCE_H
#define AKGR_DATA_REFERENCE_H

#include <cassert>

#include <gf2/core/Id.h>
#include <gf2/core/Log.h>
#include <gf2/core/TypeTraits.h>

#include "DataLexicon.h"

namespace akgr {

  template<typename T>
  struct DataReference {
    std::string tag;
    gf::Id id = gf::InvalidId;
    const T* origin = nullptr;

    void bind_from(const DataLexicon<T>& lexicon)
    {
      origin = data_lexicon_find(lexicon, id);

      if (origin == nullptr) {
        gf::Log::error("Could not bind reference '{}' ({:x})", tag, static_cast<uint64_t>(id));
      }
    }

    explicit operator bool() const noexcept
    {
      return id != gf::InvalidId;
    }

    bool check() const noexcept
    {
      return origin != nullptr;
    }

    const T* operator->() const
    {
      assert(origin != nullptr);
      return origin;
    }

    const T& operator()() const
    {
      assert(origin != nullptr);
      return *origin;
    }

    void reset()
    {
      tag.clear();
      id = gf::InvalidId;
      origin = nullptr;
    }
  };

  namespace details {

    template<typename Archive, typename Self>
    Archive& handle_data_reference_serialization(Archive& ar, Self& self)
    {
      return ar | self.tag | self.id;
    }

  }

  template<typename Archive, typename T>
  Archive& operator|(Archive& ar, DataReference<T>& reference) {
    return details::handle_data_reference_serialization(ar, reference);
  }

  template<typename Archive, typename T>
  Archive& operator|(Archive& ar, const DataReference<T>& reference) {
    return details::handle_data_reference_serialization(ar, reference);
  }

}

#endif // AKGR_DATA_REFERENCE_H
