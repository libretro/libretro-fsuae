#include "sysconfig.h"
#include "sysdeps.h"

#include "libretro.h"
#include "gui-retro/dialog.h"
#include "../retrodep/retroglue.h"
#include "libretro-mapper.h"
#include "libretro-glue.h"
#include "uae_fs.h"
#include "uae.h"
#include "fs/emu/hacks.h"
#include "fs/conf.h"
#include "fs/emu.h"
#include "fs-uae/config-model.h"
#include "fs-uae/config-common.h"
#include "fs-uae/options.h"
#include "fs-uae/fs-uae.h"
#include "fs-uae/config-drives.h"
#include "fs/data.h"
#include "fs/emu/path.h"
#include "fs-uae/paths.h"

cothread_t mainThread;
cothread_t emuThread;

int defaultw = EMULATOR_DEF_WIDTH;
int defaulth = EMULATOR_DEF_HEIGHT;
int retrow = 0;
int retroh = 0;
int CROP_WIDTH;
int CROP_HEIGHT;
int sndbufpos=0;
char key_state[512];
char key_state2[512];
bool opt_analog = false;
static int firstps = 0;

extern unsigned short int  bmp[EMULATOR_MAX_WIDTH*EMULATOR_MAX_HEIGHT];
extern unsigned short int  savebmp[EMULATOR_MAX_WIDTH*EMULATOR_MAX_HEIGHT];
extern int pauseg;
extern int SND;
extern int SHIFTON;
extern int snd_sampler;
extern short signed int SNDBUF[1024*2];
extern char RPATH[512];
int ledtype;

char *g_fs_uae_config_dir_path = NULL; /*sources/src/fs-uae/main.c*/
char *g_fs_uae_config_file_path = NULL; /*sources/src/fs-uae/main.c*/

extern void update_input(void);

static retro_video_refresh_t video_cb;
static retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;
retro_environment_t environ_cb;

static struct retro_input_descriptor input_descriptors[] = {
   { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP, "Up" },
   { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN, "Down" },
   { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT, "Left" },
   { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Right" },
   { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A, "Fire" },
   { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y, "Enter GUI" },
   { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Mouse mode toggle" },
   { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Keyboard overlay" },
   { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2, "Toggle m/k status" },
   { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L, "Joystick number" },
   { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R, "Mouse speed" },
   // Terminate
   { 255, 255, 255, 255, NULL }
};

// FS_EMU_HACKS_H /* These variables should ideally be hidden, and a proper API designed instead. */
#ifdef FS_EMU_HACKS_H
//int fs_emu_mouse_absolute_x = 0;
//int fs_emu_mouse_absolute_y = 0;
int g_fs_log_input = 0;

double fs_emu_video_scale_x  = 1.0;
double fs_emu_video_scale_y  = 1.0;
double fs_emu_video_offset_x = 0.0;
double fs_emu_video_offset_y = 0.0;
#endif /* FS_EMU_HACKS_H */

//

void retro_set_environment(retro_environment_t cb)
{
   struct retro_variable variables[] = {
     { "resolution", "Internal resolution; 640x400|640x432|640x480|640x540|704x480|704x540|720x480|720x540|800x600|1024x768", },
     { "analog","Use Analog; OFF|ON", },
     { "leds","Leds; Standard|Simplified|None", },
     { NULL, NULL },
   };
  /*
    bool no_rom = true;
    cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &no_rom);
  */
   
   environ_cb = cb;
   cb(RETRO_ENVIRONMENT_SET_VARIABLES, variables);
}

static void update_variables(void)
{
   struct retro_variable var = {0};

   var.key = "resolution";
   var.value = NULL;

#if 0
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      char *pch;
      char str[100];
      snprintf(str, sizeof(str), var.value);

      pch = strtok(str, "x");
      if (pch)
         defaultw = strtoul(pch, NULL, 0);
      pch = strtok(NULL, "x");
      if (pch)
         defaulth = strtoul(pch, NULL, 0);
   }
#endif
   
   var.key = "analog";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "OFF") == 0)
        opt_analog = false;
      if (strcmp(var.value, "ON") == 0)
        opt_analog = true;
      ledtype = 1;
   }
   
   var.key = "leds";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "Standard") == 0)
      {
        ledtype = 0;        
      }
      if (strcmp(var.value, "Simplified") == 0)
      {
        ledtype = 1;  
      }
      if (strcmp(var.value, "None") == 0)
      {
        ledtype = 2;  
      }
   }
}

