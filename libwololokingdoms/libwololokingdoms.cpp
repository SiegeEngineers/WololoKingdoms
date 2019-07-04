#include "wksettings.h"
#include "wkconverter.h"
#include <memory>
#include <filesystem>

using path_char_t = std::filesystem::path::value_type;

class Listener;

struct wkconvert_handle {
public:
  std::unique_ptr<WKConverter> converter;
  std::unique_ptr<Listener> listener;

  wkconvert_handle(WKConverter* converter, Listener* listener): converter(converter), listener(listener) {}
};

typedef WKSettings* wksettings_t;
typedef wkconvert_handle* wkconverter_t;

extern "C" {
  wksettings_t wksettings_create();
  void wksettings_free(wksettings_t);

  // Configure settings.
  void wksettings_copy_maps(wksettings_t, char enable);
  void wksettings_copy_custom_maps(wksettings_t, char enable);
  void wksettings_restricted_civ_mods(wksettings_t, char enable);
  void wksettings_fix_flags(wksettings_t, char enable);
  void wksettings_replace_tooltips(wksettings_t, char enable);
  void wksettings_use_grid(wksettings_t, char enable);
  void wksettings_use_short_walls(wksettings_t, char enable);
  void wksettings_language(wksettings_t, const char* code);
  void wksettings_patch(wksettings_t, int patch);
  void wksettings_hotkeys(wksettings_t, int choice);
  void wksettings_dlc_level(wksettings_t, int level);
  void wksettings_resource_path(wksettings_t, const path_char_t* path);
  void wksettings_hd_path(wksettings_t, const path_char_t* path);
  void wksettings_output_path(wksettings_t, const path_char_t* path);
  void wksettings_voobly_path(wksettings_t, const path_char_t* path);
  void wksettings_up_path(wksettings_t, const path_char_t* path);
  void wksettings_mod_name(wksettings_t, const path_char_t* name);

  // Add mods.
  void wksettings_data_mod(wksettings_t, const char* name, const char* exe, const char* version, int flags, const char* exe2);
  void wksettings_drs_resources(wksettings_t, const path_char_t* dir, WKSettings::IndexType type);

  // Converter setup.
  wkconverter_t wkconverter_create(wksettings_t settings, void* context);
  void wkconverter_free(wkconverter_t);

  // Callbacks.
  void wkconverter_on_finished(wkconverter_t inst, void (*onFinished)(void*));
  void wkconverter_on_log(wkconverter_t inst, void (*onLog)(void*, const char*));
  void wkconverter_on_set_info(wkconverter_t inst, void (*onSetInfo)(void*, const char*));
  void wkconverter_on_error(wkconverter_t inst, void (*onError)(void*, const char*));
  void wkconverter_on_progress(wkconverter_t inst, void (*onProgress)(void*, int));

  // Run the converter.
  int wkconverter_run(wkconverter_t);

  // Convenience wrapper.
  int wkconvert(wksettings_t settings, void* context);
}

class Listener: public WKConvertListener {
  void* context;
public:
  Listener(void* context): context(context) {}

  void (*onFinished)(void*) = nullptr;
  void (*onLog)(void*, const char*) = nullptr;
  void (*onSetInfo)(void*, const char*) = nullptr;
  void (*onError)(void*, const char*) = nullptr;
  void (*onProgress)(void*, int) = nullptr;

  virtual ~Listener() {
  }

  virtual void finished() {
    if (onFinished != nullptr) onFinished(context);
  }

  virtual void log(std::string logMessage) {
    if (onLog != nullptr) onLog(context, logMessage.c_str());
  }

  virtual void setInfo(std::string info) {
    if (onSetInfo != nullptr) onSetInfo(context, info.c_str());
  }

  virtual void error(std::exception const& err) {
    if (onError != nullptr) onError(context, err.what());
  }

  virtual void setProgress(int i) {
    if (onProgress != nullptr) onProgress(context, i);
  }

  // Will leave this to the implementer after the fact I think
  // virtual void installUserPatch();
};

extern "C" wksettings_t wksettings_create() {
  return new WKSettings();
}

extern "C" void wksettings_free(wksettings_t inst) {
  if (inst != nullptr) delete inst;
}

