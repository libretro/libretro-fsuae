/*
	modded for libretro-uae
*/

/*
  Hatari - dlgFloppy.c

  This file is distributed under the GNU Public License, version 2 or at
  your option any later version. Read the file gpl.txt for details.
*/
const char DlgFloppy_fileid[] = "Hatari dlgFloppy.c : " __DATE__ " " __TIME__;

#include <assert.h>

#include "dialog.h"
#include "sdlgui.h"
#include "file.h"

#include "sysconfig.h"
#include "sysdeps.h"
#include "uae.h"
#include "options.h"
#include "disk.h"

#include "uae/uae.h"
#include <fs/emu.h>
#ifdef USE_GLIB
#include <glib.h>
#endif /*USE_GLIB*/

#define Log_AlertDlg fprintf
#define LOG_INFO stderr
#define LOG_ERROR stderr

static const char * const pszDiskImageNameExts[] =
{
	".adf",
	".adz",
	".ipf",
	".dms",
	".zip",
	NULL
};

#define MAX_FLOPPYDRIVES 4

char szDiskZipPath[MAX_FLOPPYDRIVES][FILENAME_MAX]={ {'\0'},{'\0'}, {'\0'},{'\0'}};
char szDiskFileName[MAX_FLOPPYDRIVES][FILENAME_MAX]={ {'\0'},{'\0'}, {'\0'},{'\0'}};
char szDiskImageDirectory[FILENAME_MAX]={'\0'};

#define FLOPPYDLG_EJECTA      3
#define FLOPPYDLG_BROWSEA     4
#define FLOPPYDLG_DISKA       5
#define FLOPPYDLG_EJECTB      7
#define FLOPPYDLG_BROWSEB     8
#define FLOPPYDLG_DISKB       9
#define FLOPPYDLG_EJECT2      11
#define FLOPPYDLG_BROWSE2     12
#define FLOPPYDLG_DISK2       13
#define FLOPPYDLG_EJECT3      15
#define FLOPPYDLG_BROWSE3     16
#define FLOPPYDLG_DISK3       17
#define FLOPPYDLG_IMGDIR      19
#define FLOPPYDLG_BROWSEIMG   20
#define FLOPPYDLG_EXIT        22
#ifdef LIBRETRO_FSUAE
#define FLOPPY_SWAPMAX         8
#else /*LIBRETRO_FSUAE*/
#define FLOPPY_SWAPMAX         0
#endif /*LIBRETRO_FSUAE*/



/* The floppy disks dialog: */
static SGOBJ floppydlg[FLOPPYDLG_EXIT + 1 + FLOPPY_SWAPMAX] =
{
	{ SGBOX, 0, 0, 0,0, 64,20, NULL },
	{ SGTEXT, 0, 0, 25,1, 12,1, "Floppy disks" },
	{ SGTEXT, 0, 0, 2,3, 8,1, "DF0:" },
	{ SGBUTTON,  SG_EXIT/*0*/, 0, 46,3, 7,1, "Eject" },
	{ SGBUTTON,  SG_EXIT/*0*/, 0, 54,3, 8,1, "Browse" },
	{ SGTEXT, 0, 0, 3,4, 58,1, NULL },
	{ SGTEXT, 0, 0, 2,6, 8,1, "DF1:" },
	{ SGBUTTON,  SG_EXIT/*0*/, 0, 46,6, 7,1, "Eject" },
	{ SGBUTTON,  SG_EXIT/*0*/, 0, 54,6, 8,1, "Browse" },
	{ SGTEXT, 0, 0, 3,7, 58,1, NULL },
	{ SGTEXT, 0, 0, 2,9, 8,1, "DF2:" },
	{ SGBUTTON,  SG_EXIT/*0*/, 0, 46,9, 7,1, "Eject" },
	{ SGBUTTON,  SG_EXIT/*0*/, 0, 54,9, 8,1, "Browse" },
	{ SGTEXT, 0, 0, 3,10, 58,1, NULL },
	{ SGTEXT, 0, 0, 2,12, 8,1, "DF3:" },
	{ SGBUTTON,  SG_EXIT/*0*/, 0, 46,12, 7,1, "Eject" },
	{ SGBUTTON,  SG_EXIT/*0*/, 0, 54,12, 8,1, "Browse" },

	{ SGTEXT, 0, 0, 3,13, 58,1, NULL },
	{ SGTEXT, 0, 0, 2,14, 32,1, "Default floppy images directory:" },
	{ SGTEXT, 0, 0, 3,15, 58,1, NULL },
	{ SGBUTTON,  SG_EXIT/*0*/, 0, 54,14, 8,1, "Browse" },

	{ SGTEXT, 0, 0, 3,16, 58,1, NULL },
	{ SGBUTTON, SG_EXIT/*SG_DEFAULT*/, 0, 22,18, 20,1, "Back to main menu" },
	{ -1, 0, 0, 0,0, 0,0, NULL }
};