/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void fs_uae_load_rom_files(const char *path)
{
    fs_log("fs_uae_load_rom_files %s\n", path);
    GDir *dir = g_dir_open(path, 0, NULL);
    if (dir == NULL) {
        fs_log("error opening dir\n");
        return;
    }

    // we include the rom key when generating the cache name for the
    // kickstart cache file, so the cache will be regenerated if rom.key
    // is replaced or removed/added.
    char *key_path = g_build_filename(path, "rom.key", NULL);
    GChecksum *rom_checksum = g_checksum_new(G_CHECKSUM_MD5);
    FILE *f = g_fopen(key_path, "rb");
    if (f != NULL) {
        int64_t key_size = fs_path_get_size(key_path);
        if (key_size > 0 && key_size < 1024 * 1024) {
            guchar *key_data = malloc(key_size);
            if (fread(key_data, key_size, 1, f) != 1) {
                free(key_data);
            }
            else {
                fs_log("read rom key file, size = %d\n", key_size);
                g_checksum_update(rom_checksum, key_data, key_size);
            }
        }
        fclose(f);
    }
    g_free(key_path);

    amiga_add_key_dir(path);
    const char *name = g_dir_read_name(dir);
    while (name) {
        char *lname = g_utf8_strdown(name, -1);
        if (g_str_has_suffix(lname, ".rom")
                || g_str_has_suffix(lname, ".bin")) {
            fs_log("found file \"%s\"\n", name);
            char *full_path = g_build_filename(path, name, NULL);
            //GChecksum *checksum = g_checksum_new(G_CHECKSUM_MD5);
            GChecksum *checksum = g_checksum_copy(rom_checksum);
            g_checksum_update(
                checksum, (guchar *) full_path, strlen(full_path));
            const gchar *cache_name = g_checksum_get_string(checksum);
            char* cache_path = g_build_filename(
                fs_uae_kickstarts_cache_dir(), cache_name, NULL);
            amiga_add_rom_file(full_path, cache_path);
            // check if amiga_add_rom_file needs to own full_path
            //free(full_path);
            if (cache_path != NULL) {
                free(cache_path);
            }
            g_checksum_free(checksum);
        }
        free(lname);
        name = g_dir_read_name(dir);
    }
    g_dir_close(dir);

    if (rom_checksum != NULL) {
        g_checksum_free(rom_checksum);
    }
    //exit(1);
}

