#ifdef __cplusplus
extern "C" {
#endif

enum WKIndexType {
  DRSIndexOnly = 0,
  DRSExpansionResources = 1,
  DRSTerrainResources = 2,
};

// Create settings.
typedef void* wksettings_t;
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
void wksettings_resource_path(wksettings_t, const char* path);
void wksettings_hd_path(wksettings_t, const char* path);
void wksettings_output_path(wksettings_t, const char* path);
void wksettings_voobly_path(wksettings_t, const char* path);
void wksettings_up_path(wksettings_t, const char* path);
void wksettings_mod_name(wksettings_t, const char* name);

// Add mods.
void wksettings_data_mod(wksettings_t, const char* name, const char* exe, const char* version, int flags, const char* exe2);
void wksettings_drs_resources(wksettings_t, const char* dir, WKIndexType type);

// Converter setup.
typedef void* wkconverter_t;
wkconverter_t wkconverter_create(wksettings_t);
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
int wkconvert(wksettings_t);

#ifdef __cplusplus
}
#endif