#ifdef LIBRETRO_FSUAE
#define FLOPPYDLG_SWAP 23
int floppyswapnum = 0;

struct floppyswap {
  int y;
  int x;
} floppyswap[FLOPPY_SWAPMAX];
#endif /*LIBRETRO_FSUAE*/

#define DLGMOUNT_A       2
#define DLGMOUNT_B       3
#define DLGMOUNT_CANCEL  4

#if 0
/* The "Alert"-dialog: */
static SGOBJ alertdlg[] =
{
	{ SGBOX, 0, 0, 0,0, 40,6, NULL },
	{ SGTEXT, 0, 0, 3,1, 30,1, "Insert last created disk to?" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 3,4, 10,1, "Drive A:" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 15,4, 10,1, "Drive B:" },
	{ SGBUTTON, SG_EXIT/*SG_CANCEL*/, 0, 27,4, 10,1, "Cancel" },
	{ -1, 0, 0, 0,0, 0,0, NULL }
};
#endif

/*-----------------------------------------------------------------------*/
/**
 * Set floppy image to be ejected
 */
const char* Floppy_SetDiskFileNameNone(int Drive)
{
	assert(Drive >= 0 && Drive < MAX_FLOPPYDRIVES);
	szDiskFileName[Drive][0] = '\0';
	return szDiskFileName[Drive];
}

/*-----------------------------------------------------------------------*/
/**
 * Set given floppy drive image file name and handle
 * different image extensions.
 * Return corrected file name on success and NULL on failure.
 */
const char* Floppy_SetDiskFileName(int Drive, const char *pszFileName, const char *pszZipPath)
{
	char *filename;
	int i;

	/* setting to empty or "none" ejects */
	if (!*pszFileName || strcasecmp(pszFileName, "none") == 0)
	{
		return Floppy_SetDiskFileNameNone(Drive);
	}
	/* See if file exists, and if not, get/add correct extension */
	if (!File_Exists(pszFileName))
		filename = File_FindPossibleExtFileName(pszFileName, pszDiskImageNameExts);
	else
		filename = strdup(pszFileName);
	if (!filename)
	{
		Log_AlertDlg(LOG_INFO, "Image '%s' not found", pszFileName);
		return NULL;
	}
#if 0
	/* If we insert a disk into Drive A, should we try to put disk 2 into drive B? */
	if (Drive == 0 && ConfigureParams.DiskImage.bAutoInsertDiskB)
	{
		/* Attempt to make up second filename, eg was 'auto_100a' to 'auto_100b' */
		char *szDiskBFileName = Floppy_CreateDiskBFileName(filename);
		if (szDiskBFileName)
		{
			/* recurse with Drive B */
			Floppy_SetDiskFileName(1, szDiskBFileName, pszZipPath);
			free(szDiskBFileName);
		}
	}
#endif

	/* validity checks */
	assert(Drive >= 0 && Drive < MAX_FLOPPYDRIVES);
	for (i = 0; i < MAX_FLOPPYDRIVES; i++)
	{
		if (i == Drive)
			continue;
		/* prevent inserting same image to multiple drives */
		if (strcmp(filename, /*ConfigureParams.DiskImage.*/szDiskFileName[i]) == 0)
		{
			Log_AlertDlg(LOG_ERROR, "ERROR: Cannot insert same floppy to multiple drives!");
			return NULL;
		}
	}

	/* do the changes */
	if (pszZipPath)
		strcpy(szDiskZipPath[Drive], pszZipPath);
	else
		szDiskZipPath[Drive][0] = '\0';
	strcpy(szDiskFileName[Drive], filename);
	free(filename);
	//File_MakeAbsoluteName(ConfigureParams.DiskImage.szDiskFileName[Drive]);
	return szDiskFileName[Drive];
}