/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static const char *overlay_names[] = {
  "df0_led",     // 0
  "df1_led",     // 1
  "df2_led",     // 2
  "df3_led",     // 3
  "df0_disk",    // 4
  "df1_disk",    // 5
  "df2_disk",    // 6
  "df3_disk",    // 7
  "power_led",   // 8
  "hd_led",      // 9
  "cd_led",      // 10
  "md_led",      // 11

  "df0_d1",      // 12
  "df0_d0",
  "df1_d1",
  "df1_d0",
  "df2_d1",
  "df2_d0",
  "df3_d1",
  "df3_d0",
  NULL,
};
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void on_init() //  src/fs-uae/main.c: static void on_init()
{
    fs_log("-- uae configuration --\n");

    //amiga_set_gui_message_function(on_gui_message);

    //fs_uae_configure_amiga_model();
    fs_uae_configure_amiga_hardware();
    fs_uae_configure_floppies();
    fs_uae_configure_hard_drives();
    fs_uae_configure_cdrom();
#ifndef __LIBRETRO__
    fs_uae_configure_input();
#endif /*__LIBRETRO__*/
    fs_uae_configure_directories();

    if (fs_config_get_int("save_state_compression") == 0) {
        amiga_set_save_state_compression(0);
    }
    else {
        amiga_set_save_state_compression(1);
    }

    if (fs_config_get_int("min_first_line_pal") != FS_CONFIG_NONE) {
        amiga_set_min_first_line(fs_config_get_int("min_first_line_pal"), 0);
    }
    if (fs_config_get_int("min_first_line_ntsc") != FS_CONFIG_NONE) {
        amiga_set_min_first_line(fs_config_get_int("min_first_line_ntsc"), 1);
    }

    /*
    if (fs_emu_get_video_sync()) {
        fs_log("fs_emu_get_video_sync returned true\n");
        amiga_set_option("gfx_vsync", "true");
    }
    else {
        fs_log("fs_emu_get_video_sync returned false\n");
    }
    if (fs_emu_netplay_enabled()) {
        fs_log("netplay is enabled\n");
        // make sure UAE does not sleep between frames, we must be able
        // to control sleep times for net play
        amiga_set_option("gfx_vsync", "true");
    }
    */

    /* With sound_auto set to true, UAE stops audio output if the amiga does
     * not produce sound.  */
    // amiga_set_option("sound_auto", "false");

#ifdef __LIBRETRO__
    amiga_set_audio_frequency(44100);
#endif /*__LIBRETRO__*/

    // set the input frequency to the output frequency, since we configured
    // libamiga to output at the same frequency

    // FIXME: check the actual frequency libuae/libamiga outputs, seems
    // to output at 44100 Hz even though currprefs.freq says 48000.
    // fs_emu_set_audio_buffer_frequency(0, fs_emu_get_audio_frequency());

    //amiga_set_option("gfx_gamma", "40");

    fs_uae_set_uae_paths();
    //fs_uae_read_custom_uae_options(fs_uae_argc, fs_uae_argv);

    char *uae_file;

    uae_file = g_build_filename(fs_uae_logs_dir(), "LastConfig.uae", NULL);
    if (fs_path_exists(uae_file)) {
        g_unlink(uae_file);
    }
    g_free(uae_file);

    uae_file = g_build_filename(fs_uae_logs_dir(), "DebugConfig.uae", NULL);
    if (fs_path_exists(uae_file)) {
        g_unlink(uae_file);
    }
    g_free(uae_file);

    uae_file = g_build_filename(fs_uae_logs_dir(), "Debug.uae", NULL);
    if (fs_path_exists(uae_file)) {
        g_unlink(uae_file);
    }
    g_free(uae_file);

    uae_file = g_build_filename(fs_uae_logs_dir(), "debug.uae", NULL);
    amiga_write_uae_config(uae_file);
    g_free(uae_file);

    fs_log("\n");
    fs_log("-- end of uae configuration --\n");
    fs_log("\n");
}

