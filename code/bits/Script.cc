#include "Script.h"

#include <cassert>
#include <fstream>

#include <gf2/core/Id.h>

#include "Akagoria.h"
#include "DataLexicon.h"

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
      using namespace gf::literals;

      switch (gf::hash_string(signature)) {
        case "move_hero(_)"_id:
          return &Script::move_hero;
        case "move_hero_down()"_id:
          return &Script::move_hero_down;
        case "move_hero_up()"_id:
          return &Script::move_hero_up;
        case "post_notification(_)"_id:
          return &Script::post_notification;
        case "add_requirement(_)"_id:
          return &Script::add_requirement;
        case "remove_requirement(_)"_id:
          return &Script::remove_requirement;
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
      if (std::strcmp(text, "\n") == 0) {
        return;
      }

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
    if (m_vm != nullptr) {
      return;
    }

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

  void Script::initialize()
  {
    agateStackStart(m_vm);
    ptrdiff_t arg0 = agateSlotAllocate(m_vm);
    agateSlotSetHandle(m_vm, arg0, m_class_adventure);
    AgateStatus result = agateCallHandle(m_vm, m_method_initialize);
    agateStackFinish(m_vm);

    if (result != AGATE_STATUS_OK) {
      gf::Log::error("Could not execute 'Adventure.initialize()'");
    }
  }

  void Script::start()
  {
    agateStackStart(m_vm);
    ptrdiff_t arg0 = agateSlotAllocate(m_vm);
    agateSlotSetHandle(m_vm, arg0, m_class_adventure);
    AgateStatus result = agateCallHandle(m_vm, m_method_start);
    agateStackFinish(m_vm);

    if (result != AGATE_STATUS_OK) {
      gf::Log::error("Could not execute 'Adventure.start()'\n");
    }
  }

  void Script::on_message(const std::string& name)
  {
    agateStackStart(m_vm);
    ptrdiff_t arg0 = agateSlotAllocate(m_vm);
    agateSlotSetHandle(m_vm, arg0, m_class_adventure);
    ptrdiff_t arg1 = agateSlotAllocate(m_vm);
    agateSlotSetString(m_vm, arg1, name.c_str());
    AgateStatus result = agateCallHandle(m_vm, m_method_on_message);
    agateStackFinish(m_vm);

    if (result != AGATE_STATUS_OK) {
      gf::Log::error("Could not execute 'Adventure.on_message(_)'");
    }
  }

  void Script::on_dialog(const std::string& name)
  {
    agateStackStart(m_vm);
    ptrdiff_t arg0 = agateSlotAllocate(m_vm);
    agateSlotSetHandle(m_vm, arg0, m_class_adventure);
    ptrdiff_t arg1 = agateSlotAllocate(m_vm);
    agateSlotSetString(m_vm, arg1, name.c_str());
    AgateStatus result = agateCallHandle(m_vm, m_method_on_dialog);
    agateStackFinish(m_vm);

    if (result != AGATE_STATUS_OK) {
      gf::Log::error("Could not execute 'Adventure.on_dialog(_)'");
    }
  }

  void Script::on_message_deferred(std::string name)
  {
    m_messages.push(std::move(name));
  }

  void Script::handle_deferred_messages()
  {
    while (!m_messages.empty()) {
      on_message(m_messages.front());
      m_messages.pop();
    }
  }

  void Script::not_implemented([[maybe_unused]] AgateVM* vm) {
    // assert(false);
  }

  // move_hero(location)
  void Script::move_hero(AgateVM* vm)
  {
    const char* location_id = agateSlotGetString(vm, 1);

    gf::Log::info("[SCRIPT] move_hero({})", location_id);

    const LocationRuntime* location = data_lexicon_find(runtime(vm).locations, gf::hash_string(location_id));
    assert(location);

    state(vm).hero.spot = location->spot;
    runtime(vm).physics.hero.set_spot(location->spot);

    agateSlotSetNil(vm, AGATE_RETURN_SLOT);
  }

  // move_hero_down()
  void Script::move_hero_down(AgateVM* vm)
  {
    gf::Log::info("[SCRIPT] move_hero_down()");

    auto& hero = state(vm).hero;
    hero.spot.floor -= 2;
    runtime(vm).physics.hero.set_spot(hero.spot);

    agateSlotSetNil(vm, AGATE_RETURN_SLOT);
  }

  // move_hero_up()
  void Script::move_hero_up(AgateVM* vm)
  {
    gf::Log::info("[SCRIPT] move_hero_up()");

    auto& hero = state(vm).hero;
    hero.spot.floor += 2;
    runtime(vm).physics.hero.set_spot(hero.spot);

    agateSlotSetNil(vm, AGATE_RETURN_SLOT);
  }

  // post_notification(notification)
  void Script::post_notification(AgateVM* vm)
  {
    const char* notification_id = agateSlotGetString(vm, 1);

    gf::Log::info("[SCRIPT] post_notification({})", notification_id);

    NotificationState notification;
    notification.data.id = gf::hash_string(notification_id);
    notification.data.bind_from(data(vm).notifications);
    assert(notification.data.origin != nullptr);

    state(vm).notifications.push_back(notification);

    agateSlotSetNil(vm, AGATE_RETURN_SLOT);
  }

  // add_requirement(requirement)
  void Script::add_requirement(AgateVM* vm)
  {
    const char* requirement_id = agateSlotGetString(vm, 1);

    gf::Log::info("[SCRIPT] add_requirement({})", requirement_id);

    state(vm).hero.requirements.insert(gf::hash_string(requirement_id));
    agateSlotSetNil(vm, AGATE_RETURN_SLOT);
  }

  // remove_requirement(requirement)
  void Script::remove_requirement(AgateVM* vm)
  {
    const char* requirement_id = agateSlotGetString(vm, 1);

    gf::Log::info("[SCRIPT] remove_requirement({})", requirement_id);

    state(vm).hero.requirements.erase(gf::hash_string(requirement_id));
    agateSlotSetNil(vm, AGATE_RETURN_SLOT);
  }

  const WorldData& Script::data(AgateVM* vm)
  {
    auto* script = static_cast<Script *>(agateGetUserData(vm));
    return script->m_game->world_model()->data;
  }

  WorldState& Script::state(AgateVM* vm)
  {
    auto* script = static_cast<Script *>(agateGetUserData(vm));
    return script->m_game->world_model()->state;
  }

  WorldRuntime& Script::runtime(AgateVM* vm)
  {
    auto* script = static_cast<Script *>(agateGetUserData(vm));
    return script->m_game->world_model()->runtime;
  }

}