/**
 * Let user browse given disk, insert disk if one selected.
 */
static void DlgDisk_BrowseDisk(char *dlgname, int drive, int diskid)
{
	char *selname, *zip_path;
	const char *tmpname, *realname;

	assert(drive >= 0 && drive < MAX_FLOPPYDRIVES);
	if (szDiskFileName[drive][0])
		tmpname = szDiskFileName[drive];
	else
		tmpname = szDiskImageDirectory;

	selname = SDLGui_FileSelect(tmpname, &zip_path, false);
	if (!selname)
		return;

	if (File_Exists(selname))
	{
		realname = Floppy_SetDiskFileName(drive, selname, zip_path);
		if (realname)
			File_ShrinkName(dlgname, realname, floppydlg[diskid].w);
	}
	else
	{
		Floppy_SetDiskFileNameNone(drive);
		dlgname[0] = '\0';
	}
	if (zip_path)
		free(zip_path);
	free(selname);
}


/**
 * Let user browse given directory, set directory if one selected.
 */
static void DlgDisk_BrowseDir(char *dlgname, char *confname, int maxlen)
{
	char *str, *selname;

	selname = SDLGui_FileSelect(confname, NULL, false);
	if (!selname)
		return;

	strcpy(confname, selname);
	free(selname);

	str = strrchr(confname, PATHSEP);
	if (str != NULL)
		str[1] = 0;
	File_CleanFileName(confname);
	File_ShrinkName(dlgname, confname, maxlen);
}


#if 0
/**
 * Ask whether new disk should be inserted to A: or B: and if yes, insert.
 */
static void DlgFloppy_QueryInsert(char *namea, int ida, char *nameb, int idb, const char *path)
{
	const char *realname;
	int diskid, dlgid;
	char *dlgname;

	SDLGui_CenterDlg(alertdlg);

int but;

do{        
	but=SDLGui_DoDialog(alertdlg, NULL);

	{
		if(but== DLGMOUNT_A){
			dlgname = namea;
			dlgid = ida;
			diskid = 0;
                }
		else if(but == DLGMOUNT_B){
			dlgname = nameb;
			dlgid = idb;
			diskid = 1;
                }
		
	}
        gui_poll_events();
}
while (but != DLGMOUNT_CANCEL && but != DLGMOUNT_A  && but != DLGMOUNT_B && but != SDLGUI_QUIT
	        && but != SDLGUI_ERROR && !bQuitProgram);


	realname = Floppy_SetDiskFileName(diskid, path, NULL);
	if (realname)
		File_ShrinkName(dlgname, realname, floppydlg[dlgid].w);
}
#endif