extern "C" void wksettings_copy_maps(wksettings_t inst, char enable) {
  inst->copyMaps = enable != 0;
}

extern "C" void wksettings_copy_custom_maps(wksettings_t inst, char enable) {
  inst->copyCustomMaps = enable != 0;
}

extern "C" void wksettings_restricted_civ_mods(wksettings_t inst, char enable) {
  inst->restrictedCivMods = enable != 0;
}

extern "C" void wksettings_fix_flags(wksettings_t inst, char enable) {
  inst->fixFlags = enable != 0;
}

extern "C" void wksettings_replace_tooltips(wksettings_t inst, char enable) {
  inst->replaceTooltips = enable != 0;
}

extern "C" void wksettings_use_grid(wksettings_t inst, char enable) {
  inst->useGrid = enable != 0;
}

extern "C" void wksettings_use_short_walls(wksettings_t inst, char enable) {
  inst->useShortWalls = enable != 0;
}

extern "C" void wksettings_language(wksettings_t inst, const char* code) {
  inst->language = code;
}

extern "C" void wksettings_patch(wksettings_t inst, int patch) {
  inst->patch = patch;
}

extern "C" void wksettings_hotkeys(wksettings_t inst, int choice) {
  inst->hotkeyChoice = choice;
}

extern "C" void wksettings_dlc_level(wksettings_t inst, int level) {
  inst->dlcLevel = level;
}

extern "C" void wksettings_resource_path(wksettings_t inst, const path_char_t* path) {
  inst->resourceDir = path;
}

extern "C" void wksettings_hd_path(wksettings_t inst, const path_char_t* path) {
  inst->hdPath = path;
}

extern "C" void wksettings_output_path(wksettings_t inst, const path_char_t* path) {
  inst->outPath = path;
}

extern "C" void wksettings_voobly_path(wksettings_t inst, const path_char_t* path) {
  inst->vooblyDir = path;
}

extern "C" void wksettings_up_path(wksettings_t inst, const path_char_t* path) {
  inst->upDir = path;
}

extern "C" void wksettings_mod_name(wksettings_t inst, const path_char_t* name) {
  inst->modName = name;
}

extern "C" void wksettings_data_mod(wksettings_t inst, const char* name, const char* exe, const char* version, int flags, const char* exe2) {
  // unimplemented
  /* inst->dataModList.emplace_back(name, exe, version, flags, exe2 == nullptr ? "" : exe2); */
}

extern "C" void wksettings_drs_resources(wksettings_t inst, const path_char_t* dir, WKSettings::IndexType type) {
  inst->addDrsResources(dir, type);
}

extern "C" wkconverter_t wkconverter_create(wksettings_t settings, void* context) {
  auto listener = new Listener(context);
  auto converter = new WKConverter(*settings, listener);
  return new wkconvert_handle{converter, listener};
}

extern "C" void wkconverter_on_finished(wkconverter_t inst, void (*onFinished)(void*)) {
  inst->listener->onFinished = onFinished;
}

extern "C" void wkconverter_on_log(wkconverter_t inst, void (*onLog)(void*, const char*)) {
  inst->listener->onLog = onLog;
}

extern "C" void wkconverter_on_set_info(wkconverter_t inst, void (*onSetInfo)(void*, const char*)) {
  inst->listener->onSetInfo = onSetInfo;
}

extern "C" void wkconverter_on_error(wkconverter_t inst, void (*onError)(void*, const char*)) {
  inst->listener->onError = onError;
}

extern "C" void wkconverter_on_progress(wkconverter_t inst, void (*onProgress)(void*, int)) {
  inst->listener->onProgress = onProgress;
}

extern "C" void wkconverter_free(wkconverter_t inst) {
  if (inst != nullptr) delete inst;
}

extern "C" int wkconverter_run(wkconverter_t inst) {
  try {
    return inst->converter->run();
  } catch (const std::exception& e) {
    inst->listener->error(e);
    return -3;
  }
}

extern "C" int wkconvert(wksettings_t settings, void* context) {
  Listener listener(context);
  WKConverter converter(*settings, &listener);
  return converter.run();
}