/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
char *g_fs_uae_disk_file_path = NULL;
static int g_warn_about_missing_config_file;
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void retro_wrap_emulator(void)
{
#ifdef LIBRETRO_FSUAE
  static char *argv[] = { "fs-uae" };
#else
  static char *argv[] = { "puae" };
#endif

  //uae_restart(1, RPATH);
   keyboard_settrans();

   fs_init();  // sources/src/fs-uae/main.c: main()
   int error = fs_data_init("fs-uae", "fs-uae.dat");
   if (error) {
     fprintf(stderr, "WARNING: error (%d) loading fs-uae.dat\n", error);
   }

   fs_set_prgname("libretro-fsuae");
   fs_set_application_name("Amiga Emulator");

   //fprintf(stderr, COPYRIGHT_NOTICE, PACKAGE_VERSION, OS_NAME_2, ARCH_NAME_2);

   amiga_init();


   fs_emu_path_set_expand_function(fs_uae_expand_path);
   fs_emu_init_overlays(overlay_names);
   fs_emu_init();

   // load_config_file();
   {
     fprintf(stderr, "loading config from %s\n", RPATH); 
     fs_config_read_file(RPATH, 0); // ../fs-uae-2.7.14dev/src/fs-uae/main.c
   }
   fs_set_data_dir(fs_uae_data_dir());
   
   if (g_fs_uae_disk_file_path) {
     fs_config_set_string(OPTION_FLOPPY_DRIVE_0, g_fs_uae_disk_file_path);
     g_warn_about_missing_config_file = 0;
   }

   if (g_warn_about_missing_config_file) {
     fs_emu_warning(("No configuration file was found"));
   }

   fs_emu_set_state_check_function(amiga_get_state_checksum);
   fs_emu_set_rand_check_function(amiga_get_rand_checksum);
   
   fs_uae_kickstarts_dir();
   fs_uae_configurations_dir();
   fs_uae_init_path_resolver();

   //fs_uae_plugins_init();
   
   // must be called early, before fs_emu_init -affects video output
   fs_uae_configure_amiga_model();
   fs_uae_state_dir();

   fs_emu_init_2(FS_EMU_INIT_EVERYTHING);

   int deterministic_mode = 0;
   if (fs_emu_netplay_enabled() ||
       fs_config_get_boolean(OPTION_DETERMINISTIC) == 1) {
     deterministic_mode = 1;
   }
   if (deterministic_mode) {
     amiga_set_deterministic_mode();
   }

#if 0
   amiga_set_audio_callback(audio_callback_function);
   amiga_set_cd_audio_callback(audio_callback_function);
   amiga_set_event_function(event_handler);

   amiga_set_led_function(led_function);
   amiga_on_update_leds(on_update_leds);

   amiga_set_media_function(media_function);
#endif
   amiga_set_init_function(on_init);

   if (fs_config_get_boolean(OPTION_JIT_COMPILER) == 1) {
     amiga_init_jit_compiler();
   }

   amiga_set_video_format(AMIGA_VIDEO_FORMAT_R5G6B5);

   //amiga_set_video_format(AMIGA_VIDEO_FORMAT_RGBA);
   //amiga_set_video_format(AMIGA_VIDEO_FORMAT_BGRA);
   //amiga_set_video_format(AMIGA_VIDEO_FORMAT_R5G6B5);
   //amiga_set_video_format(AMIGA_VIDEO_FORMAT_R5G5B5A1);

   amiga_add_rtg_resolution(672, 540);
   amiga_add_rtg_resolution(960, 540);
   amiga_add_rtg_resolution(retrow, retroh);
   //fs_uae_init_video();
   fs_uae_init_mouse();
   //fs_emu_run(main_function);
   //cleanup_old_files();
   
   if (/*fs_emu_netplay_enabled()*/ 0) {
     fs_log("netplay is enabled\n");
     // make sure UAE does not sleep between frames, we must be able
     // to control sleep times for net play
     amiga_set_option("gfx_vsync", "true");
   }
   
   fs_emu_toggle_zoom(0);
   
#ifdef AHI
   //enforcer_enable(1);
#endif
   real_main(sizeof(argv)/sizeof(*argv), argv); /* e.g.: .fs-uae-2.7.14dev/src/od-fs/libamiga.cpp */

   pauseg = -1;

   environ_cb(RETRO_ENVIRONMENT_SHUTDOWN, 0); 

   /* We're done here */
   co_switch(mainThread);

   /* Dead emulator, 
    * but libco says not to return. */
   while(true)
   {
      LOGI("Running a dead emulator.");
      co_switch(mainThread);
   }
}

/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void retro_init(void)
{
   enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_RGB565;

   memset(key_state, 0, sizeof(key_state));
   memset(key_state2, 0, sizeof(key_state2));

   if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
   {
      fprintf(stderr, "[libretro-uae]: RGB565 is not supported.\n");
      exit(0);//return false;
   }

   InitOSGLU();
   memset(bmp, 0, sizeof(bmp));

   update_variables();

   CROP_WIDTH    = retrow;
   CROP_HEIGHT   = (retroh - 80);

   if(!emuThread && !mainThread)
   {
      mainThread = co_active();
      emuThread = co_create(65536 * sizeof(void*), retro_wrap_emulator);
   }
}

void retro_deinit(void)
{	
   UnInitOSGLU();	

   if(emuThread)
      co_delete(emuThread);
   emuThread = 0;
}

unsigned retro_api_version(void)
{
   return RETRO_API_VERSION;
}

void retro_set_controller_port_device(unsigned port, unsigned device)
{
   (void)port;
   (void)device;
}

void retro_get_system_info(struct retro_system_info *info)
{
   memset(info, 0, sizeof(*info));
   info->library_name     = "PUAE";
   info->library_version  = PACKAGE_STRING;
   info->need_fullpath    = true;
   info->block_extract    = false;	
   info->valid_extensions = "adf|dms|fdi|ipf|zip|uae";
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
   static struct retro_game_geometry geom480   = { 640, 480, EMULATOR_MAX_WIDTH, EMULATOR_MAX_HEIGHT, 4.0 / 3.0 };
   static struct retro_game_geometry geom540   = { 720, 540, EMULATOR_MAX_WIDTH, EMULATOR_MAX_HEIGHT, 4.0 / 3.0 };
   struct retro_system_timing timing = { 50.0, 44100.0 };

   if      (retrow == 640 && retroh == 400) info->geometry = geom480;
   else if (retrow == 640 && retroh == 400) info->geometry = geom480;
   else if (retrow == 720 && retroh == 540) info->geometry = geom540;
   else { static struct retro_game_geometry geom; geom.base_width=retrow; geom.base_height=retroh; geom.max_width=EMULATOR_MAX_WIDTH; geom.max_height=EMULATOR_MAX_HEIGHT; geom.aspect_ratio=(float)retrow/(float)retroh; info->geometry = geom; }
   info->timing   = timing;
}

