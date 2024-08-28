#ifndef AKGR_SCRIPT_H
#define AKGR_SCRIPT_H

#include <filesystem>
#include <string>
#include <vector>

#include <agate.h>

namespace akgr {
  class Akagoria;

  class Script {
  public:
    Script(Akagoria* game);
    Script(const Script&) = delete;
    Script& operator=(const Script&) = delete;
    ~Script();

    Script(Script&&) = delete;
    Script& operator=(Script&&) = delete;

    void bind();

    const char* load_module(std::filesystem::path name);

    /*
     * interface
     */

    static void not_implemented(AgateVM* vm);

  private:
    Akagoria* m_game;

    std::vector<std::string> m_sources;

    AgateVM* m_vm = nullptr;
    AgateHandle* m_class_adventure = nullptr;
    AgateHandle* m_method_initialize = nullptr;
    AgateHandle* m_method_start = nullptr;
    AgateHandle* m_method_on_message = nullptr;
    AgateHandle* m_method_on_dialog = nullptr;
  };


}

#endif // AKGR_SCRIPT_H
