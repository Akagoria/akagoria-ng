#ifndef AKGR_SCRIPT_H
#define AKGR_SCRIPT_H

#include <filesystem>
#include <queue>
#include <string>
#include <vector>

#include <agate.h>

namespace akgr {
  class Akagoria;
  struct WorldData;
  struct WorldState;
  struct WorldRuntime;

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
     * World interface
     */

    void initialize();
    void start();
    void on_message(const std::string& name);
    void on_dialog(const std::string& name);
    void on_quest(const std::string& name);

    void on_message_deferred(std::string name);
    void handle_deferred_messages();

    /*
     * Adventure interface
     */

    static void not_implemented(AgateVM* vm);

    static void move_hero(AgateVM* vm);
    static void move_hero_down(AgateVM* vm);
    static void move_hero_up(AgateVM* vm);

    static void post_notification(AgateVM* vm);

    static void add_requirement(AgateVM* vm);
    static void remove_requirement(AgateVM* vm);

    static void start_dialog(AgateVM* vm);

    static void add_character(AgateVM* vm);
    static void attach_dialog_to_character(AgateVM* vm);

  private:
    static Akagoria& game(AgateVM* vm);
    static const WorldData& data(AgateVM* vm);
    static WorldState& state(AgateVM* vm);
    static WorldRuntime& runtime(AgateVM* vm);

    Akagoria* m_game;

    std::vector<std::string> m_sources;

    AgateVM* m_vm = nullptr;
    AgateHandle* m_class_adventure = nullptr;
    AgateHandle* m_method_initialize = nullptr;
    AgateHandle* m_method_start = nullptr;
    AgateHandle* m_method_on_message = nullptr;
    AgateHandle* m_method_on_dialog = nullptr;
    AgateHandle* m_method_on_quest = nullptr;

    std::queue<std::string> m_messages;
  };


}

#endif // AKGR_SCRIPT_H