#ifdef LIBRETRO_FSUAE
static void get_drive_for_index(int index, int *type, int *drive) {
    int count = 0;
    int num_floppy_drives = amiga_get_num_floppy_drives();
    int num_cdrom_drives = amiga_get_num_cdrom_drives();
#if 0
    if (g_fs_uae_amiga_model == MODEL_CD32 ||
            g_fs_uae_amiga_model == MODEL_CDTV) {
        if (num_cdrom_drives < 1) {
            num_cdrom_drives = 1;
        }
    }
#endif /*0*/
    //printf("num drives: floppy %d cd-rom %d\n", num_floppy_drives,
    //        num_cdrom_drives);

    for (int i = 0; i < num_cdrom_drives; i++) {
        if (index == count) {
            *type = 1;
            *drive = i;
            return;
        }
        count++;
    }
    for (int i = 0; i < num_floppy_drives; i++) {
        if (index == count) {
            *type = 0;
            *drive = i;
            return;
        }
        count++;
    }
    *type = -1;
    *drive = 0;
}

static char *get_floppy_label(const char* path) {
  if (!path || path[0] == '\0') {
    return g_strdup("");
  }
  char *name = g_path_get_basename(path);
#ifdef USE_GLIB
  GError *error = NULL;
  GRegex *re = g_regex_new("([A-Za-z0-9_ ]*[Dd][Ii][Ss][Kk][A-Za-z0-9_ ]*)",  0, 0, &error);
  if (error) {
    fprintf(stderr, " *** error\n");
    return name;
  }
  GMatchInfo *mi = NULL;
  if (!g_regex_match(re, name, 0, &mi) || !g_match_info_matches(mi)) {
    //fprintf(stderr, " *** false\n");
    g_match_info_free(mi);
    g_regex_unref(re);
    return name;
  }
  //fprintf(stderr, " *** ok?\n");
  char *result = g_match_info_fetch(mi, 1);
  g_match_info_free(mi);
  g_regex_unref(re);
  if (!result) {
    return name;
  }
  g_free(name);
  return result;
#else
  return name;
#endif
}

#define disk_eject(x) insert_disk((x), -1)

static void insert_disk(int drive_index, int disk_index) {
  if (disk_index == -1) {
    fprintf(stderr, "menu: eject disk from drive %d\n", drive_index);
    int action = INPUTEVENT_SPC_EFLOPPY0 + drive_index;
    //fs_emu_queue_action(action, 1);
    ////fs_emu_queue_action(action, 0);
    amiga_send_input_event(action, 1);
    return;
  }
  fprintf(stderr, "menu: insert disk index %d into df%d\n", disk_index, drive_index);
  int action = INPUTEVENT_SPC_DISKSWAPPER_0_0;
  action += drive_index * AMIGA_FLOPPY_LIST_SIZE + disk_index;
  //fs_emu_queue_action(action, 1);
  amiga_send_input_event(action, 1);
}