void retro_set_audio_sample(retro_audio_sample_t cb)
{
   audio_cb = cb;
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
   audio_batch_cb = cb;
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
   video_cb = cb;
}

void retro_shutdown_uae(void)
{
   environ_cb(RETRO_ENVIRONMENT_SHUTDOWN, NULL);
}

void retro_reset(void) {
}

void retro_audio_cb( short l, short r)
{
   audio_cb(l,r);
}

extern unsigned short * sndbuffer;
extern int sndbufsize;
signed short rsnd = 0;
static int firstpass = 1;

void save_bkg(void) {
  memcpy(savebmp, bmp,sizeof(bmp));
}

void restore_bkg(void) {
   memcpy(bmp, savebmp, sizeof(bmp));
}

void enter_gui0(void) {
  save_bkg();

  Dialog_DoProperty();
  pauseg = 0;

  restore_bkg();
}

void pause_select(void) {
  if(pauseg==1 && firstps==0)
    {
      firstps=1;
      enter_gui0();
      firstps=0;
    }
}

void retro_run(void) {
  bool updated = false;

#ifdef DEBUG_RETRORUN
  fprintf(stderr, "%s %d %s --------------------\n", __FILE__, __LINE__, __FUNCTION__);
#endif /*DEBUG*/
   
  if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
    update_variables();

  if (pauseg==0) {
    if (firstpass) {
      firstpass=0;
      goto sortie;
    }
    update_input();	
  }

 sortie:
  video_cb(bmp,retrow,retroh , retrow << 1);
#ifdef DEBUG_RETRORUN
  fprintf(stderr, "%s %d %s --------------------\n", __FILE__, __LINE__, __FUNCTION__);
#endif /*DEBUG*/
  co_switch(emuThread);
#ifdef DEBUG_RETRORUN
  fprintf(stderr, "%s %d %s --------------------\n", __FILE__, __LINE__, __FUNCTION__);
#endif /*DEBUG*/
}

bool retro_load_game(const struct retro_game_info *info)
{
  int w = 0, h = 0;

  environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, input_descriptors);

  RPATH[0] = '\0';

  if (*info->path)
    {
      const char *full_path = (const char*)info->path;
      strncpy(RPATH, full_path, sizeof(RPATH));

      // checking parsed file for custom resolution
      FILE * configfile;

      char filebuf[4096];
      if((configfile = fopen (RPATH, "r")))
	{
	  while(fgets(filebuf, sizeof(filebuf), configfile))
	    {
	      sscanf(filebuf,"gfx_width = %d",&w);
	      sscanf(filebuf,"gfx_height = %d",&h);
	    }
	  fclose(configfile);
	}
    }

  if (w<=0 || h<=0 || w>EMULATOR_MAX_WIDTH || h>EMULATOR_MAX_HEIGHT) {
    w = defaultw;
    h = defaulth;
    }

  fprintf(stderr, "[libretro-uae]: resolution selected: %dx%d (default: %dx%d)\n", w, h, defaultw, defaulth);

  retrow = w;
  retroh = h;
  CROP_WIDTH = retrow;
  CROP_HEIGHT = (retroh-80);
  memset(bmp, 0, sizeof(bmp));
  Screen_SetFullUpdate();
  return true;
}

void retro_unload_game(void)
{
   pauseg = 0;
}

unsigned retro_get_region(void)
{
   return RETRO_REGION_NTSC;
}

bool retro_load_game_special(unsigned type, const struct retro_game_info *info, size_t num)
{
   (void)type;
   (void)info;
   (void)num;
   return false;
}

size_t retro_serialize_size(void)
{
   return 0;
}

bool retro_serialize(void *data_, size_t size)
{
   return false;
}

bool retro_unserialize(const void *data_, size_t size)
{
   return false;
}

void *retro_get_memory_data(unsigned id)
{
   (void)id;
   return NULL;
}

size_t retro_get_memory_size(unsigned id)
{
   (void)id;
   return 0;
}

void retro_cheat_reset(void) {}

void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
   (void)index;
   (void)enabled;
   (void)code;
}
