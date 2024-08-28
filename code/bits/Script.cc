#include "Script.h"

#include <cassert>
#include <fstream>

#include <gf2/core/Id.h>

#include "Akagoria.h"

namespace akgr {

  namespace {
    constexpr const char* Unit = "Adventure";

    std::string load_file(const std::filesystem::path& filename)
    {
       std::ifstream file(filename);

      if (!file) {
        gf::Log::fatal("File for script does not exist: '{}'", filename.string());
      }

      std::string content;

      for (std::string line; std::getline(file, line); ) {
        content.append(line);
        content.append(1, '\n');
      }

      return content;
    }

    const char *vm_unit_load(const char* name, void* user_data) {
      auto* script = static_cast<Script *>(user_data);
      return script->load_module(name);
    }

    AgateUnitHandler vm_unit_handler(AgateVM* vm, [[maybe_unused]] const char* name) {
      AgateUnitHandler handler;
      handler.load = vm_unit_load;
      handler.release = nullptr;
      handler.user_data = agateGetUserData(vm);
      return handler;
    }

    AgateForeignMethodFunc vm_foreign_method_handler([[maybe_unused]] AgateVM* vm, [[maybe_unused]] const char* unit_name, [[maybe_unused]] const char* class_name, [[maybe_unused]] AgateForeignMethodKind kind, const char* signature) {
      assert(std::strcmp(unit_name, "world") == 0);
      assert(std::strcmp(class_name, "World") == 0);
      assert(kind == AGATE_FOREIGN_METHOD_CLASS);

      switch (gf::hash_string(signature)) {
        // case "move_hero(_)"_id:
        //   return &Script::moveHero;
        // case "move_hero_down()"_id:
        //   return &Script::moveHeroDown;
        // case "move_hero_up()"_id:
        //   return &Script::moveHeroUp;
        // case "post_notification(_)"_id:
        //   return &Script::postNotification;
        // case "add_requirement(_)"_id:
        //   return &Script::addRequirement;
        // case "remove_requirement(_)"_id:
        //   return &Script::removeRequirement;
        // case "add_item(_,_)"_id:
        //   return &Script::addItem;
        // case "add_item_to_inventory(_)"_id:
        //   return &Script::addItemToInventory;
        // case "add_character(_,_)"_id:
        //   return &Script::addCharacter;
        // case "set_character_mood(_,_)"_id:
        //   return &Script::setCharacterMood;
        // case "start_dialog(_)"_id:
        //   return &Script::startDialog;
        // case "attach_dialog_to_character(_,_)"_id:
        //   return &Script::attachDialogToCharacter;
        default:
          break;
      }

      return Script::not_implemented;
    }

    void vm_print([[maybe_unused]] AgateVM* vm, const char* text) {
      gf::Log::info("{}", text);
    }

    const char* vm_error_to_string(AgateErrorKind kind) {
      switch (kind) {
        case AGATE_ERROR_COMPILE:
          return "Compile error";
        case AGATE_ERROR_RUNTIME:
          return "Runtime error";
        case AGATE_ERROR_STACKTRACE:
          return "Stack";
      }

      assert(false);
      return "???";
    }

    void vm_error([[maybe_unused]] AgateVM* vm, AgateErrorKind kind, const char* unit_name, int line, const char* message) {
      gf::Log::error("{}:{} [{}] {}", unit_name == nullptr ? "<main>" : unit_name, line, vm_error_to_string(kind), message);
    }

  }

  Script::Script(Akagoria* game)
  : m_game(game)
  {
  }

  Script::~Script()
  {
    for (auto* handle : { m_method_on_dialog, m_method_on_message, m_method_start, m_method_initialize, m_class_adventure }) {
      if (handle != nullptr) {
        agateReleaseHandle(m_vm, handle);
      }
    }

    if (m_vm != nullptr) {
      agateDeleteVM(m_vm);
    }
  }

  void Script::bind()
  {
    const char* script = load_module("adventure");

    AgateConfig configuration;
    agateConfigInitialize(&configuration);

    configuration.unit_handler = vm_unit_handler;
    configuration.foreign_method_handler = vm_foreign_method_handler;

    configuration.print = vm_print;
    configuration.error = vm_error;

    configuration.user_data = this;

    m_vm = agateNewVM(&configuration);

    AgateStatus status = agateCallString(m_vm, Unit, script);

    if (status != AGATE_STATUS_OK) {
      gf::Log::fatal("Could not load the main script.");
    }

    agateStackStart(m_vm);
    ptrdiff_t adventure_slot = agateSlotAllocate(m_vm);
    agateGetVariable(m_vm, Unit, "Adventure", adventure_slot);
    m_class_adventure = agateSlotGetHandle(m_vm, adventure_slot);
    agateStackFinish(m_vm);
    assert(m_class_adventure);

    m_method_initialize = agateMakeCallHandle(m_vm, "initialize()");
    assert(m_method_initialize);
    m_method_start = agateMakeCallHandle(m_vm, "start()");
    assert(m_method_start);
    m_method_on_message = agateMakeCallHandle(m_vm, "on_message(_)");
    assert(m_method_on_message);
    m_method_on_dialog = agateMakeCallHandle(m_vm, "on_dialog(_)");
    assert(m_method_on_dialog);
  }

  const char* Script::load_module(std::filesystem::path name)
  {
    auto path = m_game->resource_manager()->search("scripts" / name.replace_extension(".agate"));
    auto script = load_file(path);
    m_sources.push_back(std::move(script));
    return m_sources.back().c_str();
  }


  void Script::not_implemented([[maybe_unused]] AgateVM* vm) {
    assert(false);
  }

}