static int media_menu_function() {
  if (floppyswapnum) {
    for (int i=0;i<floppyswapnum;i++) free(floppydlg[FLOPPYDLG_SWAP+i].txt);
  }

  floppyswapnum = 0;

  for (int index = 0;index < 4 && floppyswapnum < FLOPPY_SWAPMAX;index++) {
    int drive, type;
    get_drive_for_index(index, &type, &drive);

    if (type == 0) { // floppy
      char *str;
      //fprintf(stderr, "%s:%d for df%d\n", __FUNCTION__, __LINE__, drive);

      for (int i = -1; i < AMIGA_FLOPPY_LIST_SIZE && floppyswapnum < FLOPPY_SWAPMAX; i++) {
	if (i == -1) {
	  //fs_emu_menu_item_set_title(item, _("Eject"));
	}
	else {
	  const char *path = amiga_floppy_get_list_entry(i);
	  int n;
	  str = get_floppy_label(path);
	  //fs_emu_menu_item_set_title(item, str);
	  if (str != NULL && (n=strlen(str)) != 0) {
	    char nam[256] = {0};
	    char buf[32] = {0};
	    if (n<sizeof(nam)) {
	      strncpy(nam, str, sizeof(nam));
	      if (n>=4 && !strcmp(".adf", &nam[n-4])) {
		nam[n-4]=0;
		sprintf(buf, "%d:%s", index, (n<14+4 ? nam:&nam[n-14-4]));
	      }
	      else
		sprintf(buf, "%d:%s", index, (n<14 ? nam:&nam[n-14]));
	      //fprintf(stderr, "%s:%d index=%d,i=%d <%s> -> <%s>\n", __FUNCTION__, __LINE__, index, i, str, buf);
	      floppyswap[floppyswapnum].x = index;
	      floppyswap[floppyswapnum].y = i;

	      //{ SGBUTTON,  SG_EXIT/*0*/, 0, 46,12, 7,1, "Eject" }, /*15*/
	      floppydlg[FLOPPYDLG_SWAP+floppyswapnum].type  = SGBUTTON;
	      floppydlg[FLOPPYDLG_SWAP+floppyswapnum].flags = SG_EXIT;
	      floppydlg[FLOPPYDLG_SWAP+floppyswapnum].state = 0;
	      floppydlg[FLOPPYDLG_SWAP+floppyswapnum].x     = 2 + 16*floppyswapnum;
	      floppydlg[FLOPPYDLG_SWAP+floppyswapnum].y     = 16;
	      floppydlg[FLOPPYDLG_SWAP+floppyswapnum].w     = 16;
	      floppydlg[FLOPPYDLG_SWAP+floppyswapnum].h     = 1;
	      floppydlg[FLOPPYDLG_SWAP+floppyswapnum].txt = strdup(buf);
	      floppyswapnum++;
	    }
	  }
	  free(str);
	}

	//fs_emu_menu_item_set_type(item, FS_EMU_MENU_ITEM_TYPE_ITEM);
	//fs_emu_menu_item_set_idata(item, i);
	if (drive == 0) {
	  //fs_emu_menu_item_set_activate_function(item, df0_function);
	}
	else if (drive == 1) {
	  //fs_emu_menu_item_set_activate_function(item, df1_function);
	}
	else if (drive == 2) {
	  //fs_emu_menu_item_set_activate_function(item, df2_function);
	}
	else if (drive == 3) {
	  //fs_emu_menu_item_set_activate_function(item, df3_function);
	}
      }
    }
    else {
      //fprintf(stderr, "%s: type=%d not supported...\n", __FUNCTION__, type);
    }
  }

  floppydlg[FLOPPYDLG_SWAP+floppyswapnum].type  = -1;
  floppydlg[FLOPPYDLG_SWAP+floppyswapnum].flags = 0;
  floppydlg[FLOPPYDLG_SWAP+floppyswapnum].state = 0;
  floppydlg[FLOPPYDLG_SWAP+floppyswapnum].x     = 0;
  floppydlg[FLOPPYDLG_SWAP+floppyswapnum].y     = 0;
  floppydlg[FLOPPYDLG_SWAP+floppyswapnum].w     = 0;
  floppydlg[FLOPPYDLG_SWAP+floppyswapnum].h     = 0;
  floppydlg[FLOPPYDLG_SWAP+floppyswapnum].txt = NULL;
  return 0;
}
#endif /*LIBRETRO_FSUAE*/

/**
 * Show and process the floppy disk image dialog.
 */
void DlgFloppy_Main(void)
{
	int but;
	//char *newdisk;
	char dlgname[MAX_FLOPPYDRIVES][64], dlgdiskdir[64];

	SDLGui_CenterDlg(floppydlg);

#ifdef LIBRETRO_FSUAE
	media_menu_function();
#endif /*LIBRETRO_FSUAE*/

	/* Set up dialog to actual values: */

	/* Disk name 0: */
	if(currprefs.floppyslots[1 - 1].df!=NULL)
		File_ShrinkName(dlgname[0], currprefs.floppyslots[0].df,floppydlg[FLOPPYDLG_DISKA].w);
	else
		dlgname[0][0] = '\0';

	floppydlg[FLOPPYDLG_DISKA].txt = dlgname[0];

	/* Disk name 1: */
	if(currprefs.floppyslots[2 - 1].df!=NULL)
		File_ShrinkName(dlgname[1], currprefs.floppyslots[1].df,floppydlg[FLOPPYDLG_DISKB].w);
	else
		dlgname[1][0] = '\0';
	floppydlg[FLOPPYDLG_DISKB].txt = dlgname[1];


	/* Disk name 2: */
	if(currprefs.floppyslots[3 - 1].df!=NULL)
		File_ShrinkName(dlgname[2], currprefs.floppyslots[2].df,floppydlg[FLOPPYDLG_DISK2].w);
	else
		dlgname[0][0] = '\0';

	floppydlg[FLOPPYDLG_DISK2].txt = dlgname[2];

	/* Disk name 3: */
	if(currprefs.floppyslots[4 - 1].df!=NULL)
		File_ShrinkName(dlgname[3],currprefs.floppyslots[3].df,floppydlg[FLOPPYDLG_DISK3].w);
	else
		dlgname[1][0] = '\0';
	floppydlg[FLOPPYDLG_DISK3].txt = dlgname[3];


	/* Default image directory: */
	File_ShrinkName(dlgdiskdir,szDiskImageDirectory,
	                floppydlg[FLOPPYDLG_IMGDIR].w);
	floppydlg[FLOPPYDLG_IMGDIR].txt = dlgdiskdir;


	/* Draw and process the dialog */
	do
	{       
		but = SDLGui_DoDialog(floppydlg, NULL);
		switch (but)
		{
		 case FLOPPYDLG_EJECTA:                         /* Eject disk in drive A: */
			Floppy_SetDiskFileNameNone(0);
			dlgname[0][0] = '\0';

				changed_prefs.floppyslots[0].df[0] = 0;
#ifndef LIBRETRO_FSUAE
				DISK_check_change();
#endif /*LIBRETRO_FSUAE*/
				disk_eject(0);
			break;
		 case FLOPPYDLG_BROWSEA:                        /* Choose a new disk A: */
			DlgDisk_BrowseDisk(dlgname[0], 0, FLOPPYDLG_DISKA);

			if (strlen(szDiskFileName[0]) > 0){

					if (currprefs.nr_floppies-1 < 0 ) {
						currprefs.nr_floppies = 0  + 1;
					}
					
					//check whether drive is enabled
					if (currprefs.floppyslots[0].dfxtype < 0) {
						changed_prefs.floppyslots[0 ].dfxtype = 0;
						DISK_check_change();
					}
					strcpy (changed_prefs.floppyslots[0 ].df,szDiskFileName[0]);

					DISK_check_change();	
					//disk_eject(0);
					//disk_insert (0, changed_prefs.floppyslots[0 ].df, false);

			}

			break;
		 case FLOPPYDLG_EJECTB:                         /* Eject disk in drive B: */
			Floppy_SetDiskFileNameNone(1);
			dlgname[1][0] = '\0';

				changed_prefs.floppyslots[1].df[0] = 0;
#ifndef LIBRETRO_FSUAE
				DISK_check_change();
#endif /*LIBRETRO_FSUAE*/
				disk_eject(1);

			break;
		case FLOPPYDLG_BROWSEB:                         /* Choose a new disk B: */
			DlgDisk_BrowseDisk(dlgname[1], 1, FLOPPYDLG_DISKB);
			if (strlen(szDiskFileName[1]) > 0){

					if (currprefs.nr_floppies-1 < 1 ) {
						currprefs.nr_floppies = 1  + 1;
					}

					//check whether drive is enabled
					if (currprefs.floppyslots[1].dfxtype < 0) {
						changed_prefs.floppyslots[1 ].dfxtype = 0;
						DISK_check_change();
					}
					strcpy (changed_prefs.floppyslots[1 ].df,szDiskFileName[1]);
					DISK_check_change();
			}


		 case FLOPPYDLG_EJECT2:                         /* Eject disk in drive A: */
			Floppy_SetDiskFileNameNone(2);
			dlgname[2][0] = '\0';

				changed_prefs.floppyslots[2].df[0] = 0;
#ifndef LIBRETRO_FSUAE
				DISK_check_change();
#endif /*LIBRETRO_FSUAE*/
				disk_eject(2);
			break;
		 case FLOPPYDLG_BROWSE2:                        /* Choose a new disk A: */
			DlgDisk_BrowseDisk(dlgname[2], 0, FLOPPYDLG_DISK2);

			if (strlen(szDiskFileName[2]) > 0){

					if (currprefs.nr_floppies-1 < 2 ) {
						currprefs.nr_floppies = 2  + 1;
					}
					
					//check whether drive is enabled
					if (currprefs.floppyslots[2].dfxtype < 0) {
						changed_prefs.floppyslots[2 ].dfxtype = 0;
						DISK_check_change();
					}
					strcpy (changed_prefs.floppyslots[2 ].df,szDiskFileName[2]);

					DISK_check_change();	
					//disk_eject(0);
					//disk_insert (0, changed_prefs.floppyslots[0 ].df, false);

			}

			break;
		 case FLOPPYDLG_EJECT3:                         /* Eject disk in drive B: */
			Floppy_SetDiskFileNameNone(3);
			dlgname[3][0] = '\0';

				changed_prefs.floppyslots[3].df[0] = 0;
#ifndef LIBRETRO_FSUAE
				DISK_check_change();
#endif /*LIBRETRO_FSUAE*/
				disk_eject(3);

			break;
		case FLOPPYDLG_BROWSE3:                         /* Choose a new disk B: */
			DlgDisk_BrowseDisk(dlgname[3], 1, FLOPPYDLG_DISKB);
			if (strlen(szDiskFileName[3]) > 0){

					if (currprefs.nr_floppies-1 < 3 ) {
						currprefs.nr_floppies = 3  + 1;
					}

					//check whether drive is enabled
					if (currprefs.floppyslots[3].dfxtype < 0) {
						changed_prefs.floppyslots[3 ].dfxtype = 0;
						DISK_check_change();
					}
					strcpy (changed_prefs.floppyslots[3 ].df,szDiskFileName[3]);
					DISK_check_change();
			}

			break;
		 case FLOPPYDLG_BROWSEIMG:
			DlgDisk_BrowseDir(dlgdiskdir,
			                 /*ConfigureParams.DiskImage.*/szDiskImageDirectory,
			                 floppydlg[FLOPPYDLG_IMGDIR].w);
			break;
/*
		 case FLOPPYDLG_CREATEIMG:
			newdisk = DlgNewDisk_Main();
			if (newdisk)
			{
				DlgFloppy_QueryInsert(dlgname[0], FLOPPYDLG_DISKA,
						      dlgname[1], FLOPPYDLG_DISKB,
						      newdisk);
				free(newdisk);
			}
			break;
*/
		default:
		  if (but>=FLOPPYDLG_SWAP && but<FLOPPYDLG_SWAP+FLOPPY_SWAPMAX) {
		    int i=but-FLOPPYDLG_SWAP;
		    if (i<floppyswapnum) {
		      insert_disk(floppyswap[i].x, floppyswap[i].y);
		    } else fprintf(stderr, "%s %s:%d i=%d\n", __FILE__, __FUNCTION__, __LINE__, i);
		  } /*else fprintf(stderr, "%s %s:%d but=%d\n", __FILE__, __FUNCTION__, __LINE__, but);*/
		}
                gui_poll_events();
	}
	while (but != FLOPPYDLG_EXIT && but != SDLGUI_QUIT
	        && but != SDLGUI_ERROR && !bQuitProgram);



}
