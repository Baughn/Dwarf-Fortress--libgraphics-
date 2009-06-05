#include "platform.h"
#include <string.h>
#include <math.h>
#include <iosfwd>
#include <iostream>
#include <ios>
#include <streambuf>
#include <istream>
#include <ostream>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <zlib.h>

#include "svector.h"
using std::string;

#include "endian.h"

#include "files.h"

#include "textlines.h"

#include "enabler.h"

#include "find_files.h"

#include "basics.h"

#include "g_basics.h"

#include "music_and_sound_g.h"

#include "graphics.h"

#include "init.h"

#include "interface.h"

void dwarf_help_routine();
void dwarf_end_announcements();
void dwarf_remove_screen();
void dwarf_option_screen();
void drawborder(const char *str,char style,const char *colorstr);


inline void CHECK_ERR(int err, const char* msg)
{
	if (err != Z_OK)
		{
		MessageBox(NULL, "One of the compressed files on disk has errors in it.  Restore from backup if you are able.", 0, 0);
		exit(1);
		}
}

using std::fstream;
using std::ios;

extern Uint8 symToVkey[1024];

extern interfacest gview;
extern enablerst enabler;
extern graphicst gps;
extern initst init;
#ifndef NO_FMOD
extern musicsoundst musicsound;
#endif

extern GameMode gamemode;
extern GameType gametype;

extern long movie_version;




void viewscreenst::help()
{
	dwarf_help_routine();
}

viewscreen_keybindingsst *viewscreen_keybindingsst::create(char pushtype,viewscreenst *scr)
{
	viewscreen_keybindingsst *newv=new viewscreen_keybindingsst();
	gview.addscreen(newv,pushtype,scr);

	return newv;
}

viewscreen_keybindingsst::viewscreen_keybindingsst()
{
	selected=0;
	binding=0;

	allowed_key.push_back(INTERFACEKEY_SELECT);
	allowed_key.push_back(INTERFACEKEY_DESELECT);
	allowed_key.push_back(INTERFACEKEY_SELECT_ALL);
	allowed_key.push_back(INTERFACEKEY_DESELECT_ALL);
	allowed_key.push_back(INTERFACEKEY_LEAVESCREEN);
	allowed_key.push_back(INTERFACEKEY_LEAVESCREEN_TEXT);
	allowed_key.push_back(INTERFACEKEY_LEAVESCREEN_ALL);
	allowed_key.push_back(INTERFACEKEY_CLOSE_MEGA_ANNOUNCEMENT);
	allowed_key.push_back(INTERFACEKEY_OPTIONS);
	allowed_key.push_back(INTERFACEKEY_OPTION_EXPORT);
	allowed_key.push_back(INTERFACEKEY_MENU_CONFIRM);
	allowed_key.push_back(INTERFACEKEY_SAVE_BINDINGS);
	allowed_key.push_back(INTERFACEKEY_CHANGE_BINDINGS);
	allowed_key.push_back(INTERFACEKEY_HELP);
	allowed_key.push_back(INTERFACEKEY_TOGGLE_FULLSCREEN);
	allowed_key.push_back(INTERFACEKEY_WORLD_PARAM_ADD);
	allowed_key.push_back(INTERFACEKEY_WORLD_PARAM_COPY);
	allowed_key.push_back(INTERFACEKEY_WORLD_PARAM_DELETE);
	allowed_key.push_back(INTERFACEKEY_WORLD_PARAM_TITLE);
	allowed_key.push_back(INTERFACEKEY_WORLD_PARAM_NAME_RANDOM);
	allowed_key.push_back(INTERFACEKEY_WORLD_PARAM_NAME_ENTER);
	allowed_key.push_back(INTERFACEKEY_WORLD_PARAM_SEED_RANDOM);
	allowed_key.push_back(INTERFACEKEY_WORLD_PARAM_SEED_ENTER);
	allowed_key.push_back(INTERFACEKEY_WORLD_PARAM_LOAD);
	allowed_key.push_back(INTERFACEKEY_WORLD_PARAM_SAVE);
	allowed_key.push_back(INTERFACEKEY_WORLD_PARAM_DIM_X_UP);
	allowed_key.push_back(INTERFACEKEY_WORLD_PARAM_DIM_X_DOWN);
	allowed_key.push_back(INTERFACEKEY_WORLD_PARAM_DIM_Y_UP);
	allowed_key.push_back(INTERFACEKEY_WORLD_PARAM_DIM_Y_DOWN);
	allowed_key.push_back(INTERFACEKEY_WORLD_PARAM_SET);
	allowed_key.push_back(INTERFACEKEY_WORLD_PARAM_INCREASE);
	allowed_key.push_back(INTERFACEKEY_WORLD_PARAM_DECREASE);
	allowed_key.push_back(INTERFACEKEY_WORLD_PARAM_ENTER_VALUE);
	allowed_key.push_back(INTERFACEKEY_WORLD_PARAM_NULLIFY);
	allowed_key.push_back(INTERFACEKEY_WORLD_PARAM_PRESET);
	allowed_key.push_back(INTERFACEKEY_WORLD_PARAM_REJECT_CONTINUE);
	allowed_key.push_back(INTERFACEKEY_WORLD_PARAM_REJECT_ABORT);
	allowed_key.push_back(INTERFACEKEY_WORLD_PARAM_REJECT_ALLOW_THIS);
	allowed_key.push_back(INTERFACEKEY_WORLD_PARAM_REJECT_ALLOW_ALL);
	allowed_key.push_back(INTERFACEKEY_WORLD_GEN_CONTINUE);
	allowed_key.push_back(INTERFACEKEY_WORLD_GEN_USE);
	allowed_key.push_back(INTERFACEKEY_WORLD_GEN_ABORT);
	allowed_key.push_back(INTERFACEKEY_MOVIES);
	allowed_key.push_back(INTERFACEKEY_MOVIE_RECORD);
	allowed_key.push_back(INTERFACEKEY_MOVIE_PLAY);
	allowed_key.push_back(INTERFACEKEY_MOVIE_SAVE);
	allowed_key.push_back(INTERFACEKEY_MOVIE_LOAD);
	allowed_key.push_back(INTERFACEKEY_CHANGETAB);
	allowed_key.push_back(INTERFACEKEY_STANDARDSCROLL_UP);
	allowed_key.push_back(INTERFACEKEY_STANDARDSCROLL_DOWN);
	allowed_key.push_back(INTERFACEKEY_STANDARDSCROLL_LEFT);
	allowed_key.push_back(INTERFACEKEY_STANDARDSCROLL_RIGHT);
	allowed_key.push_back(INTERFACEKEY_STANDARDSCROLL_PAGEUP);
	allowed_key.push_back(INTERFACEKEY_STANDARDSCROLL_PAGEDOWN);
	allowed_key.push_back(INTERFACEKEY_SECONDSCROLL_UP);
	allowed_key.push_back(INTERFACEKEY_SECONDSCROLL_DOWN);
	allowed_key.push_back(INTERFACEKEY_SECONDSCROLL_PAGEUP);
	allowed_key.push_back(INTERFACEKEY_SECONDSCROLL_PAGEDOWN);
	allowed_key.push_back(INTERFACEKEY_CURSOR_UP);
	allowed_key.push_back(INTERFACEKEY_CURSOR_DOWN);
	allowed_key.push_back(INTERFACEKEY_CURSOR_LEFT);
	allowed_key.push_back(INTERFACEKEY_CURSOR_RIGHT);
	allowed_key.push_back(INTERFACEKEY_CURSOR_UPLEFT);
	allowed_key.push_back(INTERFACEKEY_CURSOR_UPRIGHT);
	allowed_key.push_back(INTERFACEKEY_CURSOR_DOWNLEFT);
	allowed_key.push_back(INTERFACEKEY_CURSOR_DOWNRIGHT);
	allowed_key.push_back(INTERFACEKEY_CURSOR_UP_FAST);
	allowed_key.push_back(INTERFACEKEY_CURSOR_DOWN_FAST);
	allowed_key.push_back(INTERFACEKEY_CURSOR_LEFT_FAST);
	allowed_key.push_back(INTERFACEKEY_CURSOR_RIGHT_FAST);
	allowed_key.push_back(INTERFACEKEY_CURSOR_UPLEFT_FAST);
	allowed_key.push_back(INTERFACEKEY_CURSOR_UPRIGHT_FAST);
	allowed_key.push_back(INTERFACEKEY_CURSOR_DOWNLEFT_FAST);
	allowed_key.push_back(INTERFACEKEY_CURSOR_DOWNRIGHT_FAST);
	allowed_key.push_back(INTERFACEKEY_CURSOR_UP_Z);
	allowed_key.push_back(INTERFACEKEY_CURSOR_DOWN_Z);
	allowed_key.push_back(INTERFACEKEY_CURSOR_UP_Z_AUX);
	allowed_key.push_back(INTERFACEKEY_CURSOR_DOWN_Z_AUX);
	allowed_key.push_back(INTERFACEKEY_A_MOVE_N);
	allowed_key.push_back(INTERFACEKEY_A_MOVE_S);
	allowed_key.push_back(INTERFACEKEY_A_MOVE_E);
	allowed_key.push_back(INTERFACEKEY_A_MOVE_W);
	allowed_key.push_back(INTERFACEKEY_A_MOVE_NW);
	allowed_key.push_back(INTERFACEKEY_A_MOVE_NE);
	allowed_key.push_back(INTERFACEKEY_A_MOVE_SW);
	allowed_key.push_back(INTERFACEKEY_A_MOVE_SE);
	allowed_key.push_back(INTERFACEKEY_A_MOVE_WAIT);
	allowed_key.push_back(INTERFACEKEY_A_CARE_MOVE_N);
	allowed_key.push_back(INTERFACEKEY_A_CARE_MOVE_S);
	allowed_key.push_back(INTERFACEKEY_A_CARE_MOVE_E);
	allowed_key.push_back(INTERFACEKEY_A_CARE_MOVE_W);
	allowed_key.push_back(INTERFACEKEY_A_CARE_MOVE_NW);
	allowed_key.push_back(INTERFACEKEY_A_CARE_MOVE_NE);
	allowed_key.push_back(INTERFACEKEY_A_CARE_MOVE_SW);
	allowed_key.push_back(INTERFACEKEY_A_CARE_MOVE_SE);
	allowed_key.push_back(INTERFACEKEY_A_CARE_MOVE_UPDOWN);
	allowed_key.push_back(INTERFACEKEY_A_MOVE_N_UP);
	allowed_key.push_back(INTERFACEKEY_A_MOVE_S_UP);
	allowed_key.push_back(INTERFACEKEY_A_MOVE_E_UP);
	allowed_key.push_back(INTERFACEKEY_A_MOVE_W_UP);
	allowed_key.push_back(INTERFACEKEY_A_MOVE_NW_UP);
	allowed_key.push_back(INTERFACEKEY_A_MOVE_NE_UP);
	allowed_key.push_back(INTERFACEKEY_A_MOVE_SW_UP);
	allowed_key.push_back(INTERFACEKEY_A_MOVE_SE_UP);
	allowed_key.push_back(INTERFACEKEY_A_MOVE_UP);
	allowed_key.push_back(INTERFACEKEY_A_MOVE_N_DOWN);
	allowed_key.push_back(INTERFACEKEY_A_MOVE_S_DOWN);
	allowed_key.push_back(INTERFACEKEY_A_MOVE_E_DOWN);
	allowed_key.push_back(INTERFACEKEY_A_MOVE_W_DOWN);
	allowed_key.push_back(INTERFACEKEY_A_MOVE_NW_DOWN);
	allowed_key.push_back(INTERFACEKEY_A_MOVE_NE_DOWN);
	allowed_key.push_back(INTERFACEKEY_A_MOVE_SW_DOWN);
	allowed_key.push_back(INTERFACEKEY_A_MOVE_SE_DOWN);
	allowed_key.push_back(INTERFACEKEY_A_MOVE_DOWN);
	allowed_key.push_back(INTERFACEKEY_A_MOVE_UP_AUX);
	allowed_key.push_back(INTERFACEKEY_A_MOVE_DOWN_AUX);
	allowed_key.push_back(INTERFACEKEY_WORLDGEN_EXPORT_MAP);
	allowed_key.push_back(INTERFACEKEY_LEGENDS_EXPORT_MAP);
	allowed_key.push_back(INTERFACEKEY_LEGENDS_EXPORT_DETAILED_MAP);
	allowed_key.push_back(INTERFACEKEY_LEGENDS_TOGGLE_CIVSITE);
	allowed_key.push_back(INTERFACEKEY_LEGENDS_STRING_FILTER);
	allowed_key.push_back(INTERFACEKEY_ITEM_DESCRIPTION);
	allowed_key.push_back(INTERFACEKEY_ITEM_FORBID);
	allowed_key.push_back(INTERFACEKEY_ITEM_MELT);
	allowed_key.push_back(INTERFACEKEY_ITEM_DUMP);
	allowed_key.push_back(INTERFACEKEY_ITEM_HIDE);
	allowed_key.push_back(INTERFACEKEY_CUSTOMIZE_UNIT_NICKNAME);
	allowed_key.push_back(INTERFACEKEY_CUSTOMIZE_UNIT_PROFNAME);

	if(gamemode==GAMEMODE_DWARF)
		{
		allowed_key.push_back(INTERFACEKEY_SETUP_EMBARK);
		allowed_key.push_back(INTERFACEKEY_SETUP_NAME_FORT);
		allowed_key.push_back(INTERFACEKEY_SETUP_NAME_GROUP);
		allowed_key.push_back(INTERFACEKEY_SETUP_RECLAIM);
		allowed_key.push_back(INTERFACEKEY_SETUP_FIND);
		allowed_key.push_back(INTERFACEKEY_SETUP_NOTES);
		allowed_key.push_back(INTERFACEKEY_SETUP_NOTES_TAKE_NOTES);
		allowed_key.push_back(INTERFACEKEY_SETUP_NOTES_DELETE_NOTE);
		allowed_key.push_back(INTERFACEKEY_SETUP_NOTES_CHANGE_SYMBOL_SELECTION);
		allowed_key.push_back(INTERFACEKEY_SETUP_NOTES_ADOPT_SYMBOL);
		allowed_key.push_back(INTERFACEKEY_SETUP_LOCAL_Y_UP);
		allowed_key.push_back(INTERFACEKEY_SETUP_LOCAL_Y_DOWN);
		allowed_key.push_back(INTERFACEKEY_SETUP_LOCAL_X_UP);
		allowed_key.push_back(INTERFACEKEY_SETUP_LOCAL_X_DOWN);
		allowed_key.push_back(INTERFACEKEY_SETUP_LOCAL_Y_MUP);
		allowed_key.push_back(INTERFACEKEY_SETUP_LOCAL_Y_MDOWN);
		allowed_key.push_back(INTERFACEKEY_SETUP_LOCAL_X_MUP);
		allowed_key.push_back(INTERFACEKEY_SETUP_LOCAL_X_MDOWN);
		allowed_key.push_back(INTERFACEKEY_SETUP_BIOME_1);
		allowed_key.push_back(INTERFACEKEY_SETUP_BIOME_2);
		allowed_key.push_back(INTERFACEKEY_SETUP_BIOME_3);
		allowed_key.push_back(INTERFACEKEY_SETUP_BIOME_4);
		allowed_key.push_back(INTERFACEKEY_SETUP_BIOME_5);
		allowed_key.push_back(INTERFACEKEY_SETUP_BIOME_6);
		allowed_key.push_back(INTERFACEKEY_SETUP_BIOME_7);
		allowed_key.push_back(INTERFACEKEY_SETUP_BIOME_8);
		allowed_key.push_back(INTERFACEKEY_SETUP_BIOME_9);
		allowed_key.push_back(INTERFACEKEY_CHOOSE_NAME_RANDOM);
		allowed_key.push_back(INTERFACEKEY_CHOOSE_NAME_CLEAR);
		allowed_key.push_back(INTERFACEKEY_CHOOSE_NAME_TYPE);
		allowed_key.push_back(INTERFACEKEY_D_ONESTEP);
		allowed_key.push_back(INTERFACEKEY_D_PAUSE);
		allowed_key.push_back(INTERFACEKEY_D_DEPOT);
		allowed_key.push_back(INTERFACEKEY_D_HOT_KEYS);
		allowed_key.push_back(INTERFACEKEY_D_HOTKEY1);
		allowed_key.push_back(INTERFACEKEY_D_HOTKEY2);
		allowed_key.push_back(INTERFACEKEY_D_HOTKEY3);
		allowed_key.push_back(INTERFACEKEY_D_HOTKEY4);
		allowed_key.push_back(INTERFACEKEY_D_HOTKEY5);
		allowed_key.push_back(INTERFACEKEY_D_HOTKEY6);
		allowed_key.push_back(INTERFACEKEY_D_HOTKEY7);
		allowed_key.push_back(INTERFACEKEY_D_HOTKEY8);
		allowed_key.push_back(INTERFACEKEY_D_HOTKEY9);
		allowed_key.push_back(INTERFACEKEY_D_HOTKEY10);
		allowed_key.push_back(INTERFACEKEY_D_HOTKEY11);
		allowed_key.push_back(INTERFACEKEY_D_HOTKEY12);
		allowed_key.push_back(INTERFACEKEY_D_HOTKEY13);
		allowed_key.push_back(INTERFACEKEY_D_HOTKEY14);
		allowed_key.push_back(INTERFACEKEY_D_HOTKEY15);
		allowed_key.push_back(INTERFACEKEY_D_HOTKEY16);
		allowed_key.push_back(INTERFACEKEY_D_HOTKEY_CHANGE_NAME);
		allowed_key.push_back(INTERFACEKEY_D_HOTKEY_ZOOM);
		allowed_key.push_back(INTERFACEKEY_D_ANNOUNCE);
		allowed_key.push_back(INTERFACEKEY_D_BUILDING);
		allowed_key.push_back(INTERFACEKEY_D_CIVLIST);
		allowed_key.push_back(INTERFACEKEY_D_DESIGNATE);
		allowed_key.push_back(INTERFACEKEY_D_ARTLIST);
		allowed_key.push_back(INTERFACEKEY_D_NOBLES);
		allowed_key.push_back(INTERFACEKEY_D_ORDERS);
		allowed_key.push_back(INTERFACEKEY_D_MILITARY);
		allowed_key.push_back(INTERFACEKEY_D_ROOMS);
		allowed_key.push_back(INTERFACEKEY_D_SQUADS);
		allowed_key.push_back(INTERFACEKEY_D_STOCKPILES);
		allowed_key.push_back(INTERFACEKEY_D_CIVZONE);
		allowed_key.push_back(INTERFACEKEY_D_VIEWUNIT);
		allowed_key.push_back(INTERFACEKEY_D_JOBLIST);
		allowed_key.push_back(INTERFACEKEY_D_UNITLIST);
		allowed_key.push_back(INTERFACEKEY_D_LOOK);
		allowed_key.push_back(INTERFACEKEY_D_NOTE);
		allowed_key.push_back(INTERFACEKEY_D_NOTE_PLACE);
		allowed_key.push_back(INTERFACEKEY_D_NOTE_DELETE);
		allowed_key.push_back(INTERFACEKEY_D_NOTE_ENTER);
		allowed_key.push_back(INTERFACEKEY_D_NOTE_ADOPT_SYMBOL);
		allowed_key.push_back(INTERFACEKEY_D_NOTE_CHANGE_SELECTION);
		allowed_key.push_back(INTERFACEKEY_D_BUILDJOB);
		allowed_key.push_back(INTERFACEKEY_D_STATUS);
		allowed_key.push_back(INTERFACEKEY_D_BUILDITEM);
		allowed_key.push_back(INTERFACEKEY_D_VISUALIZE);
		allowed_key.push_back(INTERFACEKEY_D_BITEM_FORBID);
		allowed_key.push_back(INTERFACEKEY_D_BITEM_DUMP);
		allowed_key.push_back(INTERFACEKEY_D_BITEM_MELT);
		allowed_key.push_back(INTERFACEKEY_D_BITEM_HIDE);
		allowed_key.push_back(INTERFACEKEY_D_LOOK_FORBID);
		allowed_key.push_back(INTERFACEKEY_D_LOOK_DUMP);
		allowed_key.push_back(INTERFACEKEY_D_LOOK_MELT);
		allowed_key.push_back(INTERFACEKEY_D_LOOK_HIDE);
		allowed_key.push_back(INTERFACEKEY_ORDERS_GATHER_FURNITURE);
		allowed_key.push_back(INTERFACEKEY_ORDERS_GATHER_ANIMALS);
		allowed_key.push_back(INTERFACEKEY_ORDERS_GATHER_FOOD);
		allowed_key.push_back(INTERFACEKEY_ORDERS_GATHER_BODIES);
		allowed_key.push_back(INTERFACEKEY_ORDERS_REFUSE);
		allowed_key.push_back(INTERFACEKEY_ORDERS_GATHER_STONE);
		allowed_key.push_back(INTERFACEKEY_ORDERS_GATHER_WOOD);
		allowed_key.push_back(INTERFACEKEY_ORDERS_ALL_HARVEST);
		allowed_key.push_back(INTERFACEKEY_ORDERS_INDOORS);
		allowed_key.push_back(INTERFACEKEY_ORDERS_SAMEPILE);
		allowed_key.push_back(INTERFACEKEY_ORDERS_MIXFOODS);
		allowed_key.push_back(INTERFACEKEY_ORDERS_WORKSHOP);
		allowed_key.push_back(INTERFACEKEY_ORDERS_LOOM);
		allowed_key.push_back(INTERFACEKEY_ORDERS_DYED_CLOTH);
		allowed_key.push_back(INTERFACEKEY_ORDERS_COLLECT_WEB);
		allowed_key.push_back(INTERFACEKEY_ORDERS_SLAUGHTER);
		allowed_key.push_back(INTERFACEKEY_ORDERS_BUTCHER);
		allowed_key.push_back(INTERFACEKEY_ORDERS_TAN);
		allowed_key.push_back(INTERFACEKEY_ORDERS_ZONE);
		allowed_key.push_back(INTERFACEKEY_ORDERS_ZONE_DRINKING);
		allowed_key.push_back(INTERFACEKEY_ORDERS_ZONE_FISHING);
		allowed_key.push_back(INTERFACEKEY_ORDERS_EXCEPTIONS);
		allowed_key.push_back(INTERFACEKEY_ORDERS_AUTOFORBID);
		allowed_key.push_back(INTERFACEKEY_ORDERS_FORBID_PROJECTILE);
		allowed_key.push_back(INTERFACEKEY_ORDERS_FORBID_YOUR_CORPSE);
		allowed_key.push_back(INTERFACEKEY_ORDERS_FORBID_YOUR_ITEMS);
		allowed_key.push_back(INTERFACEKEY_ORDERS_FORBID_OTHER_CORPSE);
		allowed_key.push_back(INTERFACEKEY_ORDERS_FORBID_OTHER_ITEMS);
		allowed_key.push_back(INTERFACEKEY_ORDERS_REFUSE_GATHER);
		allowed_key.push_back(INTERFACEKEY_ORDERS_REFUSE_OUTSIDE);
		allowed_key.push_back(INTERFACEKEY_ORDERS_REFUSE_DUMP_CORPSE);
		allowed_key.push_back(INTERFACEKEY_ORDERS_REFUSE_DUMP_SKULL);
		allowed_key.push_back(INTERFACEKEY_ORDERS_REFUSE_DUMP_BONE);
		allowed_key.push_back(INTERFACEKEY_ORDERS_REFUSE_DUMP_SHELL);
		allowed_key.push_back(INTERFACEKEY_ORDERS_REFUSE_DUMP_SKIN);
		allowed_key.push_back(INTERFACEKEY_ORDERS_REFUSE_DUMP_OTHER);
		allowed_key.push_back(INTERFACEKEY_DESTROYBUILDING);
		allowed_key.push_back(INTERFACEKEY_SUSPENDBUILDING);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_MAKE_ASH);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_MAKE_CHARCOAL);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_MELT_OBJECT);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_GLASS_GREEN);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_GLASS_CLEAR);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_GLASS_CRYSTAL);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_COLLECT_SAND);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_GLASS_ROUGH);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_GLASS_ARMORSTAND);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_GLASS_BOX);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_GLASS_CABINET);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_GLASS_COFFIN);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_GLASS_FLOODGATE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_GLASS_HATCH_COVER);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_GLASS_GRATE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_GLASS_GOBLET);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_GLASS_TOY);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_GLASS_INSTRUMENT);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_GLASS_DOOR);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_GLASS_STATUE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_GLASS_TABLE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_GLASS_CAGE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_GLASS_CHAIR);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_GLASS_BLOCKS);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_GLASS_FLASK);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_GLASS_WEAPONRACK);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_GLASS_WINDOW);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_MAKE_PEARLASH);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_ASHERY_LYE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_ASHERY_POTASH);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_ASHERY_POTASH_DIRECT);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CARPENTER_BARREL);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CARPENTER_BLOCKS);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CARPENTER_BUCKET);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CARPENTER_TRAP_ANIMAL);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CARPENTER_CAGE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CARPENTER_ARMORSTAND);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CARPENTER_BED);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CARPENTER_CHAIR);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CARPENTER_COFFIN);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CARPENTER_DOOR);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CARPENTER_FLOODGATE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CARPENTER_HATCH_COVER);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CARPENTER_GRATE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CARPENTER_CABINET);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CARPENTER_BIN);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CARPENTER_BOX);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CARPENTER_WEAPONRACK);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CARPENTER_TABLE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_SIEGE_BALLISTA);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_SIEGE_CATAPULT);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_LEATHER_BOX);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_LEATHER_FLASK);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_LEATHER_SHIRT);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_LEATHER_CLOAK);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_LEATHER_BACKPACK);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_LEATHER_QUIVER);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_LEATHER_IMAGE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CLOTHES_MAT_PLANT);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CLOTHES_MAT_SILK);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CLOTHES_SHIRT);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CLOTHES_CLOAK);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CLOTHES_BOX);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CLOTHES_CHAIN);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CLOTHES_IMAGE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CRAFTS_MAT_STONE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CRAFTS_MAT_WOOD);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CRAFTS_DEC_BONE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CRAFTS_DEC_SHELL);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CRAFTS_TOTEM);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CRAFTS_CLOTH);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CRAFTS_SILK);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CRAFTS_SHELL);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CRAFTS_BONE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CRAFTS_LEATHER);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CRAFTS_MAT_CRAFTS);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CRAFTS_MAT_GOBLET);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CRAFTS_MAT_INSTRUMENT);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_CRAFTS_MAT_TOY);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_SMITH_WEAPON);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_SMITH_ARMOR);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_SMITH_FURNITURE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_SMITH_SIEGE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_SMITH_TRAP);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_SMITH_OTHER);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_SMITH_METAL);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_ARMORSTAND);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_BED);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_CHAIR);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_COFFIN);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_DOOR);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_FLOODGATE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_HATCH);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_GRATE_WALL);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_GRATE_FLOOR);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_BARS_VERTICAL);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_BARS_FLOOR);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_CABINET);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_BOX);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_KENNEL);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_FARMPLOT);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_WEAPONRACK);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_STATUE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_TABLE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_ROAD_DIRT);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_ROAD_PAVED);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_BRIDGE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_WELL);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_SIEGEENGINE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_FURNACE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_WINDOW_GLASS);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_WINDOW_GEM);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_SHOP);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_ANIMALTRAP);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_CHAIN);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_CAGE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_TRADEDEPOT);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_TRAP);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_MACHINE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_SUPPORT);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_ARCHERYTARGET);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_MACHINE_SCREW_PUMP);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_MACHINE_WATER_WHEEL);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_MACHINE_WINDMILL);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_MACHINE_GEAR_ASSEMBLY);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_MACHINE_AXLE_VERTICAL);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_MACHINE_AXLE_HORIZONTAL);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_SIEGEENGINE_BALLISTA);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_SIEGEENGINE_CATAPULT);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_TRAP_STONE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_TRAP_WEAPON);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_TRAP_LEVER);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_TRAP_TRIGGER);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_TRAP_CAGE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_TRAP_SPIKE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_CONSTRUCTION);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_CONSTRUCTION_WALL);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_CONSTRUCTION_FLOOR);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_CONSTRUCTION_RAMP);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_CONSTRUCTION_STAIR_UP);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_CONSTRUCTION_STAIR_DOWN);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_CONSTRUCTION_STAIR_UPDOWN);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_CONSTRUCTION_FORTIFICATION);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_LEATHER);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_QUERN);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_MILLSTONE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_LOOM);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_CLOTHES);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_BOWYER);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_CARPENTER);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_METALSMITH);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_LAVAMILL);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_JEWELER);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_MASON);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_BUTCHER);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_TANNER);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_DYER);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_CRAFTSMAN);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_SIEGE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_MECHANIC);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_STILL);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_FARMER);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_KITCHEN);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_FISHERY);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_LAB);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_ASHERY);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_FURNACE_WOOD);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_FURNACE_SMELTER);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_FURNACE_GLASS);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_FURNACE_KILN);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_FURNACE_SMELTER_LAVA);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_FURNACE_GLASS_LAVA);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUILDING_FURNACE_KILN_LAVA);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_ALCHEMIST_SOAP);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_STILL_BREW);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_STILL_EXTRACT);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_LOOM_COLLECT_SILK);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_LOOM_WEAVE_CLOTH);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_LOOM_WEAVE_SILK);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_LOOM_WEAVE_METAL);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_KITCHEN_COOK_2);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_KITCHEN_COOK_3);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_KITCHEN_COOK_4);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_KITCHEN_RENDER_FAT);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_FARMER_PROCESS);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_FARMER_PROCESS_VIAL);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_FARMER_PROCESS_BAG);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_FARMER_PROCESS_BARREL);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_FARMER_CHEESE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_FARMER_MILK);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_MILL_MILL);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_KENNEL_TRAIN_HUNT);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_KENNEL_CATCH_VERMIN);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_KENNEL_TAME_VERMIN);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_KENNEL_TAME_UNIT);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_KENNEL_TRAIN_WAR);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_FISHERY_PROCESS);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_FISHERY_EXTRACT);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_FISHERY_CATCH);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUTCHER_BUTCHER);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUTCHER_EXTRACT);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_BUTCHER_CATCH);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_TANNER_TAN);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_DYER_THREAD);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_DYER_CLOTH);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_JEWELER_FURNITURE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_JEWELER_FINISHED);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_JEWELER_AMMO);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_JEWELER_CUT);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_JEWELER_ENCRUST);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_MECHANIC_PARTS);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_MASON_ARMORSTAND);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_MASON_BLOCKS);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_MASON_CHAIR);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_MASON_COFFIN);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_MASON_DOOR);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_MASON_FLOODGATE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_MASON_HATCH_COVER);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_MASON_GRATE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_MASON_CABINET);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_MASON_BOX);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_MASON_STATUE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_MASON_TABLE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_MASON_WEAPONRACK);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_MASON_QUERN);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_MASON_MILLSTONE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_TRAP_BRIDGE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_TRAP_DOOR);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_TRAP_FLOODGATE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_TRAP_HATCH);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_TRAP_GRATE_WALL);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_TRAP_GRATE_FLOOR);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_TRAP_BARS_VERTICAL);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_TRAP_BARS_FLOOR);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_TRAP_SUPPORT);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_TRAP_CHAIN);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_TRAP_CAGE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_TRAP_SPIKE);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_TRAP_GEAR_ASSEMBLY);
		allowed_key.push_back(INTERFACEKEY_HOTKEY_TRAP_PULL_LEVER);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_ADD);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_CANCEL);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_PROMOTE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_REPEAT);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_SUSPEND);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_WORKSHOP_PROFILE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_WELL_FREE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_WELL_SIZE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_TARGET_FREE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_TARGET_SIZE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_TARGET_DOWN);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_TARGET_UP);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_TARGET_RIGHT);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_TARGET_LEFT);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_STATUE_ASSIGN);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_STATUE_FREE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_STATUE_SIZE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_CAGE_JUSTICE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_CAGE_FREE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_CAGE_SIZE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_CAGE_ASSIGN_OCC);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_CAGE_WATER);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_CAGE_ASSIGN);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_CHAIN_ASSIGN_OCC);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_CHAIN_JUSTICE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_CHAIN_ASSIGN);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_CHAIN_FREE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_CHAIN_SIZE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_SIEGE_FIRING);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_SIEGE_ORIENT);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_DOOR_INTERNAL);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_DOOR_LOCK);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_DOOR_AJAR);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_COFFIN_ASSIGN);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_COFFIN_FREE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_COFFIN_SIZE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_COFFIN_BURIAL);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_COFFIN_CIV);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_COFFIN_PET);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_CHAIR_ASSIGN);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_CHAIR_FREE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_CHAIR_SIZE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_TABLE_ASSIGN);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_TABLE_HALL);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_TABLE_FREE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_TABLE_SIZE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_BED_ASSIGN);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_BED_FREE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_BED_BARRACKS);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_BED_RENT);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_BED_SIZE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_DEPOT_BRING);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_DEPOT_TRADE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_DEPOT_REQUEST_TRADER);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_DEPOT_BROKER_ONLY);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_ANIMALTRAP_BAIT_NONE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_ANIMALTRAP_BAIT_MEAT);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_ANIMALTRAP_BAIT_FISH);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_ANIMALTRAP_BAIT_GEM);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_FARM_FALLOW);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_FARM_FERTILIZE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_FARM_SEASFERT);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_FARM_SPRING);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_FARM_SUMMER);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_FARM_AUTUMN);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_FARM_WINTER);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_STOCKPILE_MASTER);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_STOCKPILE_DELETE_CHILD);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_STOCKPILE_SETTINGS);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_STOCKPILE_BARREL_UP);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_STOCKPILE_BARREL_DOWN);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_STOCKPILE_BARREL_ZERO);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_STOCKPILE_BARREL_MAX);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_STOCKPILE_BIN_UP);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_STOCKPILE_BIN_DOWN);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_STOCKPILE_BIN_ZERO);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_STOCKPILE_BIN_MAX);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_RACK_MAT_BONE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_RACK_MAT_BRONZE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_RACK_MAT_COPPER);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_RACK_MAT_IRON);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_RACK_MAT_STEEL);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_RACK_MAT_WOOD);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_RACK_MAT_METAL);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_STAND_MAT_BONE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_STAND_MAT_LEATHER);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_STAND_MAT_COPPER);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_STAND_MAT_BRONZE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_STAND_MAT_STEEL);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_STAND_MAT_IRON);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_STAND_MAT_WOOD);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_STAND_MAT_METAL);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_RACKSTAND_ASSIGN);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_RACKSTAND_FREE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_RACKSTAND_SIZE);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_RACKSTAND_ITEM);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_RACKSTAND_MAT);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_RACKSTAND_DEFAULTS1);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_RACKSTAND_KILL1);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_RACKSTAND_DEFAULTS2);
		allowed_key.push_back(INTERFACEKEY_BUILDJOB_RACKSTAND_KILL2);
		allowed_key.push_back(INTERFACEKEY_STOCKPILE_SETTINGS_ENABLE);
		allowed_key.push_back(INTERFACEKEY_STOCKPILE_SETTINGS_DISABLE);
		allowed_key.push_back(INTERFACEKEY_STOCKPILE_SETTINGS_PERMIT_ALL);
		allowed_key.push_back(INTERFACEKEY_STOCKPILE_SETTINGS_FORBID_ALL);
		allowed_key.push_back(INTERFACEKEY_STOCKPILE_SETTINGS_PERMIT_SUB);
		allowed_key.push_back(INTERFACEKEY_STOCKPILE_SETTINGS_FORBID_SUB);
		allowed_key.push_back(INTERFACEKEY_STOCKPILE_SETTINGS_SPECIFIC1);
		allowed_key.push_back(INTERFACEKEY_STOCKPILE_SETTINGS_SPECIFIC2);
		allowed_key.push_back(INTERFACEKEY_SQUAD_LOCK);
		allowed_key.push_back(INTERFACEKEY_SQUAD_STATION);
		allowed_key.push_back(INTERFACEKEY_SQUAD_CLEAR_PATROL);
		allowed_key.push_back(INTERFACEKEY_SQUAD_PATROL);
		allowed_key.push_back(INTERFACEKEY_SQUAD_ZOOM);
		allowed_key.push_back(INTERFACEKEY_ASSIGNTRADE_VIEW);
		allowed_key.push_back(INTERFACEKEY_ASSIGNTRADE_STRING);
		allowed_key.push_back(INTERFACEKEY_ASSIGNTRADE_EXCLUDE_PROHIBITED);
		allowed_key.push_back(INTERFACEKEY_ASSIGNTRADE_PENDING);
		allowed_key.push_back(INTERFACEKEY_ASSIGNTRADE_SORT);
		allowed_key.push_back(INTERFACEKEY_NOBLELIST_REPLACE);
		allowed_key.push_back(INTERFACEKEY_NOBLELIST_SETTINGS);
		allowed_key.push_back(INTERFACEKEY_NOBLELIST_VIEW_CANDIDATE);
		allowed_key.push_back(INTERFACEKEY_TRADE_VIEW);
		allowed_key.push_back(INTERFACEKEY_TRADE_TRADE);
		allowed_key.push_back(INTERFACEKEY_TRADE_OFFER);
		allowed_key.push_back(INTERFACEKEY_TRADE_SEIZE);
		allowed_key.push_back(INTERFACEKEY_STORES_VIEW);
		allowed_key.push_back(INTERFACEKEY_STORES_ZOOM);
		allowed_key.push_back(INTERFACEKEY_STORES_FORBID);
		allowed_key.push_back(INTERFACEKEY_STORES_MELT);
		allowed_key.push_back(INTERFACEKEY_STORES_DUMP);
		allowed_key.push_back(INTERFACEKEY_STORES_HIDE);
		allowed_key.push_back(INTERFACEKEY_MILITARY_ACTIVATE);
		allowed_key.push_back(INTERFACEKEY_MILITARY_VIEW);
		allowed_key.push_back(INTERFACEKEY_MILITARY_WEAPON);
		allowed_key.push_back(INTERFACEKEY_MILITARY_ZOOM);
		allowed_key.push_back(INTERFACEKEY_SQUADINFO_FOOD);
		allowed_key.push_back(INTERFACEKEY_SQUADINFO_SLEEP);
		allowed_key.push_back(INTERFACEKEY_SQUADINFO_WATER);
		allowed_key.push_back(INTERFACEKEY_SQUADINFO_STAYCLOSE);
		allowed_key.push_back(INTERFACEKEY_SQUADINFO_ATTACKWILDS);
		allowed_key.push_back(INTERFACEKEY_SQUADINFO_STANDDOWN);
		allowed_key.push_back(INTERFACEKEY_SQUADINFO_ZOOM);
		allowed_key.push_back(INTERFACEKEY_UNITJOB_REMOVE_CRE);
		allowed_key.push_back(INTERFACEKEY_UNITJOB_ZOOM_CRE);
		allowed_key.push_back(INTERFACEKEY_UNITJOB_ZOOM_BUILD);
		allowed_key.push_back(INTERFACEKEY_UNITJOB_VIEW);
		allowed_key.push_back(INTERFACEKEY_UNITJOB_MANAGER);
		allowed_key.push_back(INTERFACEKEY_MANAGER_NEW_ORDER);
		allowed_key.push_back(INTERFACEKEY_MANAGER_REMOVE);
		allowed_key.push_back(INTERFACEKEY_MANAGER_PROMOTE);
		allowed_key.push_back(INTERFACEKEY_MANAGER_MAX);
		allowed_key.push_back(INTERFACEKEY_MANAGER_WAGES);
		allowed_key.push_back(INTERFACEKEY_PET_BUTCHER);
		allowed_key.push_back(INTERFACEKEY_KITCHEN_COOK);
		allowed_key.push_back(INTERFACEKEY_KITCHEN_BREW);
		allowed_key.push_back(INTERFACEKEY_SETUPGAME_NEW);
		allowed_key.push_back(INTERFACEKEY_SETUPGAME_VIEW);
		allowed_key.push_back(INTERFACEKEY_SETUPGAME_CUSTOMIZE_UNIT);
		allowed_key.push_back(INTERFACEKEY_SETUPGAME_SAVE_PROFILE);
		allowed_key.push_back(INTERFACEKEY_SETUPGAME_SAVE_PROFILE_ABORT);
		allowed_key.push_back(INTERFACEKEY_SETUPGAME_SAVE_PROFILE_GO);
		allowed_key.push_back(INTERFACEKEY_SETUPGAME_VIEW_PROFILE_PROBLEMS);
		allowed_key.push_back(INTERFACEKEY_CIVZONE_REMOVE);
		allowed_key.push_back(INTERFACEKEY_CIVZONE_WATER_SOURCE);
		allowed_key.push_back(INTERFACEKEY_CIVZONE_FISH);
		allowed_key.push_back(INTERFACEKEY_CIVZONE_DUMP);
		allowed_key.push_back(INTERFACEKEY_CIVZONE_POND);
		allowed_key.push_back(INTERFACEKEY_CIVZONE_SAND_COLLECT);
		allowed_key.push_back(INTERFACEKEY_CIVZONE_ACTIVE);
		allowed_key.push_back(INTERFACEKEY_CIVZONE_MEETING);
		allowed_key.push_back(INTERFACEKEY_CIVZONE_POND_OPTIONS);
		allowed_key.push_back(INTERFACEKEY_CIVZONE_POND_WATER);
		allowed_key.push_back(INTERFACEKEY_STOCKPILE_ANIMAL);
		allowed_key.push_back(INTERFACEKEY_STOCKPILE_FOOD);
		allowed_key.push_back(INTERFACEKEY_STOCKPILE_WEAPON);
		allowed_key.push_back(INTERFACEKEY_STOCKPILE_ARMOR);
		allowed_key.push_back(INTERFACEKEY_STOCKPILE_CUSTOM);
		allowed_key.push_back(INTERFACEKEY_STOCKPILE_CUSTOM_SETTINGS);
		allowed_key.push_back(INTERFACEKEY_STOCKPILE_FURNITURE);
		allowed_key.push_back(INTERFACEKEY_STOCKPILE_GRAVEYARD);
		allowed_key.push_back(INTERFACEKEY_STOCKPILE_REFUSE);
		allowed_key.push_back(INTERFACEKEY_STOCKPILE_WOOD);
		allowed_key.push_back(INTERFACEKEY_STOCKPILE_STONE);
		allowed_key.push_back(INTERFACEKEY_STOCKPILE_GEM);
		allowed_key.push_back(INTERFACEKEY_STOCKPILE_BARBLOCK);
		allowed_key.push_back(INTERFACEKEY_STOCKPILE_CLOTH);
		allowed_key.push_back(INTERFACEKEY_STOCKPILE_LEATHER);
		allowed_key.push_back(INTERFACEKEY_STOCKPILE_AMMO);
		allowed_key.push_back(INTERFACEKEY_STOCKPILE_COINS);
		allowed_key.push_back(INTERFACEKEY_STOCKPILE_FINISHED);
		allowed_key.push_back(INTERFACEKEY_STOCKPILE_NONE);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_BITEM);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_CLAIM);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_UNCLAIM);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_MELT);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_NO_MELT);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_DUMP);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_NO_DUMP);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_HIDE);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_NO_HIDE);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_TRAFFIC_HIGH);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_TRAFFIC_NORMAL);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_TRAFFIC_LOW);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_TRAFFIC_RESTRICTED);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_TRAFFIC_INCREASE_WEIGHT);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_TRAFFIC_DECREASE_WEIGHT);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_TRAFFIC_INCREASE_WEIGHT_MORE);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_TRAFFIC_DECREASE_WEIGHT_MORE);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_DIG_REMOVE_STAIRS_RAMPS);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_STAIR_UP);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_STAIR_DOWN);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_STAIR_UPDOWN);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_RAMP);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_CHANNEL);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_CHOP);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_PLANTS);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_SMOOTH);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_ENGRAVE);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_FORTIFY);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_TOGGLE_ENGRAVING);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_UNDO);
		allowed_key.push_back(INTERFACEKEY_DESIGNATE_REMOVE_CONSTRUCTION);
		allowed_key.push_back(INTERFACEKEY_BUILDING_DIM_Y_UP);
		allowed_key.push_back(INTERFACEKEY_BUILDING_DIM_Y_DOWN);
		allowed_key.push_back(INTERFACEKEY_BUILDING_DIM_X_UP);
		allowed_key.push_back(INTERFACEKEY_BUILDING_DIM_X_DOWN);
		allowed_key.push_back(INTERFACEKEY_BUILDING_ORIENT_UP);
		allowed_key.push_back(INTERFACEKEY_BUILDING_ORIENT_LEFT);
		allowed_key.push_back(INTERFACEKEY_BUILDING_ORIENT_RIGHT);
		allowed_key.push_back(INTERFACEKEY_BUILDING_ORIENT_DOWN);
		allowed_key.push_back(INTERFACEKEY_BUILDING_ORIENT_NONE);
		allowed_key.push_back(INTERFACEKEY_BUILDING_VIEW_ITEM);
		allowed_key.push_back(INTERFACEKEY_BUILDING_ADVANCE_STAGE);
		allowed_key.push_back(INTERFACEKEY_BUILDING_EXPAND_CONTRACT);
		allowed_key.push_back(INTERFACEKEY_BUILDING_TRIGGER_ENABLE_WATER);
		allowed_key.push_back(INTERFACEKEY_BUILDING_TRIGGER_MIN_WATER_UP);
		allowed_key.push_back(INTERFACEKEY_BUILDING_TRIGGER_MIN_WATER_DOWN);
		allowed_key.push_back(INTERFACEKEY_BUILDING_TRIGGER_MAX_WATER_UP);
		allowed_key.push_back(INTERFACEKEY_BUILDING_TRIGGER_MAX_WATER_DOWN);
		allowed_key.push_back(INTERFACEKEY_BUILDING_TRIGGER_ENABLE_MAGMA);
		allowed_key.push_back(INTERFACEKEY_BUILDING_TRIGGER_MIN_MAGMA_UP);
		allowed_key.push_back(INTERFACEKEY_BUILDING_TRIGGER_MIN_MAGMA_DOWN);
		allowed_key.push_back(INTERFACEKEY_BUILDING_TRIGGER_MAX_MAGMA_UP);
		allowed_key.push_back(INTERFACEKEY_BUILDING_TRIGGER_MAX_MAGMA_DOWN);
		allowed_key.push_back(INTERFACEKEY_BUILDING_TRIGGER_ENABLE_CREATURE);
		allowed_key.push_back(INTERFACEKEY_BUILDING_TRIGGER_ENABLE_LOCALS);
		allowed_key.push_back(INTERFACEKEY_BUILDING_TRIGGER_RESETS);
		allowed_key.push_back(INTERFACEKEY_BUILDING_TRIGGER_MIN_SIZE_UP);
		allowed_key.push_back(INTERFACEKEY_BUILDING_TRIGGER_MIN_SIZE_DOWN);
		allowed_key.push_back(INTERFACEKEY_BUILDING_TRIGGER_MAX_SIZE_UP);
		allowed_key.push_back(INTERFACEKEY_BUILDING_TRIGGER_MAX_SIZE_DOWN);
		allowed_key.push_back(INTERFACEKEY_UNITVIEW_GEN);
		allowed_key.push_back(INTERFACEKEY_UNITVIEW_INV);
		allowed_key.push_back(INTERFACEKEY_UNITVIEW_PRF);
		allowed_key.push_back(INTERFACEKEY_UNITVIEW_WND);
		allowed_key.push_back(INTERFACEKEY_UNITVIEW_NEXT);
		allowed_key.push_back(INTERFACEKEY_UNITVIEW_SLAUGHTER);
		allowed_key.push_back(INTERFACEKEY_UNITVIEW_PRF_PROF);
		allowed_key.push_back(INTERFACEKEY_UNITVIEW_PRF_PET);
		allowed_key.push_back(INTERFACEKEY_UNITVIEW_PRF_SOLDIER);
		allowed_key.push_back(INTERFACEKEY_UNITVIEW_PRF_CASTLE);
		allowed_key.push_back(INTERFACEKEY_UNITVIEW_PRF_ROYAL);
		allowed_key.push_back(INTERFACEKEY_UNITVIEW_PRF_TOGGLESQUAD);
		allowed_key.push_back(INTERFACEKEY_UNITVIEW_PRF_PET_HUNT);
		allowed_key.push_back(INTERFACEKEY_UNITVIEW_PRF_PET_WAR);
		allowed_key.push_back(INTERFACEKEY_UNITVIEW_PRF_SOLDIER_UNARMED);
		allowed_key.push_back(INTERFACEKEY_UNITVIEW_PRF_SOLDIER_AXE);
		allowed_key.push_back(INTERFACEKEY_UNITVIEW_PRF_SOLDIER_CROSSBOW);
		allowed_key.push_back(INTERFACEKEY_UNITVIEW_PRF_SOLDIER_HAMMER);
		allowed_key.push_back(INTERFACEKEY_UNITVIEW_PRF_SOLDIER_MACE);
		allowed_key.push_back(INTERFACEKEY_UNITVIEW_PRF_SOLDIER_SPEAR);
		allowed_key.push_back(INTERFACEKEY_UNITVIEW_PRF_SOLDIER_SWORD);
		allowed_key.push_back(INTERFACEKEY_UNITVIEW_PRF_SOLDIER_WEAPONNUM);
		allowed_key.push_back(INTERFACEKEY_UNITVIEW_PRF_SOLDIER_ARMOR);
		allowed_key.push_back(INTERFACEKEY_UNITVIEW_PRF_SOLDIER_SHIELD);
		allowed_key.push_back(INTERFACEKEY_UNITVIEW_PRF_VIEW);
		allowed_key.push_back(INTERFACEKEY_UNITVIEW_CUSTOMIZE);
		allowed_key.push_back(INTERFACEKEY_UNITVIEW_RELATIONSHIPS);
		allowed_key.push_back(INTERFACEKEY_UNITVIEW_RELATIONSHIPS_ZOOM);
		allowed_key.push_back(INTERFACEKEY_UNITVIEW_RELATIONSHIPS_VIEW);
		allowed_key.push_back(INTERFACEKEY_UNITVIEW_KILLS);
		allowed_key.push_back(INTERFACEKEY_BUILDINGLIST_ZOOM_T);
		allowed_key.push_back(INTERFACEKEY_BUILDINGLIST_ZOOM_Q);
		}
	if(gamemode==GAMEMODE_ADVENTURE)
		{
		allowed_key.push_back(INTERFACEKEY_A_OPTION1);
		allowed_key.push_back(INTERFACEKEY_A_OPTION2);
		allowed_key.push_back(INTERFACEKEY_A_OPTION3);
		allowed_key.push_back(INTERFACEKEY_A_OPTION4);
		allowed_key.push_back(INTERFACEKEY_A_OPTION5);
		allowed_key.push_back(INTERFACEKEY_A_OPTION6);
		allowed_key.push_back(INTERFACEKEY_A_OPTION7);
		allowed_key.push_back(INTERFACEKEY_A_OPTION8);
		allowed_key.push_back(INTERFACEKEY_A_OPTION9);
		allowed_key.push_back(INTERFACEKEY_A_OPTION10);
		allowed_key.push_back(INTERFACEKEY_A_OPTION11);
		allowed_key.push_back(INTERFACEKEY_A_OPTION12);
		allowed_key.push_back(INTERFACEKEY_A_OPTION13);
		allowed_key.push_back(INTERFACEKEY_A_OPTION14);
		allowed_key.push_back(INTERFACEKEY_A_OPTION15);
		allowed_key.push_back(INTERFACEKEY_A_OPTION16);
		allowed_key.push_back(INTERFACEKEY_A_OPTION17);
		allowed_key.push_back(INTERFACEKEY_A_OPTION18);
		allowed_key.push_back(INTERFACEKEY_A_OPTION19);
		allowed_key.push_back(INTERFACEKEY_A_OPTION20);
		allowed_key.push_back(INTERFACEKEY_A_ENTER_NAME);
		allowed_key.push_back(INTERFACEKEY_A_CUST_NAME);
		allowed_key.push_back(INTERFACEKEY_A_RANDOM_NAME);
		allowed_key.push_back(INTERFACEKEY_A_CHANGE_GENDER);
		allowed_key.push_back(INTERFACEKEY_A_STATUS);
		allowed_key.push_back(INTERFACEKEY_A_SLEEP);
		allowed_key.push_back(INTERFACEKEY_A_WAIT);
		allowed_key.push_back(INTERFACEKEY_A_ATTACK);
		allowed_key.push_back(INTERFACEKEY_A_LOOK);
		allowed_key.push_back(INTERFACEKEY_A_SEARCH);
		allowed_key.push_back(INTERFACEKEY_A_TALK);
		allowed_key.push_back(INTERFACEKEY_A_INTERACT);
		allowed_key.push_back(INTERFACEKEY_A_INV_LOOK);
		allowed_key.push_back(INTERFACEKEY_A_INV_REMOVE);
		allowed_key.push_back(INTERFACEKEY_A_INV_WEAR);
		allowed_key.push_back(INTERFACEKEY_A_INV_EATDRINK);
		allowed_key.push_back(INTERFACEKEY_A_INV_PUTIN);
		allowed_key.push_back(INTERFACEKEY_A_INV_DROP);
		allowed_key.push_back(INTERFACEKEY_A_GROUND);
		allowed_key.push_back(INTERFACEKEY_A_THROW);
		allowed_key.push_back(INTERFACEKEY_A_SHOOT);
		allowed_key.push_back(INTERFACEKEY_A_ANNOUNCEMENTS);
		allowed_key.push_back(INTERFACEKEY_A_COMBAT);
		allowed_key.push_back(INTERFACEKEY_A_COMBAT_ATTACK);
		allowed_key.push_back(INTERFACEKEY_A_COMBAT_DODGE);
		allowed_key.push_back(INTERFACEKEY_A_COMBAT_CHARGEDEF);
		allowed_key.push_back(INTERFACEKEY_A_MOVEMENT);
		allowed_key.push_back(INTERFACEKEY_A_MOVEMENT_SWIM);
		allowed_key.push_back(INTERFACEKEY_A_SNEAK);
		allowed_key.push_back(INTERFACEKEY_A_CENTER);
		allowed_key.push_back(INTERFACEKEY_A_BUILDING);
		allowed_key.push_back(INTERFACEKEY_A_TRAVEL);
		allowed_key.push_back(INTERFACEKEY_A_STATUS_WRESTLE);
		allowed_key.push_back(INTERFACEKEY_A_STATUS_CUSTOMIZE);
		allowed_key.push_back(INTERFACEKEY_A_STATUS_KILLS);
		allowed_key.push_back(INTERFACEKEY_A_DATE);
		allowed_key.push_back(INTERFACEKEY_A_WEATHER);
		allowed_key.push_back(INTERFACEKEY_A_TEMPERATURE);
		allowed_key.push_back(INTERFACEKEY_A_STANCE);
		allowed_key.push_back(INTERFACEKEY_A_END_TRAVEL);
		allowed_key.push_back(INTERFACEKEY_A_TRAVEL_CLOUDS);
		allowed_key.push_back(INTERFACEKEY_A_LOG);
		allowed_key.push_back(INTERFACEKEY_A_TRAVEL_LOG);
		allowed_key.push_back(INTERFACEKEY_A_LOG_TASKS);
		allowed_key.push_back(INTERFACEKEY_A_LOG_ENTITIES);
		allowed_key.push_back(INTERFACEKEY_A_LOG_SITES);
		allowed_key.push_back(INTERFACEKEY_A_LOG_SUBREGIONS);
		allowed_key.push_back(INTERFACEKEY_A_LOG_ZOOM_CURRENT_LOCATION);
		allowed_key.push_back(INTERFACEKEY_A_LOG_ZOOM_SELECTED);
		allowed_key.push_back(INTERFACEKEY_A_LOG_LINE);
		allowed_key.push_back(INTERFACEKEY_A_LOG_MAP);
		allowed_key.push_back(INTERFACEKEY_A_BARTER_VIEW);
		allowed_key.push_back(INTERFACEKEY_A_BARTER_CURRENCY_1);
		allowed_key.push_back(INTERFACEKEY_A_BARTER_CURRENCY_2);
		allowed_key.push_back(INTERFACEKEY_A_BARTER_CURRENCY_3);
		allowed_key.push_back(INTERFACEKEY_A_BARTER_CURRENCY_4);
		allowed_key.push_back(INTERFACEKEY_A_BARTER_CURRENCY_5);
		allowed_key.push_back(INTERFACEKEY_A_BARTER_CURRENCY_6);
		allowed_key.push_back(INTERFACEKEY_A_BARTER_CURRENCY_7);
		allowed_key.push_back(INTERFACEKEY_A_BARTER_CURRENCY_8);
		allowed_key.push_back(INTERFACEKEY_A_BARTER_CURRENCY_9);
		allowed_key.push_back(INTERFACEKEY_A_BARTER_TRADE);
		}

	allowed_key.push_back(INTERFACEKEY_STRING_A000);
	allowed_key.push_back(INTERFACEKEY_STRING_A001);
	allowed_key.push_back(INTERFACEKEY_STRING_A002);
	allowed_key.push_back(INTERFACEKEY_STRING_A003);
	allowed_key.push_back(INTERFACEKEY_STRING_A004);
	allowed_key.push_back(INTERFACEKEY_STRING_A005);
	allowed_key.push_back(INTERFACEKEY_STRING_A006);
	allowed_key.push_back(INTERFACEKEY_STRING_A007);
	allowed_key.push_back(INTERFACEKEY_STRING_A008);
	allowed_key.push_back(INTERFACEKEY_STRING_A009);
	allowed_key.push_back(INTERFACEKEY_STRING_A010);
	allowed_key.push_back(INTERFACEKEY_STRING_A011);
	allowed_key.push_back(INTERFACEKEY_STRING_A012);
	allowed_key.push_back(INTERFACEKEY_STRING_A013);
	allowed_key.push_back(INTERFACEKEY_STRING_A014);
	allowed_key.push_back(INTERFACEKEY_STRING_A015);
	allowed_key.push_back(INTERFACEKEY_STRING_A016);
	allowed_key.push_back(INTERFACEKEY_STRING_A017);
	allowed_key.push_back(INTERFACEKEY_STRING_A018);
	allowed_key.push_back(INTERFACEKEY_STRING_A019);
	allowed_key.push_back(INTERFACEKEY_STRING_A020);
	allowed_key.push_back(INTERFACEKEY_STRING_A021);
	allowed_key.push_back(INTERFACEKEY_STRING_A022);
	allowed_key.push_back(INTERFACEKEY_STRING_A023);
	allowed_key.push_back(INTERFACEKEY_STRING_A024);
	allowed_key.push_back(INTERFACEKEY_STRING_A025);
	allowed_key.push_back(INTERFACEKEY_STRING_A026);
	allowed_key.push_back(INTERFACEKEY_STRING_A027);
	allowed_key.push_back(INTERFACEKEY_STRING_A028);
	allowed_key.push_back(INTERFACEKEY_STRING_A029);
	allowed_key.push_back(INTERFACEKEY_STRING_A030);
	allowed_key.push_back(INTERFACEKEY_STRING_A031);
	allowed_key.push_back(INTERFACEKEY_STRING_A032);
	allowed_key.push_back(INTERFACEKEY_STRING_A033);
	allowed_key.push_back(INTERFACEKEY_STRING_A034);
	allowed_key.push_back(INTERFACEKEY_STRING_A035);
	allowed_key.push_back(INTERFACEKEY_STRING_A036);
	allowed_key.push_back(INTERFACEKEY_STRING_A037);
	allowed_key.push_back(INTERFACEKEY_STRING_A038);
	allowed_key.push_back(INTERFACEKEY_STRING_A039);
	allowed_key.push_back(INTERFACEKEY_STRING_A040);
	allowed_key.push_back(INTERFACEKEY_STRING_A041);
	allowed_key.push_back(INTERFACEKEY_STRING_A042);
	allowed_key.push_back(INTERFACEKEY_STRING_A043);
	allowed_key.push_back(INTERFACEKEY_STRING_A044);
	allowed_key.push_back(INTERFACEKEY_STRING_A045);
	allowed_key.push_back(INTERFACEKEY_STRING_A046);
	allowed_key.push_back(INTERFACEKEY_STRING_A047);
	allowed_key.push_back(INTERFACEKEY_STRING_A048);
	allowed_key.push_back(INTERFACEKEY_STRING_A049);
	allowed_key.push_back(INTERFACEKEY_STRING_A050);
	allowed_key.push_back(INTERFACEKEY_STRING_A051);
	allowed_key.push_back(INTERFACEKEY_STRING_A052);
	allowed_key.push_back(INTERFACEKEY_STRING_A053);
	allowed_key.push_back(INTERFACEKEY_STRING_A054);
	allowed_key.push_back(INTERFACEKEY_STRING_A055);
	allowed_key.push_back(INTERFACEKEY_STRING_A056);
	allowed_key.push_back(INTERFACEKEY_STRING_A057);
	allowed_key.push_back(INTERFACEKEY_STRING_A058);
	allowed_key.push_back(INTERFACEKEY_STRING_A059);
	allowed_key.push_back(INTERFACEKEY_STRING_A060);
	allowed_key.push_back(INTERFACEKEY_STRING_A061);
	allowed_key.push_back(INTERFACEKEY_STRING_A062);
	allowed_key.push_back(INTERFACEKEY_STRING_A063);
	allowed_key.push_back(INTERFACEKEY_STRING_A064);
	allowed_key.push_back(INTERFACEKEY_STRING_A065);
	allowed_key.push_back(INTERFACEKEY_STRING_A066);
	allowed_key.push_back(INTERFACEKEY_STRING_A067);
	allowed_key.push_back(INTERFACEKEY_STRING_A068);
	allowed_key.push_back(INTERFACEKEY_STRING_A069);
	allowed_key.push_back(INTERFACEKEY_STRING_A070);
	allowed_key.push_back(INTERFACEKEY_STRING_A071);
	allowed_key.push_back(INTERFACEKEY_STRING_A072);
	allowed_key.push_back(INTERFACEKEY_STRING_A073);
	allowed_key.push_back(INTERFACEKEY_STRING_A074);
	allowed_key.push_back(INTERFACEKEY_STRING_A075);
	allowed_key.push_back(INTERFACEKEY_STRING_A076);
	allowed_key.push_back(INTERFACEKEY_STRING_A077);
	allowed_key.push_back(INTERFACEKEY_STRING_A078);
	allowed_key.push_back(INTERFACEKEY_STRING_A079);
	allowed_key.push_back(INTERFACEKEY_STRING_A080);
	allowed_key.push_back(INTERFACEKEY_STRING_A081);
	allowed_key.push_back(INTERFACEKEY_STRING_A082);
	allowed_key.push_back(INTERFACEKEY_STRING_A083);
	allowed_key.push_back(INTERFACEKEY_STRING_A084);
	allowed_key.push_back(INTERFACEKEY_STRING_A085);
	allowed_key.push_back(INTERFACEKEY_STRING_A086);
	allowed_key.push_back(INTERFACEKEY_STRING_A087);
	allowed_key.push_back(INTERFACEKEY_STRING_A088);
	allowed_key.push_back(INTERFACEKEY_STRING_A089);
	allowed_key.push_back(INTERFACEKEY_STRING_A090);
	allowed_key.push_back(INTERFACEKEY_STRING_A091);
	allowed_key.push_back(INTERFACEKEY_STRING_A092);
	allowed_key.push_back(INTERFACEKEY_STRING_A093);
	allowed_key.push_back(INTERFACEKEY_STRING_A094);
	allowed_key.push_back(INTERFACEKEY_STRING_A095);
	allowed_key.push_back(INTERFACEKEY_STRING_A096);
	allowed_key.push_back(INTERFACEKEY_STRING_A097);
	allowed_key.push_back(INTERFACEKEY_STRING_A098);
	allowed_key.push_back(INTERFACEKEY_STRING_A099);
	allowed_key.push_back(INTERFACEKEY_STRING_A100);
	allowed_key.push_back(INTERFACEKEY_STRING_A101);
	allowed_key.push_back(INTERFACEKEY_STRING_A102);
	allowed_key.push_back(INTERFACEKEY_STRING_A103);
	allowed_key.push_back(INTERFACEKEY_STRING_A104);
	allowed_key.push_back(INTERFACEKEY_STRING_A105);
	allowed_key.push_back(INTERFACEKEY_STRING_A106);
	allowed_key.push_back(INTERFACEKEY_STRING_A107);
	allowed_key.push_back(INTERFACEKEY_STRING_A108);
	allowed_key.push_back(INTERFACEKEY_STRING_A109);
	allowed_key.push_back(INTERFACEKEY_STRING_A110);
	allowed_key.push_back(INTERFACEKEY_STRING_A111);
	allowed_key.push_back(INTERFACEKEY_STRING_A112);
	allowed_key.push_back(INTERFACEKEY_STRING_A113);
	allowed_key.push_back(INTERFACEKEY_STRING_A114);
	allowed_key.push_back(INTERFACEKEY_STRING_A115);
	allowed_key.push_back(INTERFACEKEY_STRING_A116);
	allowed_key.push_back(INTERFACEKEY_STRING_A117);
	allowed_key.push_back(INTERFACEKEY_STRING_A118);
	allowed_key.push_back(INTERFACEKEY_STRING_A119);
	allowed_key.push_back(INTERFACEKEY_STRING_A120);
	allowed_key.push_back(INTERFACEKEY_STRING_A121);
	allowed_key.push_back(INTERFACEKEY_STRING_A122);
	allowed_key.push_back(INTERFACEKEY_STRING_A123);
	allowed_key.push_back(INTERFACEKEY_STRING_A124);
	allowed_key.push_back(INTERFACEKEY_STRING_A125);
	allowed_key.push_back(INTERFACEKEY_STRING_A126);
	allowed_key.push_back(INTERFACEKEY_STRING_A127);
	allowed_key.push_back(INTERFACEKEY_STRING_A128);
	allowed_key.push_back(INTERFACEKEY_STRING_A129);
	allowed_key.push_back(INTERFACEKEY_STRING_A130);
	allowed_key.push_back(INTERFACEKEY_STRING_A131);
	allowed_key.push_back(INTERFACEKEY_STRING_A132);
	allowed_key.push_back(INTERFACEKEY_STRING_A133);
	allowed_key.push_back(INTERFACEKEY_STRING_A134);
	allowed_key.push_back(INTERFACEKEY_STRING_A135);
	allowed_key.push_back(INTERFACEKEY_STRING_A136);
	allowed_key.push_back(INTERFACEKEY_STRING_A137);
	allowed_key.push_back(INTERFACEKEY_STRING_A138);
	allowed_key.push_back(INTERFACEKEY_STRING_A139);
	allowed_key.push_back(INTERFACEKEY_STRING_A140);
	allowed_key.push_back(INTERFACEKEY_STRING_A141);
	allowed_key.push_back(INTERFACEKEY_STRING_A142);
	allowed_key.push_back(INTERFACEKEY_STRING_A143);
	allowed_key.push_back(INTERFACEKEY_STRING_A144);
	allowed_key.push_back(INTERFACEKEY_STRING_A145);
	allowed_key.push_back(INTERFACEKEY_STRING_A146);
	allowed_key.push_back(INTERFACEKEY_STRING_A147);
	allowed_key.push_back(INTERFACEKEY_STRING_A148);
	allowed_key.push_back(INTERFACEKEY_STRING_A149);
	allowed_key.push_back(INTERFACEKEY_STRING_A150);
	allowed_key.push_back(INTERFACEKEY_STRING_A151);
	allowed_key.push_back(INTERFACEKEY_STRING_A152);
	allowed_key.push_back(INTERFACEKEY_STRING_A153);
	allowed_key.push_back(INTERFACEKEY_STRING_A154);
	allowed_key.push_back(INTERFACEKEY_STRING_A155);
	allowed_key.push_back(INTERFACEKEY_STRING_A156);
	allowed_key.push_back(INTERFACEKEY_STRING_A157);
	allowed_key.push_back(INTERFACEKEY_STRING_A158);
	allowed_key.push_back(INTERFACEKEY_STRING_A159);
	allowed_key.push_back(INTERFACEKEY_STRING_A160);
	allowed_key.push_back(INTERFACEKEY_STRING_A161);
	allowed_key.push_back(INTERFACEKEY_STRING_A162);
	allowed_key.push_back(INTERFACEKEY_STRING_A163);
	allowed_key.push_back(INTERFACEKEY_STRING_A164);
	allowed_key.push_back(INTERFACEKEY_STRING_A165);
	allowed_key.push_back(INTERFACEKEY_STRING_A166);
	allowed_key.push_back(INTERFACEKEY_STRING_A167);
	allowed_key.push_back(INTERFACEKEY_STRING_A168);
	allowed_key.push_back(INTERFACEKEY_STRING_A169);
	allowed_key.push_back(INTERFACEKEY_STRING_A170);
	allowed_key.push_back(INTERFACEKEY_STRING_A171);
	allowed_key.push_back(INTERFACEKEY_STRING_A172);
	allowed_key.push_back(INTERFACEKEY_STRING_A173);
	allowed_key.push_back(INTERFACEKEY_STRING_A174);
	allowed_key.push_back(INTERFACEKEY_STRING_A175);
	allowed_key.push_back(INTERFACEKEY_STRING_A176);
	allowed_key.push_back(INTERFACEKEY_STRING_A177);
	allowed_key.push_back(INTERFACEKEY_STRING_A178);
	allowed_key.push_back(INTERFACEKEY_STRING_A179);
	allowed_key.push_back(INTERFACEKEY_STRING_A180);
	allowed_key.push_back(INTERFACEKEY_STRING_A181);
	allowed_key.push_back(INTERFACEKEY_STRING_A182);
	allowed_key.push_back(INTERFACEKEY_STRING_A183);
	allowed_key.push_back(INTERFACEKEY_STRING_A184);
	allowed_key.push_back(INTERFACEKEY_STRING_A185);
	allowed_key.push_back(INTERFACEKEY_STRING_A186);
	allowed_key.push_back(INTERFACEKEY_STRING_A187);
	allowed_key.push_back(INTERFACEKEY_STRING_A188);
	allowed_key.push_back(INTERFACEKEY_STRING_A189);
	allowed_key.push_back(INTERFACEKEY_STRING_A190);
	allowed_key.push_back(INTERFACEKEY_STRING_A191);
	allowed_key.push_back(INTERFACEKEY_STRING_A192);
	allowed_key.push_back(INTERFACEKEY_STRING_A193);
	allowed_key.push_back(INTERFACEKEY_STRING_A194);
	allowed_key.push_back(INTERFACEKEY_STRING_A195);
	allowed_key.push_back(INTERFACEKEY_STRING_A196);
	allowed_key.push_back(INTERFACEKEY_STRING_A197);
	allowed_key.push_back(INTERFACEKEY_STRING_A198);
	allowed_key.push_back(INTERFACEKEY_STRING_A199);
	allowed_key.push_back(INTERFACEKEY_STRING_A200);
	allowed_key.push_back(INTERFACEKEY_STRING_A201);
	allowed_key.push_back(INTERFACEKEY_STRING_A202);
	allowed_key.push_back(INTERFACEKEY_STRING_A203);
	allowed_key.push_back(INTERFACEKEY_STRING_A204);
	allowed_key.push_back(INTERFACEKEY_STRING_A205);
	allowed_key.push_back(INTERFACEKEY_STRING_A206);
	allowed_key.push_back(INTERFACEKEY_STRING_A207);
	allowed_key.push_back(INTERFACEKEY_STRING_A208);
	allowed_key.push_back(INTERFACEKEY_STRING_A209);
	allowed_key.push_back(INTERFACEKEY_STRING_A210);
	allowed_key.push_back(INTERFACEKEY_STRING_A211);
	allowed_key.push_back(INTERFACEKEY_STRING_A212);
	allowed_key.push_back(INTERFACEKEY_STRING_A213);
	allowed_key.push_back(INTERFACEKEY_STRING_A214);
	allowed_key.push_back(INTERFACEKEY_STRING_A215);
	allowed_key.push_back(INTERFACEKEY_STRING_A216);
	allowed_key.push_back(INTERFACEKEY_STRING_A217);
	allowed_key.push_back(INTERFACEKEY_STRING_A218);
	allowed_key.push_back(INTERFACEKEY_STRING_A219);
	allowed_key.push_back(INTERFACEKEY_STRING_A220);
	allowed_key.push_back(INTERFACEKEY_STRING_A221);
	allowed_key.push_back(INTERFACEKEY_STRING_A222);
	allowed_key.push_back(INTERFACEKEY_STRING_A223);
	allowed_key.push_back(INTERFACEKEY_STRING_A224);
	allowed_key.push_back(INTERFACEKEY_STRING_A225);
	allowed_key.push_back(INTERFACEKEY_STRING_A226);
	allowed_key.push_back(INTERFACEKEY_STRING_A227);
	allowed_key.push_back(INTERFACEKEY_STRING_A228);
	allowed_key.push_back(INTERFACEKEY_STRING_A229);
	allowed_key.push_back(INTERFACEKEY_STRING_A230);
	allowed_key.push_back(INTERFACEKEY_STRING_A231);
	allowed_key.push_back(INTERFACEKEY_STRING_A232);
	allowed_key.push_back(INTERFACEKEY_STRING_A233);
	allowed_key.push_back(INTERFACEKEY_STRING_A234);
	allowed_key.push_back(INTERFACEKEY_STRING_A235);
	allowed_key.push_back(INTERFACEKEY_STRING_A236);
	allowed_key.push_back(INTERFACEKEY_STRING_A237);
	allowed_key.push_back(INTERFACEKEY_STRING_A238);
	allowed_key.push_back(INTERFACEKEY_STRING_A239);
	allowed_key.push_back(INTERFACEKEY_STRING_A240);
	allowed_key.push_back(INTERFACEKEY_STRING_A241);
	allowed_key.push_back(INTERFACEKEY_STRING_A242);
	allowed_key.push_back(INTERFACEKEY_STRING_A243);
	allowed_key.push_back(INTERFACEKEY_STRING_A244);
	allowed_key.push_back(INTERFACEKEY_STRING_A245);
	allowed_key.push_back(INTERFACEKEY_STRING_A246);
	allowed_key.push_back(INTERFACEKEY_STRING_A247);
	allowed_key.push_back(INTERFACEKEY_STRING_A248);
	allowed_key.push_back(INTERFACEKEY_STRING_A249);
	allowed_key.push_back(INTERFACEKEY_STRING_A250);
	allowed_key.push_back(INTERFACEKEY_STRING_A251);
	allowed_key.push_back(INTERFACEKEY_STRING_A252);
	allowed_key.push_back(INTERFACEKEY_STRING_A253);
	allowed_key.push_back(INTERFACEKEY_STRING_A254);
	allowed_key.push_back(INTERFACEKEY_STRING_A255);
}

void merge_binding(unsigned char &c)
{
	switch(c)
		{
		case VK_UP:c='8';break;
		case VK_DOWN:c='2';break;
		case VK_LEFT:c='4';break;
		case VK_RIGHT:c='6';break;
		case VK_CLEAR:c='5';break;
		case VK_PRIOR:c='9';break;
		case VK_NEXT:c='3';break;
		case VK_END:c='1';break;
		case VK_HOME:c='7';break;
		case VK_NUMPAD0:c='0';break;
		case VK_NUMPAD1:c='1';break;
		case VK_NUMPAD2:c='2';break;
		case VK_NUMPAD3:c='3';break;
		case VK_NUMPAD4:c='4';break;
		case VK_NUMPAD5:c='5';break;
		case VK_NUMPAD6:c='6';break;
		case VK_NUMPAD7:c='7';break;
		case VK_NUMPAD8:c='8';break;
		case VK_NUMPAD9:c='9';break;
		case VK_SEPARATOR:c=VK_RETURN;break;
		}
}

void viewscreen_keybindingsst::view()
{
	enabler.flag&=~ENABLERFLAG_MAXFPS;
	enabler.qprate=init.display.dwarf_frame_rate;

#ifndef NO_FMOD
	if(gamemode==GAMEMODE_DWARF)musicsound.startbackgroundmusic(SONG_GAME);
#endif

	drawborder("  Dwarf Fortress Options: Key Bindings  ");

	string ikey;
	long scroll=(selected/21)*21;
	long i;
	for(i=scroll;i<allowed_key.size()&&i<scroll+21;i++)
		{
		if(i==selected&&binding)gps.changecolor(3,0,1);
		else if(i==selected)gps.changecolor(7,0,1);
		else gps.changecolor(7,0,0);

		gps.locate(i-scroll+2,2);
		gps.addst(gview.interfacekey[allowed_key[i]].name);

		gps.locate(i-scroll+2,40);
		getstring_interfacechar(ikey,gview.interfacekey[allowed_key[i]]);
		gps.addst(ikey);
		}

	gps.changecolor(7,0,1);
	gps.locate(2,55);
	gview.print_interface_token(INTERFACEKEY_SAVE_BINDINGS);
	gps.addst(" - Export to txt");

	gps.changecolor(7,0,1);
	gps.locate(4,55);
	gps.addst("Press ");
	gview.print_interface_token(INTERFACEKEY_CHANGE_BINDINGS);
	gps.addst(" and a key");
	gps.locate(5,55);
	gps.addst("to change a key binding.");

	gps.renewscreen();

	if(binding&&gview.c!=0)
		{
		gview.interfacekey[allowed_key[selected]].binding=gview.c;
		if(gview.c>='A'&&gview.c<='Z'&&gview.caps)gview.interfacekey[allowed_key[selected]].shift=(!gview.shift);
		else gview.interfacekey[allowed_key[selected]].shift=gview.shift;
		gview.interfacekey[allowed_key[selected]].ctrl=gview.ctrl;
		gview.interfacekey[allowed_key[selected]].alt=gview.alt;
		binding=0;
		gview.c=0;
		}
	else
		{
		if(gview.keypress(INTERFACEKEY_CHANGE_BINDINGS))
			{
			//START THE BINDING PROCESS
			binding=1;
			}

		standardscrolling(selected,0,allowed_key.size()-1,21);

		if(gview.keypress(INTERFACEKEY_SAVE_BINDINGS))
			{
			//SAVE THE INTERFACE
			std::ofstream fseed("data/init/interface.txt");
			if(fseed.is_open())
				{
				string str;

				interfacekeyst key;
				long i,j;
				for(i=1;i<256;i++)
					{
					for(j=0;j<8;j++)
						{
						if(!gview.display_string[i][j].empty())
							{
							key.binding=i;
							key.shift=((j%2)==1);
							key.ctrl=(((j/2)%2)==1);
							key.alt=(((j/4)%2)==1);

							getstring_interfacechar_save(str,key);

							if(str!="NOT_SUPPORTED")
								{
								fseed<<"[DISPLAY_STRING:"<<str.c_str()<<':'<<gview.display_string[i][j].c_str()<<']'<<std::endl;
								}
							}
						}
					}

				fseed<<std::endl;

				for(i=0;i<INTERFACEKEYNUM;i++)
					{
					getstring_interfacechar_save(str,gview.interfacekey[i]);
					fseed<<'['<<gview.interfacekey[i].tokenname.data()<<':'<<str.c_str()<<']'<<std::endl;
					}
				}
			fseed.close();
			gview.c=0;
			}

		if(gview.keypress(INTERFACEKEY_LEAVESCREEN))
			{
			breakdownlevel=INTERFACE_BREAKDOWN_STOPSCREEN;
			return;
			}
		}
}

void viewscreen_movieplayerst::help()
{
	if(is_playing)return;

	viewscreenst::help();
}

void viewscreen_keybindingsst::help()
{
	binding=0;

	viewscreenst::help();
}

bool viewscreen_keybindingsst::key_conflict(InterfaceKey test_key)
{
	if(test_key!=INTERFACEKEY_CHANGE_BINDINGS&&test_key>=(InterfaceKey)0&&test_key<INTERFACEKEYNUM)
		{
		if(gview.interfacekey[test_key].binding==gview.interfacekey[INTERFACEKEY_CHANGE_BINDINGS].binding&&
			gview.interfacekey[test_key].shift==gview.interfacekey[INTERFACEKEY_CHANGE_BINDINGS].shift&&
			gview.interfacekey[test_key].ctrl==gview.interfacekey[INTERFACEKEY_CHANGE_BINDINGS].ctrl&&
			gview.interfacekey[test_key].alt==gview.interfacekey[INTERFACEKEY_CHANGE_BINDINGS].alt)
			{
			return true;
			}
		}
	return false;
}

void interfacest::finish_movie()
{
	supermovie_on=0;
	currentblocksize=0;
	nextfilepos=0;
	supermovie_pos=0;
	viewscreen_movieplayerst::create(INTERFACE_PUSH_AT_BACK);
}

void interfacest::use_movie_input()
{
	if(supermovie_on)handlemovie(1);
	finish_movie();
	c=0;
}

viewscreen_movieplayerst *viewscreen_movieplayerst::create(char pushtype,viewscreenst *scr)
{
	viewscreen_movieplayerst *newv=new viewscreen_movieplayerst();
	gview.addscreen(newv,pushtype,scr);

	return newv;
}

void viewscreen_movieplayerst::force_play(string &file)
{
	force_file=file;
	is_forced_play=1;
}

void viewscreen_movieplayerst::view()
{
	enabler.flag&=~ENABLERFLAG_MAXFPS;
	enabler.qprate=enabler.main_qprate;//USE THE MAIN RATE FOR MOVIES

	if(!force_file.empty()&&!is_playing&&!quit_if_no_play&&is_forced_play)
		{
		is_playing=1;
		quit_if_no_play=1;
		gview.movie_file=force_file;
		gview.supermovie_on=0;
		gview.currentblocksize=0;
		gview.nextfilepos=0;
		gview.supermovie_pos=0;
		maxmoviepos=0;
		}

	if(!is_playing&&quit_if_no_play)
		{
		breakdownlevel=INTERFACE_BREAKDOWN_STOPSCREEN;
		init.display.grid_x=init.display.orig_grid_x;//JUST IN CASE
		init.display.grid_y=init.display.orig_grid_y;
		gps.prepare_rect(1);
		return;
		}

	if(!quit_if_no_play)
		{
		if(editing)drawborder(NULL);
		else drawborder("  Moving Records  ");
		}

	//LOAD A MOVIE BUFFER BY BUFFER
	if(is_playing)
		{
		if(gview.supermovie_pos>=MOVIEBUFFSIZE||gview.currentblocksize==0)
			{
			gview.read_movie_chunk(maxmoviepos,is_playing);
			}

		if(is_playing)
			{
			long half_frame_size=init.display.grid_x*init.display.grid_y;

			//PLAY ANY RELEVANT SOUNDS
			if(gview.supermovie_delaystep==gview.supermovie_delayrate)
				{
				long fr=gview.supermovie_pos/(half_frame_size*2);
				if(fr>=0&&fr<200)
					{
					long c,sd;
					for(c=0;c<16;c++)
						{
						sd=gview.supermovie_sound_time[fr][c];
						if(sd>=0&&sd<gview.supermovie_sound.str.size())
							{
#ifndef NO_FMOD
							musicsound.playsound(sd,c);
#endif
							}
						}
					}
				}

			//PRINT THE NEXT FRAME AND ADVANCE POSITION
			drawborder(NULL,-1);
			
			short x2,y2;
			for(x2=0;x2<init.display.grid_x;x2++)
				{
				for(y2=0;y2<init.display.grid_y;y2++)
					{
					gps.locate(y2,x2);

					gps.changecolor((gview.supermoviebuffer[gview.supermovie_pos+half_frame_size] & 7),
						(gview.supermoviebuffer[gview.supermovie_pos+half_frame_size] & 56)>>3,
						(gview.supermoviebuffer[gview.supermovie_pos+half_frame_size] & 64));

					gps.addchar(gview.supermoviebuffer[gview.supermovie_pos]);

					gview.supermovie_pos++;
					}
				}
			if(gview.supermovie_delaystep==0)
				{
				gview.supermovie_pos+=half_frame_size;
				gview.supermovie_delaystep=gview.supermovie_delayrate;
				}
			else
				{
				gview.supermovie_pos-=half_frame_size;//RETURN TO LAST FRAME
				gview.supermovie_delaystep--;
				}

			//DONE
			if(gview.supermovie_pos>=maxmoviepos&&
				maxmoviepos+half_frame_size*2<MOVIEBUFFSIZE)
				{
				is_playing=0;
				init.display.grid_x=init.display.orig_grid_x;
				init.display.grid_y=init.display.orig_grid_y;
				//NOTE: THIS CAUSES IT TO LOSE THE LAST FRAME DUE TO CLEARING
				gps.prepare_rect(1);
				}
			}
		}
	else if(loading)
		{
		int scroll=selfile/21*21;
		int l;
		for(l=scroll;l<filelist.size() && l<scroll+21;l++)
			{
			if(l==selfile)gps.changecolor(7,0,1);
			else gps.changecolor(7,0,0);
			gps.locate(l-scroll+2,2);
			gps.addst(filelist[l]);
			}
		}
#ifdef DEBUG_MOVIE_EDIT
	else if(editing)
		{
		if(editing_menu)
			{
			long tx,ty;
			unsigned char c=0;
			for(ty=0;ty<16;ty++)
				{
				for(tx=0;tx<16;tx++)
					{
					gps.locate(ty,tx);
					gps.changecolor(editing_screenf,editing_screenb,editing_screenbright);
					gps.addchar(c);
					c++;
					}
				}
			gps.locate(18,0);
			gps.changecolor(editing_screenf,editing_screenb,editing_screenbright);
			gps.addchar(editing_char);
			for(ty=0;ty<16;ty++)
				{
				for(tx=0;tx<8;tx++)
					{
					gps.locate(ty,tx+16);
					gps.changecolor(ty%8,tx,ty/8);
					gps.addchar('A');
					}
				}

			gps.changecolor(7,0,1);
			gps.locate(20,0);
			string str;
			gps.addst("1/100 sec per frame: ");convert_long_to_string(gview.supermovie_delayrate,str);
			gps.addst(str);

			long scroll=(editing_selected_sound/25)*25;
			long e;
			for(e=scroll;e<scroll+25&&e<gview.supermovie_sound.str.size();e++)
				{
				if(e==editing_selected_sound)gps.changecolor(7,0,1);
				else gps.changecolor(7,0,0);
				gps.locate(e-scroll,26);
				gps.addst(gview.supermovie_sound.str[e]->dat);
				}

			long frame=gview.supermovie_pos/4000,sd;
			for(e=0;e<SOUND_CHANNELNUM;e++)
				{
				gps.changecolor(2,0,1);
				gps.locate(e-scroll,52);
				sd=gview.supermovie_sound_time[frame][e];
				if(sd>=0&&sd<gview.supermovie_sound.str.size())gps.addst(gview.supermovie_sound.str[sd]->dat);
				else
					{
					gps.addst("-----------------");
					}
				gps.changecolor(4,0,1);
				gps.locate(e-scroll,(init.display.grid_x-1));
				gps.addst("X");
				}
			}
		else
			{
			drawborder(NULL,-1);

			long curp=gview.supermovie_pos;
			int x2,y2;
			for(x2=0;x2<80;x2++)
				{
				for(y2=0;y2<25;y2++)
					{
					gps.locate(y2,x2);

					gps.changecolor((gview.supermoviebuffer[curp+2000] & 7),
						(gview.supermoviebuffer[curp+2000] & 56)>>3,
						(gview.supermoviebuffer[curp+2000] & 64));

					gps.addchar(gview.supermoviebuffer[curp]);

					curp++;
					}
				}

			if(enabler.mouse_y<150)gps.locate(24,0);
			else gps.locate(0,0);
			gps.changecolor(2,0,1);
			gps.addst("Frame: ");
			string num;
			convert_long_to_string(gview.supermovie_pos/4000+1,num);
			gps.addst(num);

			if(enabler.mouse_y<150)gps.locate(24,20);
			else gps.locate(0,20);
			gps.changecolor(3,0,1);
			gps.addst("Copy From: ");
			convert_long_to_string(editing_copy_from/4000+1,num);
			gps.addst(num);

			if(enabler.mouse_y<150)gps.locate(24,40);
			else gps.locate(0,40);
			gps.changecolor(4,0,1);
			gps.addst("Ends At: ");
			convert_long_to_string(end_frame_pos/4000+1,num);
			gps.addst(num);

			if(enabler.mouse_y<150)gps.locate(24,60);
			else gps.locate(0,60);
			long sx,sy;
			gps.get_mouse_text_coords(sx,sy);
			gps.changecolor(7,0,1);
			gps.addst("(");
			convert_long_to_string(sx,num);
			gps.addst(num);
			gps.addst(",");
			convert_long_to_string(sy,num);
			gps.addst(num);
			gps.addst(")");
			}
		}
#endif
	else
		{
		gps.changecolor(7,0,1);
		gps.locate(2,2);
		gview.print_interface_token(INTERFACEKEY_MOVIE_RECORD);
		gps.addst(": Start recording (active record is erased, stops when you return here)");
		gps.locate(3,2);
		gview.print_interface_token(INTERFACEKEY_MOVIE_PLAY);
		gps.addst(": Play the active moving record");
		gps.locate(4,2);
		gview.print_interface_token(INTERFACEKEY_MOVIE_SAVE);
		gps.addst(": Save the active moving record (you will be prompted for a name)");
		gps.locate(5,2);
		gview.print_interface_token(INTERFACEKEY_MOVIE_LOAD);
		gps.addst(": Load a moving record");

#ifdef DEBUG_MOVIE_EDIT
		gps.locate(7,2);
		gps.addst("E: Edit");
#endif

		if(saving)
			{
			gps.locate(10,2);
			gps.addst("Name: ");
			gps.addst(savename);
			}
		}

	gps.renewscreen();

	if(gview.keypress(INTERFACEKEY_LEAVESCREEN))
		{
		if(is_playing)
			{
			is_playing=0;
			init.display.grid_x=init.display.orig_grid_x;
			init.display.grid_y=init.display.orig_grid_y;
			gps.prepare_rect(1);
			gview.supermovie_on=0;
			gview.currentblocksize=0;
			gview.nextfilepos=0;
			gview.supermovie_pos=0;
			maxmoviepos=0;

#ifndef NO_FMOD
			musicsound.stop_sound();
#endif
			}
		else if(saving)saving=0;
		else if(loading)loading=0;
#ifdef DEBUG_MOVIE_EDIT
		else if(editing)editing=0;
#endif
		else
			{
			is_playing=0;
			init.display.grid_x=init.display.orig_grid_x;
			init.display.grid_y=init.display.orig_grid_y;
			gps.prepare_rect(1);
			gview.supermovie_on=0;
			gview.currentblocksize=0;
			gview.nextfilepos=0;
			gview.supermovie_pos=0;
			maxmoviepos=0;

			breakdownlevel=INTERFACE_BREAKDOWN_STOPSCREEN;
			return;
			}
		}
	else if(saving)
		{
		standardstringentry(savename,39,STRINGENTRY_LETTERS|STRINGENTRY_SPACE|STRINGENTRY_NUMBERS|STRINGENTRY_SYMBOLS);

		if(gview.keypress(INTERFACEKEY_SELECT))
			{
			string filename;
			filename="data/movies/";
			filename+=savename;
			filename+=".cmv";

			copy_file(gview.movie_file,filename);
			saving=0;
			}
		}
	else if(loading)
		{
		if(gview.keypress(INTERFACEKEY_SELECT))
			{
			string filename;
			filename="data/movies/";
			filename+=filelist[selfile];

			if(filename!=gview.movie_file)
				{
				copy_file(filename,gview.movie_file);
				}
			loading=0;
			}

		standardscrolling(selfile,0,filelist.size()-1,21);
		}
#ifdef DEBUG_MOVIE_EDIT
	else if(editing)
		{
		char entering=0;

		if(editing_menu)
			{
			if(enabler.mouse_lbut&&enabler.tracking_on)
				{
				long sx,sy;
				gps.get_mouse_text_coords(sx,sy);

				if(sx>=0&&sx<80&&sy>=0&&sy<25)
					{
					if(sx>=0&&sx<16&&sy>=0&&sy<16)
						{
						editing_char=sx+sy*16;
						}
					if(sx>=16&&sx<24&&sy>=0&&sy<16)
						{
						editing_screenf=sy%8;
						editing_screenb=sx-16;
						editing_screenbright=sy/8;
						}
					if(sx>=26&&sx<=51&&sy>=0&&sy<SOUND_CHANNELNUM)
						{
						editing_selected_sound=sy;
						}
					long frame=gview.supermovie_pos/4000;
					if(sx>=52&&sx<=78&&sy>=0&&sy<SOUND_CHANNELNUM)
						{
						gview.supermovie_sound_time[frame][sy]=editing_selected_sound;
						}
					if(sx==(init.display.grid_x-1)&&sy>=0&&sy<SOUND_CHANNELNUM)
						{
						gview.supermovie_sound_time[frame][sy]=-1;
						}
					}

				enabler.mouse_lbut=0;
				}

			if(enabler.mouse_rbut&&enabler.tracking_on)
				{
				editing_menu=0;
				enabler.mouse_rbut=0;
				}

			if(editing_selected_sound>=0&&editing_selected_sound<gview.supermovie_sound.str.size())
				{
				if(gview.c== '%')
					{
					delete gview.supermovie_sound.str[editing_selected_sound];
					gview.supermovie_sound.str.erase(editing_selected_sound);
					}
				else
					{
					standardstringentry(gview.supermovie_sound.str[editing_selected_sound]->dat,26,STRINGENTRY_LETTERS|STRINGENTRY_SPACE|STRINGENTRY_NUMBERS|STRINGENTRY_SYMBOLS);
					entering=1;
					}
				}
			else
				{
				if(gview.c== '#')gview.supermovie_sound.add_string("new_sound");
				if(gview.c== '+')gview.supermovie_delayrate++;
				if(gview.c== '-')gview.supermovie_delayrate--;
				if(gview.c== 'T')text_mode=1-text_mode;
				}
			if(gview.supermovie_delayrate<0)gview.supermovie_delayrate=0;
			if(gview.supermovie_delayrate>10)gview.supermovie_delayrate=10;
			}
		else
			{
			if(text_mode)
				{
				if(gview.c!=0)
					{
					long sx,sy;
					gps.get_mouse_text_coords(sx,sy);

					if(sx>=0&&sx<80&&sy>=0&&sy<25)
						{
						long curpos=gview.supermovie_pos+sy+sx*25;
						gview.supermoviebuffer[curpos]=gview.c;
						gview.supermoviebuffer[curpos+2000]=(editing_screenf&7)+((editing_screenb&7)<<3);
						if(editing_screenbright)gview.supermoviebuffer[curpos+2000]+=64;
						}
					}
				}
			else
				{
				if(gview.c== 'a')
					{
					int x2,y2;
					for(x2=0;x2<80;x2++)
						{
						for(y2=0;y2<25;y2++)
							{
							if(x2>0)
								{
								gview.supermoviebuffer[gview.supermovie_pos+y2+(x2-1)*25]=gview.supermoviebuffer[gview.supermovie_pos+y2+x2*25];
								gview.supermoviebuffer[gview.supermovie_pos+y2+(x2-1)*25+2000]=gview.supermoviebuffer[gview.supermovie_pos+y2+x2*25+2000];
								}
							if(x2==(init.display.grid_x-1))gview.supermoviebuffer[gview.supermovie_pos+y2+x2*25]=0;
							}
						}
					}
				if(gview.c== 'd')
					{
					int x2,y2;
					for(x2=(init.display.grid_x-1);x2>=0;x2--)
						{
						for(y2=0;y2<(init.display.grid_y-1);y2++)
							{
							if(x2<(init.display.grid_x-1))
								{
								gview.supermoviebuffer[gview.supermovie_pos+y2+(x2+1)*25]=gview.supermoviebuffer[gview.supermovie_pos+y2+x2*25];
								gview.supermoviebuffer[gview.supermovie_pos+y2+(x2+1)*25+2000]=gview.supermoviebuffer[gview.supermovie_pos+y2+x2*25+2000];
								}
							if(x2==0)gview.supermoviebuffer[gview.supermovie_pos+y2+x2*25]=0;
							}
						}
					}
				if(gview.c== 'E')end_frame_pos=gview.supermovie_pos;
				if(gview.c== 'c')editing_copy_from=gview.supermovie_pos;
				if(gview.c== 'p')
					{
					long i;
					for(i=0;i<4000;i++)
						{
						gview.supermoviebuffer[gview.supermovie_pos+i]=gview.supermoviebuffer[editing_copy_from+i];
						}
					}
				if(gview.c== '+')gview.supermovie_pos+=4000;
				if(gview.c== '-')gview.supermovie_pos-=4000;
				if(gview.c== '/')gview.supermovie_pos-=40000;
				if(gview.c== '*')gview.supermovie_pos+=40000;
				if(gview.supermovie_pos<0)gview.supermovie_pos=0;
				if(gview.supermovie_pos>=MOVIEBUFFSIZE)gview.supermovie_pos=MOVIEBUFFSIZE-4000;
				}

			if(enabler.mouse_lbut&&enabler.tracking_on)
				{
				long sx,sy;
				gps.get_mouse_text_coords(sx,sy);

				if(sx>=0&&sx<80&&sy>=0&&sy<25)
					{
					long curpos=gview.supermovie_pos+sy+sx*25;
					gview.supermoviebuffer[curpos]=editing_char;
					gview.supermoviebuffer[curpos+2000]=(editing_screenf&7)+((editing_screenb&7)<<3);
					if(editing_screenbright)gview.supermoviebuffer[curpos+2000]+=64;
					}
				}
			if(enabler.mouse_rbut&&enabler.tracking_on)
				{
				editing_menu=1;
				enabler.mouse_rbut=0;
				}
			}

		if(!entering&&gview.c== 'S')
			{
			long opos=gview.supermovie_pos;
			gview.first_movie_write=1;
			gview.supermovie_pos=end_frame_pos+4000;

			gview.write_movie_chunk();

			gview.supermovie_pos=opos;
			}
		}
#endif
	else
		{
		if(is_playing)
			{
			}
		else
			{
#ifdef DEBUG_MOVIE_EDIT
			if(gview.c== 'E')
				{
				editing=1;
				gview.supermovie_pos=0;
				}
#endif

			if(gview.keypress(INTERFACEKEY_MOVIE_RECORD))
				{
				//TURN ON THE MOVIE RECORDER
				is_playing=0;
				init.display.grid_x=init.display.orig_grid_x;
				init.display.grid_y=init.display.orig_grid_y;
				gps.prepare_rect(1);
				gview.supermovie_on=1;
				gview.currentblocksize=0;
				gview.nextfilepos=0;
				gview.supermovie_pos=0;
				gview.supermovie_delayrate=0;
				gview.first_movie_write=1;
				maxmoviepos=0;

				breakdownlevel=INTERFACE_BREAKDOWN_STOPSCREEN;
				}
			if(gview.keypress(INTERFACEKEY_MOVIE_PLAY))
				{
				is_playing=1;
				gview.supermovie_on=0;
				gview.currentblocksize=0;
				gview.nextfilepos=0;
				gview.supermovie_pos=0;
				maxmoviepos=0;
				}
			if(gview.keypress(INTERFACEKEY_MOVIE_SAVE))
				{
				savename.erase();
				saving=1;
				}
			if(gview.keypress(INTERFACEKEY_MOVIE_LOAD))
				{
				selfile=0;

				clearfilelist();

				find_files_by_pattern("data/movies/*.cmv",filelist);

				if(filelist.size()>0)loading=1;
				}
			}
		}
}

void viewscreen_movieplayerst::clearfilelist()
{
	int f;
	for(f=0;f<filelist.size();f++)delete[] filelist[f];
	filelist.clear();
}

viewscreen_movieplayerst::viewscreen_movieplayerst()
{
	force_file.erase();
	gview.movie_file="data/movies/last_record.cmv";
	is_playing=0;
	init.display.grid_x=init.display.orig_grid_x;
	init.display.grid_y=init.display.orig_grid_y;
	gps.prepare_rect(1);
	is_forced_play=0;
	quit_if_no_play=0;
	gview.supermovie_on=0;
	gview.currentblocksize=0;
	gview.nextfilepos=0;
	gview.supermovie_pos=0;
	maxmoviepos=0;
	saving=0;
	loading=0;
	editing=0;
	text_mode=0;
	editing_copy_from=0;
	editing_char=219;
	editing_screenf=7;
	editing_screenb=0;
	editing_screenbright=0;
	editing_menu=0;
	editing_selected_sound=0;
	end_frame_pos=0;
	gview.supermovie_sound.clean();
	long i,c;
	for(i=0;i<200;i++)
		{
		for(c=0;c<SOUND_CHANNELNUM;c++)gview.supermovie_sound_time[i][c]=-1;
		}
}

interfacest::interfacest()
{
	shutdown_interface_for_ms=0;
	shutdown_interface_tickcount=0;
	c=0;
	caps=0;
	shift=0;
    alt=0;
    upper=0;
	flag=0;
	supermovie_on=0;
	supermovie_pos=0;
	supermovie_delayrate=0;

	interfacekey[INTERFACEKEY_SELECT].name="Select";
	interfacekey[INTERFACEKEY_DESELECT].name="Deselect";
	interfacekey[INTERFACEKEY_SELECT_ALL].name="Select all";
	interfacekey[INTERFACEKEY_DESELECT_ALL].name="Deselect All";
	interfacekey[INTERFACEKEY_LEAVESCREEN].name="Leave screen";
	interfacekey[INTERFACEKEY_LEAVESCREEN_TEXT].name="Leave text entry screen";
	interfacekey[INTERFACEKEY_LEAVESCREEN_ALL].name="Leave all screens";
	interfacekey[INTERFACEKEY_CLOSE_MEGA_ANNOUNCEMENT].name="Close mega announcement";
	interfacekey[INTERFACEKEY_TOGGLE_FULLSCREEN].name="Toggle Fullscreen";
	interfacekey[INTERFACEKEY_WORLD_PARAM_ADD].name="World Param: Add";
	interfacekey[INTERFACEKEY_WORLD_PARAM_COPY].name="World Param: Copy";
	interfacekey[INTERFACEKEY_WORLD_PARAM_DELETE].name="World Param: Delete";
	interfacekey[INTERFACEKEY_WORLD_PARAM_TITLE].name="World Param: Title";
	interfacekey[INTERFACEKEY_WORLD_PARAM_NAME_RANDOM].name="World Param: Name, Random";
	interfacekey[INTERFACEKEY_WORLD_PARAM_NAME_ENTER].name="World Param: Name, Enter";
	interfacekey[INTERFACEKEY_WORLD_PARAM_SEED_RANDOM].name="World Param: Seed, Random";
	interfacekey[INTERFACEKEY_WORLD_PARAM_SEED_ENTER].name="World Param: Seed, Enter";
	interfacekey[INTERFACEKEY_WORLD_PARAM_LOAD].name="World Param: Load";
	interfacekey[INTERFACEKEY_WORLD_PARAM_SAVE].name="World Param: Save";
	interfacekey[INTERFACEKEY_WORLD_PARAM_DIM_X_UP].name="World Param: Dim X, Up";
	interfacekey[INTERFACEKEY_WORLD_PARAM_DIM_X_DOWN].name="World Param: Dim X, Down";
	interfacekey[INTERFACEKEY_WORLD_PARAM_DIM_Y_UP].name="World Param: Dim Y, Up";
	interfacekey[INTERFACEKEY_WORLD_PARAM_DIM_Y_DOWN].name="World Param: Dim Y, Down";
	interfacekey[INTERFACEKEY_WORLD_PARAM_SET].name="World Param: Enter Advanced Parameters";
	interfacekey[INTERFACEKEY_WORLD_PARAM_INCREASE].name="World Param: Increase Parameter";
	interfacekey[INTERFACEKEY_WORLD_PARAM_DECREASE].name="World Param: Decrease Parameter";
	interfacekey[INTERFACEKEY_WORLD_PARAM_ENTER_VALUE].name="World Param: Enter Value";
	interfacekey[INTERFACEKEY_WORLD_PARAM_NULLIFY].name="World Param: Nullify Parameter";
	interfacekey[INTERFACEKEY_WORLD_PARAM_PRESET].name="World Param: Set Presets";
	interfacekey[INTERFACEKEY_WORLD_PARAM_REJECT_CONTINUE].name="World Param: Reject, Continue";
	interfacekey[INTERFACEKEY_WORLD_PARAM_REJECT_ABORT].name="World Param: Reject, Abort";
	interfacekey[INTERFACEKEY_WORLD_PARAM_REJECT_ALLOW_THIS].name="World Param: Reject, Allow This";
	interfacekey[INTERFACEKEY_WORLD_PARAM_REJECT_ALLOW_ALL].name="World Param: Reject, Allow All";
	interfacekey[INTERFACEKEY_WORLD_GEN_CONTINUE].name="World Generation: Continue";
	interfacekey[INTERFACEKEY_WORLD_GEN_USE].name="World Generation: Use";
	interfacekey[INTERFACEKEY_WORLD_GEN_ABORT].name="World Generation: Abort";
	interfacekey[INTERFACEKEY_SETUP_EMBARK].name="Setup game: Embark";
	interfacekey[INTERFACEKEY_SETUP_NAME_FORT].name="Setup game: Name Fort";
	interfacekey[INTERFACEKEY_SETUP_NAME_GROUP].name="Setup game: Name Group";
	interfacekey[INTERFACEKEY_SETUP_RECLAIM].name="Setup game: Reclaim";
	interfacekey[INTERFACEKEY_SETUP_FIND].name="Setup game: Find";
	interfacekey[INTERFACEKEY_SETUP_NOTES].name="Setup game: Notes";
	interfacekey[INTERFACEKEY_SETUP_NOTES_TAKE_NOTES].name="Setup game: Notes, New Note";
	interfacekey[INTERFACEKEY_SETUP_NOTES_DELETE_NOTE].name="Setup game: Notes, Delete Note";
	interfacekey[INTERFACEKEY_SETUP_NOTES_CHANGE_SYMBOL_SELECTION].name="Setup game: Notes, Change Symbol Selection";
	interfacekey[INTERFACEKEY_SETUP_NOTES_ADOPT_SYMBOL].name="Setup game: Notes, Adopt Symbol";
	interfacekey[INTERFACEKEY_SETUP_LOCAL_Y_UP].name="Setup game: Resize Local Y Up";
	interfacekey[INTERFACEKEY_SETUP_LOCAL_Y_DOWN].name="Setup game: Resize Local Y Down";
	interfacekey[INTERFACEKEY_SETUP_LOCAL_X_UP].name="Setup game: Resize Local X Up";
	interfacekey[INTERFACEKEY_SETUP_LOCAL_X_DOWN].name="Setup game: Resize Local X Down";
	interfacekey[INTERFACEKEY_SETUP_LOCAL_Y_MUP].name="Setup game: Move Local Y Up";
	interfacekey[INTERFACEKEY_SETUP_LOCAL_Y_MDOWN].name="Setup game: Move Local Y Down";
	interfacekey[INTERFACEKEY_SETUP_LOCAL_X_MUP].name="Setup game: Move Local X Up";
	interfacekey[INTERFACEKEY_SETUP_LOCAL_X_MDOWN].name="Setup game: Move Local X Down";
	interfacekey[INTERFACEKEY_SETUP_BIOME_1].name="Setup game: View Biome 1";
	interfacekey[INTERFACEKEY_SETUP_BIOME_2].name="Setup game: View Biome 2";
	interfacekey[INTERFACEKEY_SETUP_BIOME_3].name="Setup game: View Biome 3";
	interfacekey[INTERFACEKEY_SETUP_BIOME_4].name="Setup game: View Biome 4";
	interfacekey[INTERFACEKEY_SETUP_BIOME_5].name="Setup game: View Biome 5";
	interfacekey[INTERFACEKEY_SETUP_BIOME_6].name="Setup game: View Biome 6";
	interfacekey[INTERFACEKEY_SETUP_BIOME_7].name="Setup game: View Biome 7";
	interfacekey[INTERFACEKEY_SETUP_BIOME_8].name="Setup game: View Biome 8";
	interfacekey[INTERFACEKEY_SETUP_BIOME_9].name="Setup game: View Biome 9";
	interfacekey[INTERFACEKEY_CHOOSE_NAME_RANDOM].name="Choose name: Random";
	interfacekey[INTERFACEKEY_CHOOSE_NAME_CLEAR].name="Choose name: Clear";
	interfacekey[INTERFACEKEY_CHOOSE_NAME_TYPE].name="Choose name: Type";
	interfacekey[INTERFACEKEY_ITEM_DESCRIPTION].name="View item: Description";
	interfacekey[INTERFACEKEY_ITEM_FORBID].name="View item: Forbid";
	interfacekey[INTERFACEKEY_ITEM_MELT].name="View item: Melt";
	interfacekey[INTERFACEKEY_ITEM_DUMP].name="View item: Dump";
	interfacekey[INTERFACEKEY_ITEM_HIDE].name="View item: Hide";
	interfacekey[INTERFACEKEY_OPTIONS].name="Main menu";
	interfacekey[INTERFACEKEY_OPTION_EXPORT].name="Options, Export Local Image";
	interfacekey[INTERFACEKEY_HELP].name="Help";
	interfacekey[INTERFACEKEY_MOVIES].name="Movies";
	interfacekey[INTERFACEKEY_CHANGETAB].name="Change tab or highlight selection";
	interfacekey[INTERFACEKEY_STANDARDSCROLL_UP].name="Move selector up";
	interfacekey[INTERFACEKEY_STANDARDSCROLL_DOWN].name="Move selector down";
	interfacekey[INTERFACEKEY_STANDARDSCROLL_LEFT].name="Move selector left";
	interfacekey[INTERFACEKEY_STANDARDSCROLL_RIGHT].name="Move selector right";
	interfacekey[INTERFACEKEY_STANDARDSCROLL_PAGEUP].name="Page selector up";
	interfacekey[INTERFACEKEY_STANDARDSCROLL_PAGEDOWN].name="Page selector down";
	interfacekey[INTERFACEKEY_SECONDSCROLL_UP].name="Move secondary selector up";
	interfacekey[INTERFACEKEY_SECONDSCROLL_DOWN].name="Move secondary selector down";
	interfacekey[INTERFACEKEY_SECONDSCROLL_PAGEUP].name="Page secondary selector up";
	interfacekey[INTERFACEKEY_SECONDSCROLL_PAGEDOWN].name="Page secondary selector down";
	interfacekey[INTERFACEKEY_CURSOR_UP].name="Move view/cursor up";
	interfacekey[INTERFACEKEY_CURSOR_DOWN].name="Move view/cursor down";
	interfacekey[INTERFACEKEY_CURSOR_LEFT].name="Move view/cursor left";
	interfacekey[INTERFACEKEY_CURSOR_RIGHT].name="Move view/cursor right";
	interfacekey[INTERFACEKEY_CURSOR_UPLEFT].name="Move view/cursor up and left";
	interfacekey[INTERFACEKEY_CURSOR_UPRIGHT].name="Move view/cursor up and right";
	interfacekey[INTERFACEKEY_CURSOR_DOWNLEFT].name="Move view/cursor down and left";
	interfacekey[INTERFACEKEY_CURSOR_DOWNRIGHT].name="Move view/cursor down and right";
	interfacekey[INTERFACEKEY_CURSOR_UP_FAST].name="Move view/cursor up, fast";
	interfacekey[INTERFACEKEY_CURSOR_DOWN_FAST].name="Move view/cursor down, fast";
	interfacekey[INTERFACEKEY_CURSOR_LEFT_FAST].name="Move view/cursor left, fast";
	interfacekey[INTERFACEKEY_CURSOR_RIGHT_FAST].name="Move view/cursor right, fast";
	interfacekey[INTERFACEKEY_CURSOR_UPLEFT_FAST].name="Move view/cursor up and left, fast";
	interfacekey[INTERFACEKEY_CURSOR_UPRIGHT_FAST].name="Move view/cursor up and right, fast";
	interfacekey[INTERFACEKEY_CURSOR_DOWNLEFT_FAST].name="Move view/cursor down and left, fast";
	interfacekey[INTERFACEKEY_CURSOR_DOWNRIGHT_FAST].name="Move view/cursor down and right, fast";
	interfacekey[INTERFACEKEY_CURSOR_UP_Z].name="Move view/cursor up (z)";
	interfacekey[INTERFACEKEY_CURSOR_DOWN_Z].name="Move view/cursor down (z)";
	interfacekey[INTERFACEKEY_CURSOR_UP_Z_AUX].name="Move view/cursor up (z), aux";
	interfacekey[INTERFACEKEY_CURSOR_DOWN_Z_AUX].name="Move view/cursor down (z), aux";
	interfacekey[INTERFACEKEY_A_MOVE_N].name="Adventure: move north";
	interfacekey[INTERFACEKEY_A_MOVE_S].name="Adventure: move south";
	interfacekey[INTERFACEKEY_A_MOVE_E].name="Adventure: move east";
	interfacekey[INTERFACEKEY_A_MOVE_W].name="Adventure: move west";
	interfacekey[INTERFACEKEY_A_MOVE_NW].name="Adventure: move northwest";
	interfacekey[INTERFACEKEY_A_MOVE_NE].name="Adventure: move northeast";
	interfacekey[INTERFACEKEY_A_MOVE_SW].name="Adventure: move southwest";
	interfacekey[INTERFACEKEY_A_MOVE_SE].name="Adventure: move southeast";
	interfacekey[INTERFACEKEY_A_MOVE_WAIT].name="Adventure: move wait";
	interfacekey[INTERFACEKEY_A_CARE_MOVE_N].name="Adventure: careful move north up/down";
	interfacekey[INTERFACEKEY_A_CARE_MOVE_S].name="Adventure: careful move south up/down";
	interfacekey[INTERFACEKEY_A_CARE_MOVE_E].name="Adventure: careful move east up/down";
	interfacekey[INTERFACEKEY_A_CARE_MOVE_W].name="Adventure: careful move west up/down";
	interfacekey[INTERFACEKEY_A_CARE_MOVE_NW].name="Adventure: careful move northwest up/down";
	interfacekey[INTERFACEKEY_A_CARE_MOVE_NE].name="Adventure: careful move northeast up/down";
	interfacekey[INTERFACEKEY_A_CARE_MOVE_SW].name="Adventure: careful move southwest up/down";
	interfacekey[INTERFACEKEY_A_CARE_MOVE_SE].name="Adventure: careful move southeast up/down";
	interfacekey[INTERFACEKEY_A_CARE_MOVE_UPDOWN].name="Adventure: careful move up/down";
	interfacekey[INTERFACEKEY_A_MOVE_N_UP].name="Adventure: move north/up";
	interfacekey[INTERFACEKEY_A_MOVE_S_UP].name="Adventure: move south/up";
	interfacekey[INTERFACEKEY_A_MOVE_E_UP].name="Adventure: move east/up";
	interfacekey[INTERFACEKEY_A_MOVE_W_UP].name="Adventure: move west/up";
	interfacekey[INTERFACEKEY_A_MOVE_NW_UP].name="Adventure: move northwest/up";
	interfacekey[INTERFACEKEY_A_MOVE_NE_UP].name="Adventure: move northeast/up";
	interfacekey[INTERFACEKEY_A_MOVE_SW_UP].name="Adventure: move southwest/up";
	interfacekey[INTERFACEKEY_A_MOVE_SE_UP].name="Adventure: move southeast/up";
	interfacekey[INTERFACEKEY_A_MOVE_UP].name="Adventure: move up";
	interfacekey[INTERFACEKEY_A_MOVE_N_DOWN].name="Adventure: move north/down";
	interfacekey[INTERFACEKEY_A_MOVE_S_DOWN].name="Adventure: move south/down";
	interfacekey[INTERFACEKEY_A_MOVE_E_DOWN].name="Adventure: move east/down";
	interfacekey[INTERFACEKEY_A_MOVE_W_DOWN].name="Adventure: move west/down";
	interfacekey[INTERFACEKEY_A_MOVE_NW_DOWN].name="Adventure: move northwest/down";
	interfacekey[INTERFACEKEY_A_MOVE_NE_DOWN].name="Adventure: move northeast/down";
	interfacekey[INTERFACEKEY_A_MOVE_SW_DOWN].name="Adventure: move southwest/down";
	interfacekey[INTERFACEKEY_A_MOVE_SE_DOWN].name="Adventure: move southeast/down";
	interfacekey[INTERFACEKEY_A_MOVE_DOWN].name="Adventure: move down";
	interfacekey[INTERFACEKEY_A_MOVE_UP_AUX].name="Adventure: move up, aux";
	interfacekey[INTERFACEKEY_A_MOVE_DOWN_AUX].name="Adventure: move down, aux";
	interfacekey[INTERFACEKEY_WORLDGEN_EXPORT_MAP].name="World Gen: Export Map";
	interfacekey[INTERFACEKEY_LEGENDS_EXPORT_MAP].name="Legends: Export Map";
	interfacekey[INTERFACEKEY_LEGENDS_EXPORT_DETAILED_MAP].name="Legends: Export Detailed Map";
	interfacekey[INTERFACEKEY_LEGENDS_TOGGLE_CIVSITE].name="Legends: Civ/Site";
	interfacekey[INTERFACEKEY_LEGENDS_STRING_FILTER].name="Legends: String filter";
	interfacekey[INTERFACEKEY_A_COMBAT_ATTACK].name="Adventure: Combat, Attack Mode";
	interfacekey[INTERFACEKEY_A_COMBAT_DODGE].name="Adventure: Combat, Dodge Mode";
	interfacekey[INTERFACEKEY_A_COMBAT_CHARGEDEF].name="Adventure: Combat, Charge Defend Mode";
	interfacekey[INTERFACEKEY_A_STATUS].name="Adventure: Status";
	interfacekey[INTERFACEKEY_A_STATUS_WRESTLE].name="Adventure: Status, Close Combat";
	interfacekey[INTERFACEKEY_A_STATUS_CUSTOMIZE].name="Adventure: Status, Customize";
	interfacekey[INTERFACEKEY_A_STATUS_KILLS].name="Adventure: Status, Kills";
	interfacekey[INTERFACEKEY_UNITVIEW_CUSTOMIZE].name="Unit View, Customize";
	interfacekey[INTERFACEKEY_UNITVIEW_RELATIONSHIPS].name="Unit View, Relationships";
	interfacekey[INTERFACEKEY_UNITVIEW_RELATIONSHIPS_ZOOM].name="Unit View, Relationships, Zoom";
	interfacekey[INTERFACEKEY_UNITVIEW_RELATIONSHIPS_VIEW].name="Unit View, Relationships, View";
	interfacekey[INTERFACEKEY_UNITVIEW_KILLS].name="Unit View, Kills";
	interfacekey[INTERFACEKEY_CUSTOMIZE_UNIT_NICKNAME].name="Customize Unit, Nickname";
	interfacekey[INTERFACEKEY_CUSTOMIZE_UNIT_PROFNAME].name="Customize Unit, Profession";
	interfacekey[INTERFACEKEY_A_SLEEP].name="Adventure: Sleep";
	interfacekey[INTERFACEKEY_A_WAIT].name="Adventure: Wait";
	interfacekey[INTERFACEKEY_A_ATTACK].name="Adventure: Attack";
	interfacekey[INTERFACEKEY_A_LOOK].name="Adventure: Look";
	interfacekey[INTERFACEKEY_A_SEARCH].name="Adventure: Search";
	interfacekey[INTERFACEKEY_A_TALK].name="Adventure: Talk";
	interfacekey[INTERFACEKEY_A_INTERACT].name="Adventure: Inv. Interact";
	interfacekey[INTERFACEKEY_A_INV_LOOK].name="Adventure: Inv. Look";
	interfacekey[INTERFACEKEY_A_INV_REMOVE].name="Adventure: Inv. Remove";
	interfacekey[INTERFACEKEY_A_INV_WEAR].name="Adventure: Inv. Wear";
	interfacekey[INTERFACEKEY_A_INV_EATDRINK].name="Adventure: Inv. Eat/Drink";
	interfacekey[INTERFACEKEY_A_INV_PUTIN].name="Adventure: Inv. Put In";
	interfacekey[INTERFACEKEY_A_INV_DROP].name="Adventure: Inv. Drop";
	interfacekey[INTERFACEKEY_A_GROUND].name="Adventure: Get/Ground";
	interfacekey[INTERFACEKEY_A_THROW].name="Adventure: Throw";
	interfacekey[INTERFACEKEY_A_SHOOT].name="Adventure: Fire";
	interfacekey[INTERFACEKEY_A_ANNOUNCEMENTS].name="Adventure: Announcements";
	interfacekey[INTERFACEKEY_A_COMBAT].name="Adventure: Combat Options";
	interfacekey[INTERFACEKEY_A_MOVEMENT].name="Adventure: Movement Options";
	interfacekey[INTERFACEKEY_A_MOVEMENT_SWIM].name="Adventure: Movement Options, Swim";
	interfacekey[INTERFACEKEY_A_SNEAK].name="Adventure: Sneak";
	interfacekey[INTERFACEKEY_A_CENTER].name="Adventure: Center";
	interfacekey[INTERFACEKEY_A_BUILDING].name="Adventure: Building Interact";
	interfacekey[INTERFACEKEY_A_TRAVEL].name="Adventure: Travel";
	interfacekey[INTERFACEKEY_A_DATE].name="Adventure: Get Date";
	interfacekey[INTERFACEKEY_A_WEATHER].name="Adventure: Get Weather";
	interfacekey[INTERFACEKEY_A_TEMPERATURE].name="Adventure: Get Temperature";
	interfacekey[INTERFACEKEY_A_STANCE].name="Adventure: Change Stance";
	interfacekey[INTERFACEKEY_A_OPTION1].name="Adventure: Option 1";
	interfacekey[INTERFACEKEY_A_OPTION2].name="Adventure: Option 2";
	interfacekey[INTERFACEKEY_A_OPTION3].name="Adventure: Option 3";
	interfacekey[INTERFACEKEY_A_OPTION4].name="Adventure: Option 4";
	interfacekey[INTERFACEKEY_A_OPTION5].name="Adventure: Option 5";
	interfacekey[INTERFACEKEY_A_OPTION6].name="Adventure: Option 6";
	interfacekey[INTERFACEKEY_A_OPTION7].name="Adventure: Option 7";
	interfacekey[INTERFACEKEY_A_OPTION8].name="Adventure: Option 8";
	interfacekey[INTERFACEKEY_A_OPTION9].name="Adventure: Option 9";
	interfacekey[INTERFACEKEY_A_OPTION10].name="Adventure: Option 10";
	interfacekey[INTERFACEKEY_A_OPTION11].name="Adventure: Option 11";
	interfacekey[INTERFACEKEY_A_OPTION12].name="Adventure: Option 12";
	interfacekey[INTERFACEKEY_A_OPTION13].name="Adventure: Option 13";
	interfacekey[INTERFACEKEY_A_OPTION14].name="Adventure: Option 14";
	interfacekey[INTERFACEKEY_A_OPTION15].name="Adventure: Option 15";
	interfacekey[INTERFACEKEY_A_OPTION16].name="Adventure: Option 16";
	interfacekey[INTERFACEKEY_A_OPTION17].name="Adventure: Option 17";
	interfacekey[INTERFACEKEY_A_OPTION18].name="Adventure: Option 18";
	interfacekey[INTERFACEKEY_A_OPTION19].name="Adventure: Option 19";
	interfacekey[INTERFACEKEY_A_OPTION20].name="Adventure: Option 20";
	interfacekey[INTERFACEKEY_HOTKEY_MAKE_ASH].name="Hotkey: Make Ash";
	interfacekey[INTERFACEKEY_HOTKEY_MAKE_CHARCOAL].name="Hotkey: Make Charcoal";
	interfacekey[INTERFACEKEY_HOTKEY_MELT_OBJECT].name="Hotkey: Melt Object";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_GREEN].name="Hotkey: Green Glass";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_CLEAR].name="Hotkey: Clear Glass";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_CRYSTAL].name="Hotkey: Crystal Glass";
	interfacekey[INTERFACEKEY_HOTKEY_COLLECT_SAND].name="Hotkey: Collect Sand";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_ROUGH].name="Hotkey: Raw Glass";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_ARMORSTAND].name="Hotkey: Glass Armorstand";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_BOX].name="Hotkey: Glass Box";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_CABINET].name="Hotkey: Glass Cabinet";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_COFFIN].name="Hotkey: Glass Coffin";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_FLOODGATE].name="Hotkey: Glass Floodgate";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_HATCH_COVER].name="Hotkey: Glass Hatch Cover";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_GRATE].name="Hotkey: Glass Grate";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_GOBLET].name="Hotkey: Glass Goblet";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_TOY].name="Hotkey: Glass Toy";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_INSTRUMENT].name="Hotkey: Glass Instrument";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_DOOR].name="Hotkey: Glass Portal";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_STATUE].name="Hotkey: Glass Statue";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_TABLE].name="Hotkey: Glass Table";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_CAGE].name="Hotkey: Glass Cage";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_CHAIR].name="Hotkey: Glass Chair";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_BLOCKS].name="Hotkey: Glass Blocks";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_FLASK].name="Hotkey: Glass Vial";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_WEAPONRACK].name="Hotkey: Glass Weaponrack";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_WINDOW].name="Hotkey: Glass Window";
	interfacekey[INTERFACEKEY_HOTKEY_MAKE_PEARLASH].name="Hotkey: Make Pearlash";
	interfacekey[INTERFACEKEY_HOTKEY_ASHERY_LYE].name="Hotkey: Ashery, Lye";
	interfacekey[INTERFACEKEY_HOTKEY_ASHERY_POTASH].name="Hotkey: Ashery, Potash (Lye)";
	interfacekey[INTERFACEKEY_HOTKEY_ASHERY_POTASH_DIRECT].name="Hotkey: Ashery, Potash (Ash)";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_BARREL].name="Hotkey: Carpenter, Barrel";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_BLOCKS].name="Hotkey: Carpenter, Blocks";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_BUCKET].name="Hotkey: Carpenter, Bucket";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_TRAP_ANIMAL].name="Hotkey: Carpenter, Animal Trap";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_CAGE].name="Hotkey: Carpenter, Cage";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_ARMORSTAND].name="Hotkey: Carpenter, Armorstand";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_BED].name="Hotkey: Carpenter, Bed";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_CHAIR].name="Hotkey: Carpenter, Chair";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_COFFIN].name="Hotkey: Carpenter, Coffin";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_DOOR].name="Hotkey: Carpenter, Door";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_FLOODGATE].name="Hotkey: Carpenter, Floodgate";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_HATCH_COVER].name="Hotkey: Carpenter, Hatch Cover";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_GRATE].name="Hotkey: Carpenter, Grate";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_CABINET].name="Hotkey: Carpenter, Cabinet";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_BIN].name="Hotkey: Carpenter, Bin";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_BOX].name="Hotkey: Carpenter, Box";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_WEAPONRACK].name="Hotkey: Carpenter, Weaponrack";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_TABLE].name="Hotkey: Carpenter, Table";
	interfacekey[INTERFACEKEY_HOTKEY_SIEGE_BALLISTA].name="Hotkey: Siege Shop, Ballista";
	interfacekey[INTERFACEKEY_HOTKEY_SIEGE_CATAPULT].name="Hotkey: Siege Shop, Catapult";
	interfacekey[INTERFACEKEY_HOTKEY_LEATHER_BOX].name="Hotkey: Leather, Bag";
	interfacekey[INTERFACEKEY_HOTKEY_LEATHER_FLASK].name="Hotkey: Leather, Flask";
	interfacekey[INTERFACEKEY_HOTKEY_LEATHER_SHIRT].name="Hotkey: Leather, Shirt";
	interfacekey[INTERFACEKEY_HOTKEY_LEATHER_CLOAK].name="Hotkey: Leather, Cloak";
	interfacekey[INTERFACEKEY_HOTKEY_LEATHER_BACKPACK].name="Hotkey: Leather, Backpack";
	interfacekey[INTERFACEKEY_HOTKEY_LEATHER_QUIVER].name="Hotkey: Leather, Quiver";
	interfacekey[INTERFACEKEY_HOTKEY_LEATHER_IMAGE].name="Hotkey: Leather, Image";
	interfacekey[INTERFACEKEY_HOTKEY_CLOTHES_MAT_PLANT].name="Hotkey: Clothes, Cloth";
	interfacekey[INTERFACEKEY_HOTKEY_CLOTHES_MAT_SILK].name="Hotkey: Clothes, Silk";
	interfacekey[INTERFACEKEY_HOTKEY_CLOTHES_SHIRT].name="Hotkey: Clothes, Shirt";
	interfacekey[INTERFACEKEY_HOTKEY_CLOTHES_CLOAK].name="Hotkey: Clothes, Cloak";
	interfacekey[INTERFACEKEY_HOTKEY_CLOTHES_BOX].name="Hotkey: Clothes, Box";
	interfacekey[INTERFACEKEY_HOTKEY_CLOTHES_CHAIN].name="Hotkey: Clothes, Rope";
	interfacekey[INTERFACEKEY_HOTKEY_CLOTHES_IMAGE].name="Hotkey: Clothes, Image";
	interfacekey[INTERFACEKEY_HOTKEY_CRAFTS_MAT_STONE].name="Hotkey: Crafts, Mat Stone";
	interfacekey[INTERFACEKEY_HOTKEY_CRAFTS_MAT_WOOD].name="Hotkey: Crafts, Mat Wood";
	interfacekey[INTERFACEKEY_HOTKEY_CRAFTS_DEC_BONE].name="Hotkey: Crafts, Dec Bone";
	interfacekey[INTERFACEKEY_HOTKEY_CRAFTS_DEC_SHELL].name="Hotkey: Crafts, Dec Shell";
	interfacekey[INTERFACEKEY_HOTKEY_CRAFTS_TOTEM].name="Hotkey: Crafts, Totem";
	interfacekey[INTERFACEKEY_HOTKEY_CRAFTS_CLOTH].name="Hotkey: Crafts, Cloth";
	interfacekey[INTERFACEKEY_HOTKEY_CRAFTS_SILK].name="Hotkey: Crafts, Silk";
	interfacekey[INTERFACEKEY_HOTKEY_CRAFTS_SHELL].name="Hotkey: Crafts, Shell";
	interfacekey[INTERFACEKEY_HOTKEY_CRAFTS_BONE].name="Hotkey: Crafts, Bone";
	interfacekey[INTERFACEKEY_HOTKEY_CRAFTS_LEATHER].name="Hotkey: Crafts, Leather";
	interfacekey[INTERFACEKEY_HOTKEY_CRAFTS_MAT_CRAFTS].name="Hotkey: Crafts, Mat Crafts";
	interfacekey[INTERFACEKEY_HOTKEY_CRAFTS_MAT_GOBLET].name="Hotkey: Crafts, Mat Goblet";
	interfacekey[INTERFACEKEY_HOTKEY_CRAFTS_MAT_INSTRUMENT].name="Hotkey: Crafts, Mat Instrument";
	interfacekey[INTERFACEKEY_HOTKEY_CRAFTS_MAT_TOY].name="Hotkey: Crafts, Mat Toy";
	interfacekey[INTERFACEKEY_HOTKEY_SMITH_WEAPON].name="Hotkey: Forge, Weapon";
	interfacekey[INTERFACEKEY_HOTKEY_SMITH_ARMOR].name="Hotkey: Forge, Armor";
	interfacekey[INTERFACEKEY_HOTKEY_SMITH_FURNITURE].name="Hotkey: Forge, Furniture";
	interfacekey[INTERFACEKEY_HOTKEY_SMITH_SIEGE].name="Hotkey: Forge, Siege";
	interfacekey[INTERFACEKEY_HOTKEY_SMITH_TRAP].name="Hotkey: Forge, Trap";
	interfacekey[INTERFACEKEY_HOTKEY_SMITH_OTHER].name="Hotkey: Forge, Other";
	interfacekey[INTERFACEKEY_HOTKEY_SMITH_METAL].name="Hotkey: Forge, Metal Clothing";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_ARMORSTAND].name="Hotkey: Building, Armorstand";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_BED].name="Hotkey: Building, Bed";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_CHAIR].name="Hotkey: Building, Chair";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_COFFIN].name="Hotkey: Building, Coffin";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_DOOR].name="Hotkey: Building, Door";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_FLOODGATE].name="Hotkey: Building, Floodgate";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_HATCH].name="Hotkey: Building, Hatch";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_GRATE_WALL].name="Hotkey: Building, Wall Grate";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_GRATE_FLOOR].name="Hotkey: Building, Floor Grate";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_BARS_VERTICAL].name="Hotkey: Building, Vertical Bars";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_BARS_FLOOR].name="Hotkey: Building, Floor Bars";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_CABINET].name="Hotkey: Building, Cabinet";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_BOX].name="Hotkey: Building, Chest";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_KENNEL].name="Hotkey: Building, Kennel";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_FARMPLOT].name="Hotkey: Building, Farm Plot";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WEAPONRACK].name="Hotkey: Building, Weaponrack";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_STATUE].name="Hotkey: Building, Statue";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_TABLE].name="Hotkey: Building, Table";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_ROAD_DIRT].name="Hotkey: Building, Dirt Road";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_ROAD_PAVED].name="Hotkey: Building, Paved Road";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_BRIDGE].name="Hotkey: Building, Bridge";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WELL].name="Hotkey: Building, Well";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_SIEGEENGINE].name="Hotkey: Building, Siege";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP].name="Hotkey: Building, Workshop";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_FURNACE].name="Hotkey: Building, Furnace";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WINDOW_GLASS].name="Hotkey: Building, Glass Window";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WINDOW_GEM].name="Hotkey: Building, Gem Window";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_SHOP].name="Hotkey: Building, Shop";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_ANIMALTRAP].name="Hotkey: Building, Animal Trap";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_CHAIN].name="Hotkey: Building, Chain";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_CAGE].name="Hotkey: Building, Cage";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_TRADEDEPOT].name="Hotkey: Building, Trade Depot";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_TRAP].name="Hotkey: Building, Trap";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_MACHINE].name="Hotkey: Building, Machine Component";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_SUPPORT].name="Hotkey: Building, Support";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_ARCHERYTARGET].name="Hotkey: Building, Archery Target";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_MACHINE_SCREW_PUMP].name="Hotkey: Building, Machine Component, Screw Pump";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_MACHINE_WATER_WHEEL].name="Hotkey: Building, Machine Component, Water Wheel";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_MACHINE_WINDMILL].name="Hotkey: Building, Machine Component, Windmill";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_MACHINE_GEAR_ASSEMBLY].name="Hotkey: Building, Machine Component, Gear Assembly";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_MACHINE_AXLE_HORIZONTAL].name="Hotkey: Building, Machine Component, Horizontal Axle";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_MACHINE_AXLE_VERTICAL].name="Hotkey: Building, Machine Component, Vertical Axle";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_SIEGEENGINE_BALLISTA].name="Hotkey: Building, Siege, Ballista";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_SIEGEENGINE_CATAPULT].name="Hotkey: Building, Siege, Catapult";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_TRAP_STONE].name="Hotkey: Building, Trap, Stone";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_TRAP_WEAPON].name="Hotkey: Building, Trap, Weapon";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_TRAP_LEVER].name="Hotkey: Building, Trap, Lever";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_TRAP_TRIGGER].name="Hotkey: Building, Trap, Trigger";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_TRAP_CAGE].name="Hotkey: Building, Trap, Cage";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_TRAP_SPIKE].name="Hotkey: Building, Trap, Upright Spear/Spike";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_CONSTRUCTION].name="Hotkey: Building, Wall/Floor/Stairs";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_CONSTRUCTION_FORTIFICATION].name="Hotkey: Building, Construction, Fortification";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_CONSTRUCTION_WALL].name="Hotkey: Building, Construction, Wall";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_CONSTRUCTION_FLOOR].name="Hotkey: Building, Construction, Floor";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_CONSTRUCTION_RAMP].name="Hotkey: Building, Construction, Ramp";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_CONSTRUCTION_STAIR_UP].name="Hotkey: Building, Construction, Upward Stair";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_CONSTRUCTION_STAIR_DOWN].name="Hotkey: Building, Construction, Downward Stair";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_CONSTRUCTION_STAIR_UPDOWN].name="Hotkey: Building, Construction, Up/Down Stair";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_LEATHER].name="Hotkey: Building, Wksp, Leather";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_QUERN].name="Hotkey: Building, Wksp, Quern";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_MILLSTONE].name="Hotkey: Building, Wksp, Millstone";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_LOOM].name="Hotkey: Building, Wksp, Loom";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_CLOTHES].name="Hotkey: Building, Wksp, Clothes";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_BOWYER].name="Hotkey: Building, Wksp, Bowyer";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_CARPENTER].name="Hotkey: Building, Wksp, Carpenter";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_METALSMITH].name="Hotkey: Building, Wksp, Metalsmith";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_LAVAMILL].name="Hotkey: Building, Wksp, Lavamill";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_JEWELER].name="Hotkey: Building, Wksp, Jeweler";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_MASON].name="Hotkey: Building, Wksp, Mason";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_BUTCHER].name="Hotkey: Building, Wksp, Butcher";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_TANNER].name="Hotkey: Building, Wksp, Tanner";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_DYER].name="Hotkey: Building, Wksp, Dyer";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_CRAFTSMAN].name="Hotkey: Building, Wksp, Craftsman";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_SIEGE].name="Hotkey: Building, Wksp, Siege";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_MECHANIC].name="Hotkey: Building, Wksp, Mechanic";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_STILL].name="Hotkey: Building, Wksp, Still";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_FARMER].name="Hotkey: Building, Wksp, Farmer";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_KITCHEN].name="Hotkey: Building, Wksp, Kitchen";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_FISHERY].name="Hotkey: Building, Wksp, Fishery";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_LAB].name="Hotkey: Building, Wksp, Lab";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_ASHERY].name="Hotkey: Building, Wksp, Ashery";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_FURNACE_WOOD].name="Hotkey: Building, Furn, Wood";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_FURNACE_SMELTER].name="Hotkey: Building, Furn, Smelter";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_FURNACE_GLASS].name="Hotkey: Building, Furn, Glass";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_FURNACE_KILN].name="Hotkey: Building, Furn, Kiln";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_FURNACE_SMELTER_LAVA].name="Hotkey: Building, Furn, Smelter";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_FURNACE_GLASS_LAVA].name="Hotkey: Building, Furn, Glass";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_FURNACE_KILN_LAVA].name="Hotkey: Building, Furn, Kiln";
	interfacekey[INTERFACEKEY_D_ONESTEP].name="Main: One-Step";
	interfacekey[INTERFACEKEY_D_PAUSE].name="Main: Pause/Resume";
	interfacekey[INTERFACEKEY_D_DEPOT].name="Depot Access";
	interfacekey[INTERFACEKEY_D_HOT_KEYS].name="Main: Hot Keys";
	interfacekey[INTERFACEKEY_D_HOTKEY1].name="Main: Hot Key 1";
	interfacekey[INTERFACEKEY_D_HOTKEY2].name="Main: Hot Key 2";
	interfacekey[INTERFACEKEY_D_HOTKEY3].name="Main: Hot Key 3";
	interfacekey[INTERFACEKEY_D_HOTKEY4].name="Main: Hot Key 4";
	interfacekey[INTERFACEKEY_D_HOTKEY5].name="Main: Hot Key 5";
	interfacekey[INTERFACEKEY_D_HOTKEY6].name="Main: Hot Key 6";
	interfacekey[INTERFACEKEY_D_HOTKEY7].name="Main: Hot Key 7";
	interfacekey[INTERFACEKEY_D_HOTKEY8].name="Main: Hot Key 8";
	interfacekey[INTERFACEKEY_D_HOTKEY9].name="Main: Hot Key 9";
	interfacekey[INTERFACEKEY_D_HOTKEY10].name="Main: Hot Key 10";
	interfacekey[INTERFACEKEY_D_HOTKEY11].name="Main: Hot Key 11";
	interfacekey[INTERFACEKEY_D_HOTKEY12].name="Main: Hot Key 12";
	interfacekey[INTERFACEKEY_D_HOTKEY13].name="Main: Hot Key 13";
	interfacekey[INTERFACEKEY_D_HOTKEY14].name="Main: Hot Key 14";
	interfacekey[INTERFACEKEY_D_HOTKEY15].name="Main: Hot Key 15";
	interfacekey[INTERFACEKEY_D_HOTKEY16].name="Main: Hot Key 16";
	interfacekey[INTERFACEKEY_D_HOTKEY_CHANGE_NAME].name="Hot Keys: Change Name";
	interfacekey[INTERFACEKEY_D_HOTKEY_ZOOM].name="Hot Keys: Zoom";
	interfacekey[INTERFACEKEY_D_ANNOUNCE].name="Main: Announcements";
	interfacekey[INTERFACEKEY_D_BUILDING].name="Main: Place Building";
	interfacekey[INTERFACEKEY_D_CIVLIST].name="Main: Civilizations";
	interfacekey[INTERFACEKEY_D_DESIGNATE].name="Main: Designations";
	interfacekey[INTERFACEKEY_D_ARTLIST].name="Main: Artifacts";
	interfacekey[INTERFACEKEY_D_NOBLES].name="Main: Nobles";
	interfacekey[INTERFACEKEY_D_ORDERS].name="Main: Standing Orders";
	interfacekey[INTERFACEKEY_D_MILITARY].name="Main: Military";
	interfacekey[INTERFACEKEY_D_ROOMS].name="Main: Rooms";
	interfacekey[INTERFACEKEY_BUILDINGLIST_ZOOM_T].name="Building List: Zoom T";
	interfacekey[INTERFACEKEY_BUILDINGLIST_ZOOM_Q].name="Building List: Zoom Q";
	interfacekey[INTERFACEKEY_D_SQUADS].name="Main: Control Squads";
	interfacekey[INTERFACEKEY_D_STOCKPILES].name="Main: Stockpiles";
	interfacekey[INTERFACEKEY_D_CIVZONE].name="Main: Activity Zone";
	interfacekey[INTERFACEKEY_D_VIEWUNIT].name="Main: View Units";
	interfacekey[INTERFACEKEY_D_JOBLIST].name="Main: Job List";
	interfacekey[INTERFACEKEY_D_UNITLIST].name="Main: Unit List";
	interfacekey[INTERFACEKEY_D_LOOK].name="Main: Look";
	interfacekey[INTERFACEKEY_D_NOTE].name="Main: Note";
	interfacekey[INTERFACEKEY_D_NOTE_PLACE].name="Main: Note, Place";
	interfacekey[INTERFACEKEY_D_NOTE_DELETE].name="Main: Note, Delete";
	interfacekey[INTERFACEKEY_D_NOTE_ENTER].name="Main: Note, Enter Text";
	interfacekey[INTERFACEKEY_D_NOTE_ADOPT_SYMBOL].name="Main: Note, Adopt Symbol";
	interfacekey[INTERFACEKEY_D_NOTE_CHANGE_SELECTION].name="Main: Note, Change Selection";
	interfacekey[INTERFACEKEY_D_BUILDJOB].name="Main: Building Jobs";
	interfacekey[INTERFACEKEY_D_STATUS].name="Main: Overall Status";
	interfacekey[INTERFACEKEY_D_BUILDITEM].name="Main: Building Items";
	interfacekey[INTERFACEKEY_D_VISUALIZE].name="Main: Visualize";
	interfacekey[INTERFACEKEY_D_BITEM_FORBID].name="Building Items: Forbid";
	interfacekey[INTERFACEKEY_D_BITEM_DUMP].name="Building Items: Dump";
	interfacekey[INTERFACEKEY_D_BITEM_MELT].name="Building Items: Melt";
	interfacekey[INTERFACEKEY_D_BITEM_HIDE].name="Building Items: Hide";
	interfacekey[INTERFACEKEY_D_LOOK_FORBID].name="Dwf Look: Forbid";
	interfacekey[INTERFACEKEY_D_LOOK_DUMP].name="Dwf Look: Dump";
	interfacekey[INTERFACEKEY_D_LOOK_MELT].name="Dwf Look: Melt";
	interfacekey[INTERFACEKEY_D_LOOK_HIDE].name="Dwf Look: Hide";
	interfacekey[INTERFACEKEY_A_ENTER_NAME].name="Adventure: Creation, Name Entry";
	interfacekey[INTERFACEKEY_A_CUST_NAME].name="Adventure: Creation, Customize Name";
	interfacekey[INTERFACEKEY_A_RANDOM_NAME].name="Adventure: Creation, Random Name";
	interfacekey[INTERFACEKEY_A_CHANGE_GENDER].name="Adventure: Creation, Change Gender";
	interfacekey[INTERFACEKEY_A_END_TRAVEL].name="Adventure: Travel, Visit Site";
	interfacekey[INTERFACEKEY_A_TRAVEL_CLOUDS].name="Adventure: Travel, Clouds";
	interfacekey[INTERFACEKEY_A_LOG].name="Adventure: Log";
	interfacekey[INTERFACEKEY_A_TRAVEL_LOG].name="Adventure: Travel, Log";
	interfacekey[INTERFACEKEY_A_LOG_TASKS].name="Adventure: Log, Tasks";
	interfacekey[INTERFACEKEY_A_LOG_ENTITIES].name="Adventure: Log, Entities";
	interfacekey[INTERFACEKEY_A_LOG_SITES].name="Adventure: Log, Sites";
	interfacekey[INTERFACEKEY_A_LOG_SUBREGIONS].name="Adventure: Log, Regions";
	interfacekey[INTERFACEKEY_A_LOG_ZOOM_CURRENT_LOCATION].name="Adventure: Log, Zoom to Current Location";
	interfacekey[INTERFACEKEY_A_LOG_ZOOM_SELECTED].name="Adventure: Log, Zoom to Selected";
	interfacekey[INTERFACEKEY_A_LOG_LINE].name="Adventure: Log, Toggle Line";
	interfacekey[INTERFACEKEY_A_LOG_MAP].name="Adventure: Log, Toggle Map/Info";
	interfacekey[INTERFACEKEY_ORDERS_AUTOFORBID].name="Orders: Forbid";
	interfacekey[INTERFACEKEY_ORDERS_FORBID_PROJECTILE].name="Orders: Forbid Projectiles";
	interfacekey[INTERFACEKEY_ORDERS_FORBID_YOUR_CORPSE].name="Orders: Forbid Your Corpse";
	interfacekey[INTERFACEKEY_ORDERS_FORBID_YOUR_ITEMS].name="Orders: Forbid Your Items";
	interfacekey[INTERFACEKEY_ORDERS_FORBID_OTHER_CORPSE].name="Orders: Forbid Other Corpse";
	interfacekey[INTERFACEKEY_ORDERS_FORBID_OTHER_ITEMS].name="Orders: Forbid Other Items";
	interfacekey[INTERFACEKEY_ORDERS_REFUSE_GATHER].name="Orders: Gather Refuse";
	interfacekey[INTERFACEKEY_ORDERS_REFUSE_OUTSIDE].name="Orders: Gather Outside";
	interfacekey[INTERFACEKEY_ORDERS_REFUSE_DUMP_CORPSE].name="Orders: Dump Corpse";
	interfacekey[INTERFACEKEY_ORDERS_REFUSE_DUMP_SKULL].name="Orders: Dump Skull";
	interfacekey[INTERFACEKEY_ORDERS_REFUSE_DUMP_SKIN].name="Orders: Dump Skin";
	interfacekey[INTERFACEKEY_ORDERS_REFUSE_DUMP_BONE].name="Orders: Dump Bone";
	interfacekey[INTERFACEKEY_ORDERS_REFUSE_DUMP_SHELL].name="Orders: Dump Shell";
	interfacekey[INTERFACEKEY_ORDERS_REFUSE_DUMP_OTHER].name="Orders: Dump Other";
	interfacekey[INTERFACEKEY_ORDERS_GATHER_FURNITURE].name="Orders: Gather Furniture";
	interfacekey[INTERFACEKEY_ORDERS_GATHER_ANIMALS].name="Orders: Gather Animals";
	interfacekey[INTERFACEKEY_ORDERS_GATHER_FOOD].name="Orders: Gather Food";
	interfacekey[INTERFACEKEY_ORDERS_GATHER_BODIES].name="Orders: Gather Bodies";
	interfacekey[INTERFACEKEY_ORDERS_REFUSE].name="Orders: Refuse";
	interfacekey[INTERFACEKEY_ORDERS_GATHER_STONE].name="Orders: Gather Stone";
	interfacekey[INTERFACEKEY_ORDERS_GATHER_WOOD].name="Orders: Gather Wood";
	interfacekey[INTERFACEKEY_ORDERS_ALL_HARVEST].name="Orders: All Harvest";
	interfacekey[INTERFACEKEY_ORDERS_INDOORS].name="Orders: Stay Indoors";
	interfacekey[INTERFACEKEY_ORDERS_SAMEPILE].name="Orders: Piles Same";
	interfacekey[INTERFACEKEY_ORDERS_MIXFOODS].name="Orders: Mix Foods";
	interfacekey[INTERFACEKEY_ORDERS_EXCEPTIONS].name="Orders: Exceptions";
	interfacekey[INTERFACEKEY_ORDERS_LOOM].name="Orders: Loom";
	interfacekey[INTERFACEKEY_ORDERS_DYED_CLOTH].name="Orders: Dyed Cloth";
	interfacekey[INTERFACEKEY_ORDERS_WORKSHOP].name="Orders: Workshop";
	interfacekey[INTERFACEKEY_ORDERS_COLLECT_WEB].name="Orders: Collect Web";
	interfacekey[INTERFACEKEY_ORDERS_SLAUGHTER].name="Orders: Slaughter";
	interfacekey[INTERFACEKEY_ORDERS_BUTCHER].name="Orders: Butcher";
	interfacekey[INTERFACEKEY_ORDERS_TAN].name="Orders: Tan";
	interfacekey[INTERFACEKEY_ORDERS_ZONE].name="Orders: Zone";
	interfacekey[INTERFACEKEY_ORDERS_ZONE_DRINKING].name="Orders: Zone, Drinking";
	interfacekey[INTERFACEKEY_ORDERS_ZONE_FISHING].name="Orders: Zone, Fishing";
	interfacekey[INTERFACEKEY_DESTROYBUILDING].name="Destroy Building";
	interfacekey[INTERFACEKEY_SUSPENDBUILDING].name="Suspend Building";
	interfacekey[INTERFACEKEY_MENU_CONFIRM].name="Menu Confirm";
	interfacekey[INTERFACEKEY_SAVE_BINDINGS].name="Save Bindings";
	interfacekey[INTERFACEKEY_CHANGE_BINDINGS].name="Change Bindings";
	interfacekey[INTERFACEKEY_HOTKEY_ALCHEMIST_SOAP].name="Hotkey: Alchemist, Soap";
	interfacekey[INTERFACEKEY_HOTKEY_STILL_BREW].name="Hotkey: Still, Brew";
	interfacekey[INTERFACEKEY_HOTKEY_STILL_EXTRACT].name="Hotkey: Still, Extract";
	interfacekey[INTERFACEKEY_HOTKEY_LOOM_COLLECT_SILK].name="Hotkey: Loom, Collect Silk";
	interfacekey[INTERFACEKEY_HOTKEY_LOOM_WEAVE_CLOTH].name="Hotkey: Loom, Weave Cloth";
	interfacekey[INTERFACEKEY_HOTKEY_LOOM_WEAVE_SILK].name="Hotkey: Loom, Weave Silk";
	interfacekey[INTERFACEKEY_HOTKEY_LOOM_WEAVE_METAL].name="Hotkey: Loom, Metal";
	interfacekey[INTERFACEKEY_HOTKEY_KITCHEN_COOK_2].name="Hotkey: Kitchen, Meal 1";
	interfacekey[INTERFACEKEY_HOTKEY_KITCHEN_COOK_3].name="Hotkey: Kitchen, Meal 2";
	interfacekey[INTERFACEKEY_HOTKEY_KITCHEN_COOK_4].name="Hotkey: Kitchen, Meal 3";
	interfacekey[INTERFACEKEY_HOTKEY_KITCHEN_RENDER_FAT].name="Hotkey: Kitchen, Render Fat";
	interfacekey[INTERFACEKEY_HOTKEY_FARMER_PROCESS].name="Hotkey: Farmer, Process";
	interfacekey[INTERFACEKEY_HOTKEY_FARMER_PROCESS_VIAL].name="Hotkey: Farmer, Vial";
	interfacekey[INTERFACEKEY_HOTKEY_FARMER_PROCESS_BAG].name="Hotkey: Farmer, Bag";
	interfacekey[INTERFACEKEY_HOTKEY_FARMER_PROCESS_BARREL].name="Hotkey: Farmer, Barrel";
	interfacekey[INTERFACEKEY_HOTKEY_FARMER_CHEESE].name="Hotkey: Farmer, Cheese";
	interfacekey[INTERFACEKEY_HOTKEY_FARMER_MILK].name="Hotkey: Farmer, Milk";
	interfacekey[INTERFACEKEY_HOTKEY_MILL_MILL].name="Hotkey: Mill, Mill";
	interfacekey[INTERFACEKEY_HOTKEY_KENNEL_TRAIN_HUNT].name="Hotkey: Kennel, Hunt";
	interfacekey[INTERFACEKEY_HOTKEY_KENNEL_CATCH_VERMIN].name="Hotkey: Kennel, Catch";
	interfacekey[INTERFACEKEY_HOTKEY_KENNEL_TAME_VERMIN].name="Hotkey: Kennel, Tame Small";
	interfacekey[INTERFACEKEY_HOTKEY_KENNEL_TAME_UNIT].name="Hotkey: Kennel, Tame Large";
	interfacekey[INTERFACEKEY_HOTKEY_KENNEL_TRAIN_WAR].name="Hotkey: Kennel, War";
	interfacekey[INTERFACEKEY_HOTKEY_FISHERY_PROCESS].name="Hotkey: Fishery, Process";
	interfacekey[INTERFACEKEY_HOTKEY_FISHERY_EXTRACT].name="Hotkey: Fishery, Extract";
	interfacekey[INTERFACEKEY_HOTKEY_FISHERY_CATCH].name="Hotkey: Fishery, Catch";
	interfacekey[INTERFACEKEY_HOTKEY_BUTCHER_BUTCHER].name="Hotkey: Butcher, Butcher";
	interfacekey[INTERFACEKEY_HOTKEY_BUTCHER_EXTRACT].name="Hotkey: Butcher, Extract";
	interfacekey[INTERFACEKEY_HOTKEY_BUTCHER_CATCH].name="Hotkey: Butcher, Catch";
	interfacekey[INTERFACEKEY_HOTKEY_TANNER_TAN].name="Hotkey: Tanner, Tan";
	interfacekey[INTERFACEKEY_HOTKEY_DYER_THREAD].name="Hotkey: Dyer, Thread";
	interfacekey[INTERFACEKEY_HOTKEY_DYER_CLOTH].name="Hotkey: Dyer, Cloth";
	interfacekey[INTERFACEKEY_HOTKEY_JEWELER_FURNITURE].name="Hotkey: Jeweler, Furniture";
	interfacekey[INTERFACEKEY_HOTKEY_JEWELER_FINISHED].name="Hotkey: Jeweler, Finished";
	interfacekey[INTERFACEKEY_HOTKEY_JEWELER_AMMO].name="Hotkey: Jeweler, Ammo";
	interfacekey[INTERFACEKEY_HOTKEY_JEWELER_CUT].name="Hotkey: Jeweler, Cut";
	interfacekey[INTERFACEKEY_HOTKEY_JEWELER_ENCRUST].name="Hotkey: Jeweler, Encrust";
	interfacekey[INTERFACEKEY_HOTKEY_MECHANIC_PARTS].name="Hotkey: Mechanic, Mechanisms";
	interfacekey[INTERFACEKEY_HOTKEY_MASON_ARMORSTAND].name="Hotkey: Mason, Armorstand";
	interfacekey[INTERFACEKEY_HOTKEY_MASON_BLOCKS].name="Hotkey: Mason, Blocks";
	interfacekey[INTERFACEKEY_HOTKEY_MASON_CHAIR].name="Hotkey: Mason, Chair";
	interfacekey[INTERFACEKEY_HOTKEY_MASON_COFFIN].name="Hotkey: Mason, Coffin";
	interfacekey[INTERFACEKEY_HOTKEY_MASON_DOOR].name="Hotkey: Mason, Door";
	interfacekey[INTERFACEKEY_HOTKEY_MASON_FLOODGATE].name="Hotkey: Mason, Floodgate";
	interfacekey[INTERFACEKEY_HOTKEY_MASON_HATCH_COVER].name="Hotkey: Mason, Hatch Cover";
	interfacekey[INTERFACEKEY_HOTKEY_MASON_GRATE].name="Hotkey: Mason, Grate";
	interfacekey[INTERFACEKEY_HOTKEY_MASON_CABINET].name="Hotkey: Mason, Cabinet";
	interfacekey[INTERFACEKEY_HOTKEY_MASON_BOX].name="Hotkey: Mason, Box";
	interfacekey[INTERFACEKEY_HOTKEY_MASON_STATUE].name="Hotkey: Mason, Statue";
	interfacekey[INTERFACEKEY_HOTKEY_MASON_QUERN].name="Hotkey: Mason, Quern";
	interfacekey[INTERFACEKEY_HOTKEY_MASON_MILLSTONE].name="Hotkey: Mason, Millstone";
	interfacekey[INTERFACEKEY_HOTKEY_MASON_TABLE].name="Hotkey: Mason, Table";
	interfacekey[INTERFACEKEY_HOTKEY_MASON_WEAPONRACK].name="Hotkey: Mason, Weaponrack";
	interfacekey[INTERFACEKEY_HOTKEY_TRAP_BRIDGE].name="Hotkey: Trap, Bridge";
	interfacekey[INTERFACEKEY_HOTKEY_TRAP_GEAR_ASSEMBLY].name="Hotkey: Trap, Gear Assembly";
	interfacekey[INTERFACEKEY_HOTKEY_TRAP_DOOR].name="Hotkey: Trap, Door";
	interfacekey[INTERFACEKEY_HOTKEY_TRAP_FLOODGATE].name="Hotkey: Trap, Floodgate";
	interfacekey[INTERFACEKEY_HOTKEY_TRAP_SPIKE].name="Hotkey: Trap, Spike";
	interfacekey[INTERFACEKEY_HOTKEY_TRAP_HATCH].name="Hotkey: Trap, Hatch";
	interfacekey[INTERFACEKEY_HOTKEY_TRAP_GRATE_WALL].name="Hotkey: Trap, Wall Grate";
	interfacekey[INTERFACEKEY_HOTKEY_TRAP_GRATE_FLOOR].name="Hotkey: Trap, Floor Grate";
	interfacekey[INTERFACEKEY_HOTKEY_TRAP_BARS_VERTICAL].name="Hotkey: Trap, Vertical Bars";
	interfacekey[INTERFACEKEY_HOTKEY_TRAP_BARS_FLOOR].name="Hotkey: Trap, Floor Bars";
	interfacekey[INTERFACEKEY_HOTKEY_TRAP_SUPPORT].name="Hotkey: Trap, Support";
	interfacekey[INTERFACEKEY_HOTKEY_TRAP_CHAIN].name="Hotkey: Trap, Chain";
	interfacekey[INTERFACEKEY_HOTKEY_TRAP_CAGE].name="Hotkey: Trap, Cage";
	interfacekey[INTERFACEKEY_HOTKEY_TRAP_PULL_LEVER].name="Hotkey: Trap, Lever";
	interfacekey[INTERFACEKEY_BUILDJOB_ADD].name="Buildjob: Add";
	interfacekey[INTERFACEKEY_BUILDJOB_CANCEL].name="Buildjob: Cancel";
	interfacekey[INTERFACEKEY_BUILDJOB_PROMOTE].name="Buildjob: Promote";
	interfacekey[INTERFACEKEY_BUILDJOB_REPEAT].name="Buildjob: Repeat";
	interfacekey[INTERFACEKEY_BUILDJOB_SUSPEND].name="Buildjob: Suspend";
	interfacekey[INTERFACEKEY_BUILDJOB_WORKSHOP_PROFILE].name="Buildjob: Workshop Profile";
	interfacekey[INTERFACEKEY_BUILDJOB_WELL_FREE].name="Buildjob: Well, Free";
	interfacekey[INTERFACEKEY_BUILDJOB_WELL_SIZE].name="Buildjob: Well, Size";
	interfacekey[INTERFACEKEY_BUILDJOB_TARGET_FREE].name="Buildjob: Target, Free";
	interfacekey[INTERFACEKEY_BUILDJOB_TARGET_SIZE].name="Buildjob: Target, Size";
	interfacekey[INTERFACEKEY_BUILDJOB_TARGET_DOWN].name="Buildjob: Target, Down";
	interfacekey[INTERFACEKEY_BUILDJOB_TARGET_UP].name="Buildjob: Target, Up";
	interfacekey[INTERFACEKEY_BUILDJOB_TARGET_RIGHT].name="Buildjob: Target, Right";
	interfacekey[INTERFACEKEY_BUILDJOB_TARGET_LEFT].name="Buildjob: Target, Left";
	interfacekey[INTERFACEKEY_BUILDJOB_STATUE_ASSIGN].name="Buildjob: Statue, Assign";
	interfacekey[INTERFACEKEY_BUILDJOB_STATUE_FREE].name="Buildjob: Statue, Free";
	interfacekey[INTERFACEKEY_BUILDJOB_STATUE_SIZE].name="Buildjob: Statue, Size";
	interfacekey[INTERFACEKEY_BUILDJOB_CAGE_JUSTICE].name="Buildjob: Cage, Justice";
	interfacekey[INTERFACEKEY_BUILDJOB_CAGE_FREE].name="Buildjob: Cage, Free";
	interfacekey[INTERFACEKEY_BUILDJOB_CAGE_SIZE].name="Buildjob: Cage, Size";
	interfacekey[INTERFACEKEY_BUILDJOB_CAGE_ASSIGN_OCC].name="Buildjob: Cage, Assign Occ";
	interfacekey[INTERFACEKEY_BUILDJOB_CAGE_WATER].name="Buildjob: Cage, Water";
	interfacekey[INTERFACEKEY_BUILDJOB_CAGE_ASSIGN].name="Buildjob: Cage, Assign Owner";
	interfacekey[INTERFACEKEY_BUILDJOB_CHAIN_ASSIGN_OCC].name="Buildjob: Chain, Assign Occ";
	interfacekey[INTERFACEKEY_BUILDJOB_CHAIN_JUSTICE].name="Buildjob: Chain, Justice";
	interfacekey[INTERFACEKEY_BUILDJOB_CHAIN_ASSIGN].name="Buildjob: Chain, Assign Owner";
	interfacekey[INTERFACEKEY_BUILDJOB_CHAIN_FREE].name="Buildjob: Chain, Free";
	interfacekey[INTERFACEKEY_BUILDJOB_CHAIN_SIZE].name="Buildjob: Chain, Size";
	interfacekey[INTERFACEKEY_BUILDJOB_SIEGE_FIRING].name="Buildjob: Siege, Fire";
	interfacekey[INTERFACEKEY_BUILDJOB_SIEGE_ORIENT].name="Buildjob: Siege, Orient";
	interfacekey[INTERFACEKEY_BUILDJOB_DOOR_INTERNAL].name="Buildjob: Door, Internal";
	interfacekey[INTERFACEKEY_BUILDJOB_DOOR_LOCK].name="Buildjob: Door, Forbid";
	interfacekey[INTERFACEKEY_BUILDJOB_DOOR_AJAR].name="Buildjob: Door, Pet-passable";
	interfacekey[INTERFACEKEY_BUILDJOB_COFFIN_ASSIGN].name="Buildjob: Coffin, Assign";
	interfacekey[INTERFACEKEY_BUILDJOB_COFFIN_FREE].name="Buildjob: Coffin, Free";
	interfacekey[INTERFACEKEY_BUILDJOB_COFFIN_SIZE].name="Buildjob: Coffin, Size";
	interfacekey[INTERFACEKEY_BUILDJOB_COFFIN_BURIAL].name="Buildjob: Coffin, Gen. Burial";
	interfacekey[INTERFACEKEY_BUILDJOB_COFFIN_CIV].name="Buildjob: Coffing, Allow Citizens";
	interfacekey[INTERFACEKEY_BUILDJOB_COFFIN_PET].name="Buildjob: Coffing, Allow Pets";
	interfacekey[INTERFACEKEY_BUILDJOB_CHAIR_ASSIGN].name="Buildjob: Chair, Assign";
	interfacekey[INTERFACEKEY_BUILDJOB_CHAIR_FREE].name="Buildjob: Chair, Free";
	interfacekey[INTERFACEKEY_BUILDJOB_CHAIR_SIZE].name="Buildjob: Chair, Size";
	interfacekey[INTERFACEKEY_BUILDJOB_TABLE_ASSIGN].name="Buildjob: Table, Assign";
	interfacekey[INTERFACEKEY_BUILDJOB_TABLE_HALL].name="Buildjob: Table, Hall";
	interfacekey[INTERFACEKEY_BUILDJOB_TABLE_FREE].name="Buildjob: Table, Free";
	interfacekey[INTERFACEKEY_BUILDJOB_TABLE_SIZE].name="Buildjob: Table, Size";
	interfacekey[INTERFACEKEY_BUILDJOB_BED_ASSIGN].name="Buildjob: Bed, Assign";
	interfacekey[INTERFACEKEY_BUILDJOB_BED_FREE].name="Buildjob: Bed, Free";
	interfacekey[INTERFACEKEY_BUILDJOB_BED_BARRACKS].name="Buildjob: Bed, Barracks";
	interfacekey[INTERFACEKEY_BUILDJOB_BED_RENT].name="Buildjob: Bed, Rent";
	interfacekey[INTERFACEKEY_BUILDJOB_BED_SIZE].name="Buildjob: Bed, Size";
	interfacekey[INTERFACEKEY_BUILDJOB_DEPOT_BRING].name="Buildjob: Depot, Bring";
	interfacekey[INTERFACEKEY_BUILDJOB_DEPOT_TRADE].name="Buildjob: Depot, Trade";
	interfacekey[INTERFACEKEY_BUILDJOB_DEPOT_REQUEST_TRADER].name="Buildjob: Depot, Request Trader";
	interfacekey[INTERFACEKEY_BUILDJOB_DEPOT_BROKER_ONLY].name="Buildjob: Depot, Broker Only";
	interfacekey[INTERFACEKEY_BUILDJOB_ANIMALTRAP_BAIT_NONE].name="Buildjob: Animal Trap, No Bait";
	interfacekey[INTERFACEKEY_BUILDJOB_ANIMALTRAP_BAIT_MEAT].name="Buildjob: Animal Trap, Meat Bait";
	interfacekey[INTERFACEKEY_BUILDJOB_ANIMALTRAP_BAIT_FISH].name="Buildjob: Animal Trap, Fish Bait";
	interfacekey[INTERFACEKEY_BUILDJOB_ANIMALTRAP_BAIT_GEM].name="Buildjob: Animal Trap, Gem Bait";
	interfacekey[INTERFACEKEY_BUILDJOB_FARM_FALLOW].name="Buildjob: Farm, Fallow";
	interfacekey[INTERFACEKEY_BUILDJOB_FARM_FERTILIZE].name="Buildjob: Farm, Fertilize";
	interfacekey[INTERFACEKEY_BUILDJOB_FARM_SEASFERT].name="Buildjob: Farm, Seas Fert";
	interfacekey[INTERFACEKEY_BUILDJOB_FARM_SPRING].name="Buildjob: Farm, Spring";
	interfacekey[INTERFACEKEY_BUILDJOB_FARM_SUMMER].name="Buildjob: Farm, Summer";
	interfacekey[INTERFACEKEY_BUILDJOB_FARM_AUTUMN].name="Buildjob: Farm, Autumn";
	interfacekey[INTERFACEKEY_BUILDJOB_FARM_WINTER].name="Buildjob: Farm, Winter";
	interfacekey[INTERFACEKEY_BUILDJOB_STOCKPILE_MASTER].name="Buildjob: Stockpile, Master";
	interfacekey[INTERFACEKEY_BUILDJOB_STOCKPILE_DELETE_CHILD].name="Buildjob: Stockpile, Delete Child";
	interfacekey[INTERFACEKEY_BUILDJOB_STOCKPILE_SETTINGS].name="Buildjob: Stockpile, Settings";
	interfacekey[INTERFACEKEY_BUILDJOB_STOCKPILE_BARREL_UP].name="Buildjob: Stockpile, Barrel Up";
	interfacekey[INTERFACEKEY_BUILDJOB_STOCKPILE_BARREL_DOWN].name="Buildjob: Stockpile, Barrel Down";
	interfacekey[INTERFACEKEY_BUILDJOB_STOCKPILE_BARREL_ZERO].name="Buildjob: Stockpile, Barrel Zero";
	interfacekey[INTERFACEKEY_BUILDJOB_STOCKPILE_BARREL_MAX].name="Buildjob: Stockpile, Barrel Max";
	interfacekey[INTERFACEKEY_BUILDJOB_STOCKPILE_BIN_UP].name="Buildjob: Stockpile, Bin Up";
	interfacekey[INTERFACEKEY_BUILDJOB_STOCKPILE_BIN_DOWN].name="Buildjob: Stockpile, Bin Down";
	interfacekey[INTERFACEKEY_BUILDJOB_STOCKPILE_BIN_ZERO].name="Buildjob: Stockpile, Bin Zero";
	interfacekey[INTERFACEKEY_BUILDJOB_STOCKPILE_BIN_MAX].name="Buildjob: Stockpile, Bin Max";
	interfacekey[INTERFACEKEY_BUILDJOB_RACK_MAT_BONE].name="Buildjob: Rack, Mat, Bone";
	interfacekey[INTERFACEKEY_BUILDJOB_RACK_MAT_BRONZE].name="Buildjob: Rack, Mat, Bronze";
	interfacekey[INTERFACEKEY_BUILDJOB_RACK_MAT_COPPER].name="Buildjob: Rack, Mat, Copper";
	interfacekey[INTERFACEKEY_BUILDJOB_RACK_MAT_IRON].name="Buildjob: Rack, Mat, Iron";
	interfacekey[INTERFACEKEY_BUILDJOB_RACK_MAT_STEEL].name="Buildjob: Rack, Mat, Steel";
	interfacekey[INTERFACEKEY_BUILDJOB_RACK_MAT_WOOD].name="Buildjob: Rack, Mat, Wood";
	interfacekey[INTERFACEKEY_BUILDJOB_RACK_MAT_METAL].name="Buildjob: Rack, Mat, Special Metal";
	interfacekey[INTERFACEKEY_BUILDJOB_STAND_MAT_BONE].name="Buildjob: Stand, Mat, Bone";
	interfacekey[INTERFACEKEY_BUILDJOB_STAND_MAT_LEATHER].name="Buildjob: Stand, Mat, Leather";
	interfacekey[INTERFACEKEY_BUILDJOB_STAND_MAT_COPPER].name="Buildjob: Stand, Mat, Copper";
	interfacekey[INTERFACEKEY_BUILDJOB_STAND_MAT_BRONZE].name="Buildjob: Stand, Mat, Bronze";
	interfacekey[INTERFACEKEY_BUILDJOB_STAND_MAT_STEEL].name="Buildjob: Stand, Mat, Steel";
	interfacekey[INTERFACEKEY_BUILDJOB_STAND_MAT_IRON].name="Buildjob: Stand, Mat, Iron";
	interfacekey[INTERFACEKEY_BUILDJOB_STAND_MAT_WOOD].name="Buildjob: Stand, Mat, Wood";
	interfacekey[INTERFACEKEY_BUILDJOB_STAND_MAT_METAL].name="Buildjob: Stand, Mat, Special Metal";
	interfacekey[INTERFACEKEY_BUILDJOB_RACKSTAND_ASSIGN].name="Buildjob: Rackstand, Assign";
	interfacekey[INTERFACEKEY_BUILDJOB_RACKSTAND_FREE].name="Buildjob: Rackstand, Free";
	interfacekey[INTERFACEKEY_BUILDJOB_RACKSTAND_SIZE].name="Buildjob: Rackstand, Size";
	interfacekey[INTERFACEKEY_BUILDJOB_RACKSTAND_ITEM].name="Buildjob: Rackstand, Item";
	interfacekey[INTERFACEKEY_BUILDJOB_RACKSTAND_MAT].name="Buildjob: Rackstand, Mat";
	interfacekey[INTERFACEKEY_BUILDJOB_RACKSTAND_DEFAULTS1].name="Buildjob: Rackstand, All Items";
	interfacekey[INTERFACEKEY_BUILDJOB_RACKSTAND_KILL1].name="Buildjob: Rackstand, No Items";
	interfacekey[INTERFACEKEY_BUILDJOB_RACKSTAND_DEFAULTS2].name="Buildjob: Rackstand, All Mats";
	interfacekey[INTERFACEKEY_BUILDJOB_RACKSTAND_KILL2].name="Buildjob: Rackstand, No Mats";
	interfacekey[INTERFACEKEY_STOCKPILE_SETTINGS_ENABLE].name="Stockpile Settings: Enable";
	interfacekey[INTERFACEKEY_STOCKPILE_SETTINGS_DISABLE].name="Stockpile Settings: Disable";
	interfacekey[INTERFACEKEY_STOCKPILE_SETTINGS_PERMIT_ALL].name="Stockpile Settings: Permit All";
	interfacekey[INTERFACEKEY_STOCKPILE_SETTINGS_FORBID_ALL].name="Stockpile Settings: Forbid All";
	interfacekey[INTERFACEKEY_STOCKPILE_SETTINGS_PERMIT_SUB].name="Stockpile Settings: Permit Sub";
	interfacekey[INTERFACEKEY_STOCKPILE_SETTINGS_FORBID_SUB].name="Stockpile Settings: Forbit Sub";
	interfacekey[INTERFACEKEY_STOCKPILE_SETTINGS_SPECIFIC1].name="Stockpile Settings: Specific 1";
	interfacekey[INTERFACEKEY_STOCKPILE_SETTINGS_SPECIFIC2].name="Stockpile Settings: Specific 2";
	interfacekey[INTERFACEKEY_SQUAD_LOCK].name="Squad: Lock";
	interfacekey[INTERFACEKEY_SQUAD_STATION].name="Squad: Station";
	interfacekey[INTERFACEKEY_SQUAD_CLEAR_PATROL].name="Squad: Clear Patrol";
	interfacekey[INTERFACEKEY_SQUAD_PATROL].name="Squad: Patrol";
	interfacekey[INTERFACEKEY_SQUAD_ZOOM].name="Squad: Zoom";
	interfacekey[INTERFACEKEY_MOVIE_RECORD].name="Movie: Record";
	interfacekey[INTERFACEKEY_MOVIE_PLAY].name="Movie: Play";
	interfacekey[INTERFACEKEY_MOVIE_SAVE].name="Movie: Save";
	interfacekey[INTERFACEKEY_MOVIE_LOAD].name="Movie: Load";
	interfacekey[INTERFACEKEY_ASSIGNTRADE_VIEW].name="Assign Trade: View";
	interfacekey[INTERFACEKEY_ASSIGNTRADE_STRING].name="Assign Trade: String";
	interfacekey[INTERFACEKEY_ASSIGNTRADE_EXCLUDE_PROHIBITED].name="Assign Trade: Exclude Prohibited";
	interfacekey[INTERFACEKEY_ASSIGNTRADE_PENDING].name="Assign Trade: Pending";
	interfacekey[INTERFACEKEY_ASSIGNTRADE_SORT].name="Assign Trade: Sort";
	interfacekey[INTERFACEKEY_NOBLELIST_REPLACE].name="Noble List: Replace";
	interfacekey[INTERFACEKEY_NOBLELIST_SETTINGS].name="Noble List: Settings";
	interfacekey[INTERFACEKEY_NOBLELIST_VIEW_CANDIDATE].name="Noble List: View Candidate";
	interfacekey[INTERFACEKEY_A_BARTER_VIEW].name="Adventure: Barter, View";
	interfacekey[INTERFACEKEY_A_BARTER_CURRENCY_1].name="Adventure: Barter, Currency 1";
	interfacekey[INTERFACEKEY_A_BARTER_CURRENCY_2].name="Adventure: Barter, Currency 2";
	interfacekey[INTERFACEKEY_A_BARTER_CURRENCY_3].name="Adventure: Barter, Currency 3";
	interfacekey[INTERFACEKEY_A_BARTER_CURRENCY_4].name="Adventure: Barter, Currency 4";
	interfacekey[INTERFACEKEY_A_BARTER_CURRENCY_5].name="Adventure: Barter, Currency 5";
	interfacekey[INTERFACEKEY_A_BARTER_CURRENCY_6].name="Adventure: Barter, Currency 6";
	interfacekey[INTERFACEKEY_A_BARTER_CURRENCY_7].name="Adventure: Barter, Currency 7";
	interfacekey[INTERFACEKEY_A_BARTER_CURRENCY_8].name="Adventure: Barter, Currency 8";
	interfacekey[INTERFACEKEY_A_BARTER_CURRENCY_9].name="Adventure: Barter, Currency 9";
	interfacekey[INTERFACEKEY_A_BARTER_TRADE].name="Adventure: Barter, Trade";
	interfacekey[INTERFACEKEY_TRADE_VIEW].name="Trade, View";
	interfacekey[INTERFACEKEY_TRADE_TRADE].name="Trade, Trade";
	interfacekey[INTERFACEKEY_TRADE_OFFER].name="Trade, Offer";
	interfacekey[INTERFACEKEY_TRADE_SEIZE].name="Trade, Seize";
	interfacekey[INTERFACEKEY_STORES_VIEW].name="Stores, View";
	interfacekey[INTERFACEKEY_STORES_FORBID].name="Stores, Forbid";
	interfacekey[INTERFACEKEY_STORES_MELT].name="Stores, Melt";
	interfacekey[INTERFACEKEY_STORES_DUMP].name="Stores, Dump";
	interfacekey[INTERFACEKEY_STORES_ZOOM].name="Stores, Zoom";
	interfacekey[INTERFACEKEY_STORES_HIDE].name="Stores, Hide";
	interfacekey[INTERFACEKEY_MILITARY_ACTIVATE].name="Military, Activate";
	interfacekey[INTERFACEKEY_MILITARY_VIEW].name="Military, View";
	interfacekey[INTERFACEKEY_MILITARY_WEAPON].name="Military, Weapon";
	interfacekey[INTERFACEKEY_MILITARY_ZOOM].name="Military, Zoom";
	interfacekey[INTERFACEKEY_SQUADINFO_FOOD].name="Squad Info, Food";
	interfacekey[INTERFACEKEY_SQUADINFO_SLEEP].name="Squad Info, Sleep";
	interfacekey[INTERFACEKEY_SQUADINFO_WATER].name="Squad Info, Water";
	interfacekey[INTERFACEKEY_SQUADINFO_STAYCLOSE].name="Squad Info, Stay Close";
	interfacekey[INTERFACEKEY_SQUADINFO_ATTACKWILDS].name="Squad Info, Attack Wilderness";
	interfacekey[INTERFACEKEY_SQUADINFO_STANDDOWN].name="Squad Info, Stand Down";
	interfacekey[INTERFACEKEY_SQUADINFO_ZOOM].name="Squad Info, Zoom";
	interfacekey[INTERFACEKEY_UNITJOB_REMOVE_CRE].name="Unitjob, Remove Unit";
	interfacekey[INTERFACEKEY_UNITJOB_ZOOM_CRE].name="Unitjob, Zoom Unit";
	interfacekey[INTERFACEKEY_UNITJOB_ZOOM_BUILD].name="Unitjob, Zoom Building";
	interfacekey[INTERFACEKEY_UNITJOB_VIEW].name="Unitjob, View";
	interfacekey[INTERFACEKEY_UNITJOB_MANAGER].name="Unitjob, Manager";
	interfacekey[INTERFACEKEY_MANAGER_NEW_ORDER].name="Manager, New Order";
	interfacekey[INTERFACEKEY_MANAGER_REMOVE].name="Manager, Remove";
	interfacekey[INTERFACEKEY_MANAGER_PROMOTE].name="Manager, Promote";
	interfacekey[INTERFACEKEY_MANAGER_MAX].name="Manager, Max";
	interfacekey[INTERFACEKEY_MANAGER_WAGES].name="Manager, Wages";
	interfacekey[INTERFACEKEY_PET_BUTCHER].name="Animals, Slaughter";
	interfacekey[INTERFACEKEY_KITCHEN_COOK].name="Kitchen, Cook";
	interfacekey[INTERFACEKEY_KITCHEN_BREW].name="Kitchen, Brew";
	interfacekey[INTERFACEKEY_SETUPGAME_NEW].name="Setup, New";
	interfacekey[INTERFACEKEY_SETUPGAME_VIEW].name="Setup, View";
	interfacekey[INTERFACEKEY_SETUPGAME_CUSTOMIZE_UNIT].name="Setup, Customize Unit";
	interfacekey[INTERFACEKEY_SETUPGAME_SAVE_PROFILE].name="Setup, Save Profile";
	interfacekey[INTERFACEKEY_SETUPGAME_SAVE_PROFILE_ABORT].name="Setup, Save Profile, Abort";
	interfacekey[INTERFACEKEY_SETUPGAME_SAVE_PROFILE_GO].name="Setup, Save Profile, Go";
	interfacekey[INTERFACEKEY_SETUPGAME_VIEW_PROFILE_PROBLEMS].name="Setup, View Profile Problems";
	interfacekey[INTERFACEKEY_CIVZONE_REMOVE].name="Main: Activity Zone, Remove";
	interfacekey[INTERFACEKEY_CIVZONE_WATER_SOURCE].name="Main: Activity Zone, Water Source";
	interfacekey[INTERFACEKEY_CIVZONE_DUMP].name="Main: Activity Zone, Garbage Dump";
	interfacekey[INTERFACEKEY_CIVZONE_POND].name="Main: Activity Zone, Pond";
	interfacekey[INTERFACEKEY_CIVZONE_SAND_COLLECT].name="Main: Activity Zone, Sand Collect";
	interfacekey[INTERFACEKEY_CIVZONE_ACTIVE].name="Main: Activity Zone, Active";
	interfacekey[INTERFACEKEY_CIVZONE_FISH].name="Main: Activity Zone, Fish";
	interfacekey[INTERFACEKEY_CIVZONE_MEETING].name="Main: Activity Zone, Meeting";
	interfacekey[INTERFACEKEY_CIVZONE_POND_OPTIONS].name="Main: Activity Zone, Pond Options";
	interfacekey[INTERFACEKEY_CIVZONE_POND_WATER].name="Main: Activity Zone: Pond, Water";
	interfacekey[INTERFACEKEY_STOCKPILE_ANIMAL].name="Stockpile, Animal";
	interfacekey[INTERFACEKEY_STOCKPILE_FOOD].name="Stockpile, Food";
	interfacekey[INTERFACEKEY_STOCKPILE_WEAPON].name="Stockpile, Weapon";
	interfacekey[INTERFACEKEY_STOCKPILE_ARMOR].name="Stockpile, Armor";
	interfacekey[INTERFACEKEY_STOCKPILE_CUSTOM].name="Stockpile, Custom";
	interfacekey[INTERFACEKEY_STOCKPILE_CUSTOM_SETTINGS].name="Stockpile, Custom Settings";
	interfacekey[INTERFACEKEY_STOCKPILE_FURNITURE].name="Stockpile, Furniture";
	interfacekey[INTERFACEKEY_STOCKPILE_GRAVEYARD].name="Stockpile, Graveyard";
	interfacekey[INTERFACEKEY_STOCKPILE_REFUSE].name="Stockpile, Refuse";
	interfacekey[INTERFACEKEY_STOCKPILE_WOOD].name="Stockpile, Wood";
	interfacekey[INTERFACEKEY_STOCKPILE_STONE].name="Stockpile, Stone";
	interfacekey[INTERFACEKEY_STOCKPILE_GEM].name="Stockpile, Gem";
	interfacekey[INTERFACEKEY_STOCKPILE_BARBLOCK].name="Stockpile, Bar/Block";
	interfacekey[INTERFACEKEY_STOCKPILE_CLOTH].name="Stockpile, Cloth";
	interfacekey[INTERFACEKEY_STOCKPILE_LEATHER].name="Stockpile, Leather";
	interfacekey[INTERFACEKEY_STOCKPILE_AMMO].name="Stockpile, Ammo";
	interfacekey[INTERFACEKEY_STOCKPILE_COINS].name="Stockpile, Coins";
	interfacekey[INTERFACEKEY_STOCKPILE_FINISHED].name="Stockpile, Finished";
	interfacekey[INTERFACEKEY_STOCKPILE_NONE].name="Stockpile, None";
	interfacekey[INTERFACEKEY_DESIGNATE_BITEM].name="Designate, Building/Item";
	interfacekey[INTERFACEKEY_DESIGNATE_CLAIM].name="Designate, Reclaim";
	interfacekey[INTERFACEKEY_DESIGNATE_UNCLAIM].name="Designate, Forbid";
	interfacekey[INTERFACEKEY_DESIGNATE_MELT].name="Designate, Melt";
	interfacekey[INTERFACEKEY_DESIGNATE_NO_MELT].name="Designate, No Melt";
	interfacekey[INTERFACEKEY_DESIGNATE_DUMP].name="Designate, Dump";
	interfacekey[INTERFACEKEY_DESIGNATE_NO_DUMP].name="Designate, No Dump";
	interfacekey[INTERFACEKEY_DESIGNATE_HIDE].name="Designate, Hide";
	interfacekey[INTERFACEKEY_DESIGNATE_NO_HIDE].name="Designate, No Hide";
	interfacekey[INTERFACEKEY_DESIGNATE_TRAFFIC].name="Designate, Traffic";
	interfacekey[INTERFACEKEY_DESIGNATE_TRAFFIC_HIGH].name="Designate, High Traffic";
	interfacekey[INTERFACEKEY_DESIGNATE_TRAFFIC_NORMAL].name="Designate, Normal Traffic";
	interfacekey[INTERFACEKEY_DESIGNATE_TRAFFIC_LOW].name="Designate, Low Traffic";
	interfacekey[INTERFACEKEY_DESIGNATE_TRAFFIC_RESTRICTED].name="Designate, Restricted Traffic";
	interfacekey[INTERFACEKEY_DESIGNATE_TRAFFIC_INCREASE_WEIGHT].name="Designate, Increase Weight";
	interfacekey[INTERFACEKEY_DESIGNATE_TRAFFIC_DECREASE_WEIGHT].name="Designate, Decrease Weight";
	interfacekey[INTERFACEKEY_DESIGNATE_TRAFFIC_INCREASE_WEIGHT_MORE].name="Designate, Increase Weight More";
	interfacekey[INTERFACEKEY_DESIGNATE_TRAFFIC_DECREASE_WEIGHT_MORE].name="Designate, Decrease Weight More";
	interfacekey[INTERFACEKEY_DESIGNATE_DIG].name="Designate, Dig";
	interfacekey[INTERFACEKEY_DESIGNATE_DIG_REMOVE_STAIRS_RAMPS].name="Designate, Dig Remove Stairs Ramps";
	interfacekey[INTERFACEKEY_DESIGNATE_STAIR_UP].name="Designate, U Stair";
	interfacekey[INTERFACEKEY_DESIGNATE_STAIR_DOWN].name="Designate, D Stair";
	interfacekey[INTERFACEKEY_DESIGNATE_STAIR_UPDOWN].name="Designate, UD Stair";
	interfacekey[INTERFACEKEY_DESIGNATE_RAMP].name="Designate, Ramp";
	interfacekey[INTERFACEKEY_DESIGNATE_CHANNEL].name="Designate, Channel";
	interfacekey[INTERFACEKEY_DESIGNATE_CHOP].name="Designate, Chop";
	interfacekey[INTERFACEKEY_DESIGNATE_PLANTS].name="Designate, Plants";
	interfacekey[INTERFACEKEY_DESIGNATE_SMOOTH].name="Designate, Smooth";
	interfacekey[INTERFACEKEY_DESIGNATE_ENGRAVE].name="Designate, Engrave";
	interfacekey[INTERFACEKEY_DESIGNATE_FORTIFY].name="Designate, Fortify";
	interfacekey[INTERFACEKEY_DESIGNATE_TOGGLE_ENGRAVING].name="Designate, Toggle Engraving";
	interfacekey[INTERFACEKEY_DESIGNATE_UNDO].name="Designate, Undo";
	interfacekey[INTERFACEKEY_DESIGNATE_REMOVE_CONSTRUCTION].name="Designate, Remove Construction";
	interfacekey[INTERFACEKEY_BUILDING_DIM_Y_UP].name="Building, Change Height +";
	interfacekey[INTERFACEKEY_BUILDING_DIM_Y_DOWN].name="Building, Change Height -";
	interfacekey[INTERFACEKEY_BUILDING_DIM_X_UP].name="Building, Change Width +";
	interfacekey[INTERFACEKEY_BUILDING_DIM_X_DOWN].name="Building, Change Width -";
	interfacekey[INTERFACEKEY_BUILDING_ORIENT_UP].name="Building, Orient Up";
	interfacekey[INTERFACEKEY_BUILDING_ORIENT_LEFT].name="Building, Orient Left";
	interfacekey[INTERFACEKEY_BUILDING_ORIENT_RIGHT].name="Building, Orient Right";
	interfacekey[INTERFACEKEY_BUILDING_ORIENT_DOWN].name="Building, Orient Down";
	interfacekey[INTERFACEKEY_BUILDING_ORIENT_NONE].name="Building, Orient None";
	interfacekey[INTERFACEKEY_BUILDING_VIEW_ITEM].name="Building, View Item";
	interfacekey[INTERFACEKEY_BUILDING_ADVANCE_STAGE].name="Building, Done Selecting";
	interfacekey[INTERFACEKEY_BUILDING_EXPAND_CONTRACT].name="Building, Expand/Contract";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_ENABLE_WATER].name="Building, Trigger, Enable Water";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_MIN_WATER_UP].name="Building, Trigger, Min Water Up";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_MIN_WATER_DOWN].name="Building, Trigger, Min Water Down";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_MAX_WATER_UP].name="Building, Trigger, Max Water Up";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_MAX_WATER_DOWN].name="Building, Trigger, Max Water Down";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_ENABLE_MAGMA].name="Building, Trigger, Enabler Magma";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_MIN_MAGMA_UP].name="Building, Trigger, Min Magma Up";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_MIN_MAGMA_DOWN].name="Building, Trigger, Min Magma Down";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_MAX_MAGMA_UP].name="Building, Trigger, Max Magma Up";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_MAX_MAGMA_DOWN].name="Building, Trigger, Max Magma Down";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_ENABLE_CREATURE].name="Building, Trigger, Enable Creature";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_ENABLE_LOCALS].name="Building, Trigger, Enable Locals";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_RESETS].name="Building, Trigger, Resets";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_MIN_SIZE_UP].name="Building, Trigger, Min Size Up";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_MIN_SIZE_DOWN].name="Building, Trigger, Min Size Down";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_MAX_SIZE_UP].name="Building, Trigger, Max Size Up";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_MAX_SIZE_DOWN].name="Building, Trigger, Max Size Down";
	interfacekey[INTERFACEKEY_UNITVIEW_GEN].name="Unitview, General";
	interfacekey[INTERFACEKEY_UNITVIEW_INV].name="Unitview, Inventory";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF].name="Unitview, Prefs";
	interfacekey[INTERFACEKEY_UNITVIEW_WND].name="Unitview, Wounds";
	interfacekey[INTERFACEKEY_UNITVIEW_NEXT].name="Unitview, Next Unit";
	interfacekey[INTERFACEKEY_UNITVIEW_SLAUGHTER].name="Unitview, Slaughter";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_PROF].name="Unitview, Prefs, Labor";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_PET].name="Unitview, Prefs, Pets";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_SOLDIER].name="Unitview, Prefs, Soldier";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_CASTLE].name="Unitview, Prefs, Fortress Guard";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_ROYAL].name="Unitview, Prefs, Royal Guard";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_TOGGLESQUAD].name="Unitview, Prefs, Toggle Activation";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_PET_HUNT].name="Unitview, Prefs, Pets, Hunting";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_PET_WAR].name="Unitview, Prefs, Pets, War";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_SOLDIER_UNARMED].name="Unitview, Prefs, Soldier, Unarmed";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_SOLDIER_AXE].name="Unitview, Prefs, Soldier, Axe";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_SOLDIER_CROSSBOW].name="Unitview, Prefs, Soldier, Crossbow";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_SOLDIER_HAMMER].name="Unitview, Prefs, Soldier, Hammer";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_SOLDIER_MACE].name="Unitview, Prefs, Soldier, Mace";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_SOLDIER_SPEAR].name="Unitview, Prefs, Soldier, Spear";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_SOLDIER_SWORD].name="Unitview, Prefs, Soldier, Sword";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_SOLDIER_WEAPONNUM].name="Unitview, Prefs, Soldier, Weapon Num";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_SOLDIER_ARMOR].name="Unitview, Prefs, Soldier, Armor";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_SOLDIER_SHIELD].name="Unitview, Prefs, Soldier, Shield";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_VIEW].name="Unitview, Prefs, Profile";

	interfacekey[INTERFACEKEY_STRING_A000].name="String: Backspace";
	interfacekey[INTERFACEKEY_STRING_A001].name="String: Character 001";
	interfacekey[INTERFACEKEY_STRING_A002].name="String: Character 002";
	interfacekey[INTERFACEKEY_STRING_A003].name="String: Character 003";
	interfacekey[INTERFACEKEY_STRING_A004].name="String: Character 004";
	interfacekey[INTERFACEKEY_STRING_A005].name="String: Character 005";
	interfacekey[INTERFACEKEY_STRING_A006].name="String: Character 006";
	interfacekey[INTERFACEKEY_STRING_A007].name="String: Character 007";
	interfacekey[INTERFACEKEY_STRING_A008].name="String: Character 008";
	interfacekey[INTERFACEKEY_STRING_A009].name="String: Character 009";
	interfacekey[INTERFACEKEY_STRING_A010].name="String: Character 010";
	interfacekey[INTERFACEKEY_STRING_A011].name="String: Character 011";
	interfacekey[INTERFACEKEY_STRING_A012].name="String: Character 012";
	interfacekey[INTERFACEKEY_STRING_A013].name="String: Character 013";
	interfacekey[INTERFACEKEY_STRING_A014].name="String: Character 014";
	interfacekey[INTERFACEKEY_STRING_A015].name="String: Character 015";
	interfacekey[INTERFACEKEY_STRING_A016].name="String: Character 016";
	interfacekey[INTERFACEKEY_STRING_A017].name="String: Character 017";
	interfacekey[INTERFACEKEY_STRING_A018].name="String: Character 018";
	interfacekey[INTERFACEKEY_STRING_A019].name="String: Character 019";
	interfacekey[INTERFACEKEY_STRING_A020].name="String: Character 020";
	interfacekey[INTERFACEKEY_STRING_A021].name="String: Character 021";
	interfacekey[INTERFACEKEY_STRING_A022].name="String: Character 022";
	interfacekey[INTERFACEKEY_STRING_A023].name="String: Character 023";
	interfacekey[INTERFACEKEY_STRING_A024].name="String: Character 024";
	interfacekey[INTERFACEKEY_STRING_A025].name="String: Character 025";
	interfacekey[INTERFACEKEY_STRING_A026].name="String: Character 026";
	interfacekey[INTERFACEKEY_STRING_A027].name="String: Character 027";
	interfacekey[INTERFACEKEY_STRING_A028].name="String: Character 028";
	interfacekey[INTERFACEKEY_STRING_A029].name="String: Character 029";
	interfacekey[INTERFACEKEY_STRING_A030].name="String: Character 030";
	interfacekey[INTERFACEKEY_STRING_A031].name="String: Character 031";
	interfacekey[INTERFACEKEY_STRING_A032].name="String: Character 032";
	interfacekey[INTERFACEKEY_STRING_A033].name="String: Character 033";
	interfacekey[INTERFACEKEY_STRING_A034].name="String: Character 034";
	interfacekey[INTERFACEKEY_STRING_A035].name="String: Character 035";
	interfacekey[INTERFACEKEY_STRING_A036].name="String: Character 036";
	interfacekey[INTERFACEKEY_STRING_A037].name="String: Character 037";
	interfacekey[INTERFACEKEY_STRING_A038].name="String: Character 038";
	interfacekey[INTERFACEKEY_STRING_A039].name="String: Character 039";
	interfacekey[INTERFACEKEY_STRING_A040].name="String: Character 040";
	interfacekey[INTERFACEKEY_STRING_A041].name="String: Character 041";
	interfacekey[INTERFACEKEY_STRING_A042].name="String: Character 042";
	interfacekey[INTERFACEKEY_STRING_A043].name="String: Character 043";
	interfacekey[INTERFACEKEY_STRING_A044].name="String: Character 044";
	interfacekey[INTERFACEKEY_STRING_A045].name="String: Character 045";
	interfacekey[INTERFACEKEY_STRING_A046].name="String: Character 046";
	interfacekey[INTERFACEKEY_STRING_A047].name="String: Character 047";
	interfacekey[INTERFACEKEY_STRING_A048].name="String: Character 048";
	interfacekey[INTERFACEKEY_STRING_A049].name="String: Character 049";
	interfacekey[INTERFACEKEY_STRING_A050].name="String: Character 050";
	interfacekey[INTERFACEKEY_STRING_A051].name="String: Character 051";
	interfacekey[INTERFACEKEY_STRING_A052].name="String: Character 052";
	interfacekey[INTERFACEKEY_STRING_A053].name="String: Character 053";
	interfacekey[INTERFACEKEY_STRING_A054].name="String: Character 054";
	interfacekey[INTERFACEKEY_STRING_A055].name="String: Character 055";
	interfacekey[INTERFACEKEY_STRING_A056].name="String: Character 056";
	interfacekey[INTERFACEKEY_STRING_A057].name="String: Character 057";
	interfacekey[INTERFACEKEY_STRING_A058].name="String: Character 058";
	interfacekey[INTERFACEKEY_STRING_A059].name="String: Character 059";
	interfacekey[INTERFACEKEY_STRING_A060].name="String: Character 060";
	interfacekey[INTERFACEKEY_STRING_A061].name="String: Character 061";
	interfacekey[INTERFACEKEY_STRING_A062].name="String: Character 062";
	interfacekey[INTERFACEKEY_STRING_A063].name="String: Character 063";
	interfacekey[INTERFACEKEY_STRING_A064].name="String: Character 064";
	interfacekey[INTERFACEKEY_STRING_A065].name="String: Character 065";
	interfacekey[INTERFACEKEY_STRING_A066].name="String: Character 066";
	interfacekey[INTERFACEKEY_STRING_A067].name="String: Character 067";
	interfacekey[INTERFACEKEY_STRING_A068].name="String: Character 068";
	interfacekey[INTERFACEKEY_STRING_A069].name="String: Character 069";
	interfacekey[INTERFACEKEY_STRING_A070].name="String: Character 070";
	interfacekey[INTERFACEKEY_STRING_A071].name="String: Character 071";
	interfacekey[INTERFACEKEY_STRING_A072].name="String: Character 072";
	interfacekey[INTERFACEKEY_STRING_A073].name="String: Character 073";
	interfacekey[INTERFACEKEY_STRING_A074].name="String: Character 074";
	interfacekey[INTERFACEKEY_STRING_A075].name="String: Character 075";
	interfacekey[INTERFACEKEY_STRING_A076].name="String: Character 076";
	interfacekey[INTERFACEKEY_STRING_A077].name="String: Character 077";
	interfacekey[INTERFACEKEY_STRING_A078].name="String: Character 078";
	interfacekey[INTERFACEKEY_STRING_A079].name="String: Character 079";
	interfacekey[INTERFACEKEY_STRING_A080].name="String: Character 080";
	interfacekey[INTERFACEKEY_STRING_A081].name="String: Character 081";
	interfacekey[INTERFACEKEY_STRING_A082].name="String: Character 082";
	interfacekey[INTERFACEKEY_STRING_A083].name="String: Character 083";
	interfacekey[INTERFACEKEY_STRING_A084].name="String: Character 084";
	interfacekey[INTERFACEKEY_STRING_A085].name="String: Character 085";
	interfacekey[INTERFACEKEY_STRING_A086].name="String: Character 086";
	interfacekey[INTERFACEKEY_STRING_A087].name="String: Character 087";
	interfacekey[INTERFACEKEY_STRING_A088].name="String: Character 088";
	interfacekey[INTERFACEKEY_STRING_A089].name="String: Character 089";
	interfacekey[INTERFACEKEY_STRING_A090].name="String: Character 090";
	interfacekey[INTERFACEKEY_STRING_A091].name="String: Character 091";
	interfacekey[INTERFACEKEY_STRING_A092].name="String: Character 092";
	interfacekey[INTERFACEKEY_STRING_A093].name="String: Character 093";
	interfacekey[INTERFACEKEY_STRING_A094].name="String: Character 094";
	interfacekey[INTERFACEKEY_STRING_A095].name="String: Character 095";
	interfacekey[INTERFACEKEY_STRING_A096].name="String: Character 096";
	interfacekey[INTERFACEKEY_STRING_A097].name="String: Character 097";
	interfacekey[INTERFACEKEY_STRING_A098].name="String: Character 098";
	interfacekey[INTERFACEKEY_STRING_A099].name="String: Character 099";
	interfacekey[INTERFACEKEY_STRING_A100].name="String: Character 100";
	interfacekey[INTERFACEKEY_STRING_A101].name="String: Character 101";
	interfacekey[INTERFACEKEY_STRING_A102].name="String: Character 102";
	interfacekey[INTERFACEKEY_STRING_A103].name="String: Character 103";
	interfacekey[INTERFACEKEY_STRING_A104].name="String: Character 104";
	interfacekey[INTERFACEKEY_STRING_A105].name="String: Character 105";
	interfacekey[INTERFACEKEY_STRING_A106].name="String: Character 106";
	interfacekey[INTERFACEKEY_STRING_A107].name="String: Character 107";
	interfacekey[INTERFACEKEY_STRING_A108].name="String: Character 108";
	interfacekey[INTERFACEKEY_STRING_A109].name="String: Character 109";
	interfacekey[INTERFACEKEY_STRING_A110].name="String: Character 110";
	interfacekey[INTERFACEKEY_STRING_A111].name="String: Character 111";
	interfacekey[INTERFACEKEY_STRING_A112].name="String: Character 112";
	interfacekey[INTERFACEKEY_STRING_A113].name="String: Character 113";
	interfacekey[INTERFACEKEY_STRING_A114].name="String: Character 114";
	interfacekey[INTERFACEKEY_STRING_A115].name="String: Character 115";
	interfacekey[INTERFACEKEY_STRING_A116].name="String: Character 116";
	interfacekey[INTERFACEKEY_STRING_A117].name="String: Character 117";
	interfacekey[INTERFACEKEY_STRING_A118].name="String: Character 118";
	interfacekey[INTERFACEKEY_STRING_A119].name="String: Character 119";
	interfacekey[INTERFACEKEY_STRING_A120].name="String: Character 120";
	interfacekey[INTERFACEKEY_STRING_A121].name="String: Character 121";
	interfacekey[INTERFACEKEY_STRING_A122].name="String: Character 122";
	interfacekey[INTERFACEKEY_STRING_A123].name="String: Character 123";
	interfacekey[INTERFACEKEY_STRING_A124].name="String: Character 124";
	interfacekey[INTERFACEKEY_STRING_A125].name="String: Character 125";
	interfacekey[INTERFACEKEY_STRING_A126].name="String: Character 126";
	interfacekey[INTERFACEKEY_STRING_A127].name="String: Character 127";
	interfacekey[INTERFACEKEY_STRING_A128].name="String: Character 128";
	interfacekey[INTERFACEKEY_STRING_A129].name="String: Character 129";
	interfacekey[INTERFACEKEY_STRING_A130].name="String: Character 130";
	interfacekey[INTERFACEKEY_STRING_A131].name="String: Character 131";
	interfacekey[INTERFACEKEY_STRING_A132].name="String: Character 132";
	interfacekey[INTERFACEKEY_STRING_A133].name="String: Character 133";
	interfacekey[INTERFACEKEY_STRING_A134].name="String: Character 134";
	interfacekey[INTERFACEKEY_STRING_A135].name="String: Character 135";
	interfacekey[INTERFACEKEY_STRING_A136].name="String: Character 136";
	interfacekey[INTERFACEKEY_STRING_A137].name="String: Character 137";
	interfacekey[INTERFACEKEY_STRING_A138].name="String: Character 138";
	interfacekey[INTERFACEKEY_STRING_A139].name="String: Character 139";
	interfacekey[INTERFACEKEY_STRING_A140].name="String: Character 140";
	interfacekey[INTERFACEKEY_STRING_A141].name="String: Character 141";
	interfacekey[INTERFACEKEY_STRING_A142].name="String: Character 142";
	interfacekey[INTERFACEKEY_STRING_A143].name="String: Character 143";
	interfacekey[INTERFACEKEY_STRING_A144].name="String: Character 144";
	interfacekey[INTERFACEKEY_STRING_A145].name="String: Character 145";
	interfacekey[INTERFACEKEY_STRING_A146].name="String: Character 146";
	interfacekey[INTERFACEKEY_STRING_A147].name="String: Character 147";
	interfacekey[INTERFACEKEY_STRING_A148].name="String: Character 148";
	interfacekey[INTERFACEKEY_STRING_A149].name="String: Character 149";
	interfacekey[INTERFACEKEY_STRING_A150].name="String: Character 150";
	interfacekey[INTERFACEKEY_STRING_A151].name="String: Character 151";
	interfacekey[INTERFACEKEY_STRING_A152].name="String: Character 152";
	interfacekey[INTERFACEKEY_STRING_A153].name="String: Character 153";
	interfacekey[INTERFACEKEY_STRING_A154].name="String: Character 154";
	interfacekey[INTERFACEKEY_STRING_A155].name="String: Character 155";
	interfacekey[INTERFACEKEY_STRING_A156].name="String: Character 156";
	interfacekey[INTERFACEKEY_STRING_A157].name="String: Character 157";
	interfacekey[INTERFACEKEY_STRING_A158].name="String: Character 158";
	interfacekey[INTERFACEKEY_STRING_A159].name="String: Character 159";
	interfacekey[INTERFACEKEY_STRING_A160].name="String: Character 160";
	interfacekey[INTERFACEKEY_STRING_A161].name="String: Character 161";
	interfacekey[INTERFACEKEY_STRING_A162].name="String: Character 162";
	interfacekey[INTERFACEKEY_STRING_A163].name="String: Character 163";
	interfacekey[INTERFACEKEY_STRING_A164].name="String: Character 164";
	interfacekey[INTERFACEKEY_STRING_A165].name="String: Character 165";
	interfacekey[INTERFACEKEY_STRING_A166].name="String: Character 166";
	interfacekey[INTERFACEKEY_STRING_A167].name="String: Character 167";
	interfacekey[INTERFACEKEY_STRING_A168].name="String: Character 168";
	interfacekey[INTERFACEKEY_STRING_A169].name="String: Character 169";
	interfacekey[INTERFACEKEY_STRING_A170].name="String: Character 170";
	interfacekey[INTERFACEKEY_STRING_A171].name="String: Character 171";
	interfacekey[INTERFACEKEY_STRING_A172].name="String: Character 172";
	interfacekey[INTERFACEKEY_STRING_A173].name="String: Character 173";
	interfacekey[INTERFACEKEY_STRING_A174].name="String: Character 174";
	interfacekey[INTERFACEKEY_STRING_A175].name="String: Character 175";
	interfacekey[INTERFACEKEY_STRING_A176].name="String: Character 176";
	interfacekey[INTERFACEKEY_STRING_A177].name="String: Character 177";
	interfacekey[INTERFACEKEY_STRING_A178].name="String: Character 178";
	interfacekey[INTERFACEKEY_STRING_A179].name="String: Character 179";
	interfacekey[INTERFACEKEY_STRING_A180].name="String: Character 180";
	interfacekey[INTERFACEKEY_STRING_A181].name="String: Character 181";
	interfacekey[INTERFACEKEY_STRING_A182].name="String: Character 182";
	interfacekey[INTERFACEKEY_STRING_A183].name="String: Character 183";
	interfacekey[INTERFACEKEY_STRING_A184].name="String: Character 184";
	interfacekey[INTERFACEKEY_STRING_A185].name="String: Character 185";
	interfacekey[INTERFACEKEY_STRING_A186].name="String: Character 186";
	interfacekey[INTERFACEKEY_STRING_A187].name="String: Character 187";
	interfacekey[INTERFACEKEY_STRING_A188].name="String: Character 188";
	interfacekey[INTERFACEKEY_STRING_A189].name="String: Character 189";
	interfacekey[INTERFACEKEY_STRING_A190].name="String: Character 190";
	interfacekey[INTERFACEKEY_STRING_A191].name="String: Character 191";
	interfacekey[INTERFACEKEY_STRING_A192].name="String: Character 192";
	interfacekey[INTERFACEKEY_STRING_A193].name="String: Character 193";
	interfacekey[INTERFACEKEY_STRING_A194].name="String: Character 194";
	interfacekey[INTERFACEKEY_STRING_A195].name="String: Character 195";
	interfacekey[INTERFACEKEY_STRING_A196].name="String: Character 196";
	interfacekey[INTERFACEKEY_STRING_A197].name="String: Character 197";
	interfacekey[INTERFACEKEY_STRING_A198].name="String: Character 198";
	interfacekey[INTERFACEKEY_STRING_A199].name="String: Character 199";
	interfacekey[INTERFACEKEY_STRING_A200].name="String: Character 200";
	interfacekey[INTERFACEKEY_STRING_A201].name="String: Character 201";
	interfacekey[INTERFACEKEY_STRING_A202].name="String: Character 202";
	interfacekey[INTERFACEKEY_STRING_A203].name="String: Character 203";
	interfacekey[INTERFACEKEY_STRING_A204].name="String: Character 204";
	interfacekey[INTERFACEKEY_STRING_A205].name="String: Character 205";
	interfacekey[INTERFACEKEY_STRING_A206].name="String: Character 206";
	interfacekey[INTERFACEKEY_STRING_A207].name="String: Character 207";
	interfacekey[INTERFACEKEY_STRING_A208].name="String: Character 208";
	interfacekey[INTERFACEKEY_STRING_A209].name="String: Character 209";
	interfacekey[INTERFACEKEY_STRING_A210].name="String: Character 210";
	interfacekey[INTERFACEKEY_STRING_A211].name="String: Character 211";
	interfacekey[INTERFACEKEY_STRING_A212].name="String: Character 212";
	interfacekey[INTERFACEKEY_STRING_A213].name="String: Character 213";
	interfacekey[INTERFACEKEY_STRING_A214].name="String: Character 214";
	interfacekey[INTERFACEKEY_STRING_A215].name="String: Character 215";
	interfacekey[INTERFACEKEY_STRING_A216].name="String: Character 216";
	interfacekey[INTERFACEKEY_STRING_A217].name="String: Character 217";
	interfacekey[INTERFACEKEY_STRING_A218].name="String: Character 218";
	interfacekey[INTERFACEKEY_STRING_A219].name="String: Character 219";
	interfacekey[INTERFACEKEY_STRING_A220].name="String: Character 220";
	interfacekey[INTERFACEKEY_STRING_A221].name="String: Character 221";
	interfacekey[INTERFACEKEY_STRING_A222].name="String: Character 222";
	interfacekey[INTERFACEKEY_STRING_A223].name="String: Character 223";
	interfacekey[INTERFACEKEY_STRING_A224].name="String: Character 224";
	interfacekey[INTERFACEKEY_STRING_A225].name="String: Character 225";
	interfacekey[INTERFACEKEY_STRING_A226].name="String: Character 226";
	interfacekey[INTERFACEKEY_STRING_A227].name="String: Character 227";
	interfacekey[INTERFACEKEY_STRING_A228].name="String: Character 228";
	interfacekey[INTERFACEKEY_STRING_A229].name="String: Character 229";
	interfacekey[INTERFACEKEY_STRING_A230].name="String: Character 230";
	interfacekey[INTERFACEKEY_STRING_A231].name="String: Character 231";
	interfacekey[INTERFACEKEY_STRING_A232].name="String: Character 232";
	interfacekey[INTERFACEKEY_STRING_A233].name="String: Character 233";
	interfacekey[INTERFACEKEY_STRING_A234].name="String: Character 234";
	interfacekey[INTERFACEKEY_STRING_A235].name="String: Character 235";
	interfacekey[INTERFACEKEY_STRING_A236].name="String: Character 236";
	interfacekey[INTERFACEKEY_STRING_A237].name="String: Character 237";
	interfacekey[INTERFACEKEY_STRING_A238].name="String: Character 238";
	interfacekey[INTERFACEKEY_STRING_A239].name="String: Character 239";
	interfacekey[INTERFACEKEY_STRING_A240].name="String: Character 240";
	interfacekey[INTERFACEKEY_STRING_A241].name="String: Character 241";
	interfacekey[INTERFACEKEY_STRING_A242].name="String: Character 242";
	interfacekey[INTERFACEKEY_STRING_A243].name="String: Character 243";
	interfacekey[INTERFACEKEY_STRING_A244].name="String: Character 244";
	interfacekey[INTERFACEKEY_STRING_A245].name="String: Character 245";
	interfacekey[INTERFACEKEY_STRING_A246].name="String: Character 246";
	interfacekey[INTERFACEKEY_STRING_A247].name="String: Character 247";
	interfacekey[INTERFACEKEY_STRING_A248].name="String: Character 248";
	interfacekey[INTERFACEKEY_STRING_A249].name="String: Character 249";
	interfacekey[INTERFACEKEY_STRING_A250].name="String: Character 250";
	interfacekey[INTERFACEKEY_STRING_A251].name="String: Character 251";
	interfacekey[INTERFACEKEY_STRING_A252].name="String: Character 252";
	interfacekey[INTERFACEKEY_STRING_A253].name="String: Character 253";
	interfacekey[INTERFACEKEY_STRING_A254].name="String: Character 254";
	interfacekey[INTERFACEKEY_STRING_A255].name="String: Character 255";

	interfacekey[INTERFACEKEY_SELECT].tokenname="SELECT";
	interfacekey[INTERFACEKEY_DESELECT].tokenname="DESELECT";
	interfacekey[INTERFACEKEY_SELECT_ALL].tokenname="SELECT_ALL";
	interfacekey[INTERFACEKEY_DESELECT_ALL].tokenname="DESELECT_ALL";
	interfacekey[INTERFACEKEY_LEAVESCREEN].tokenname="LEAVESCREEN";
	interfacekey[INTERFACEKEY_LEAVESCREEN_TEXT].tokenname="LEAVESCREEN_TEXT";
	interfacekey[INTERFACEKEY_LEAVESCREEN_ALL].tokenname="LEAVESCREEN_ALL";
	interfacekey[INTERFACEKEY_CLOSE_MEGA_ANNOUNCEMENT].tokenname="CLOSE_MEGA_ANNOUNCEMENT";
	interfacekey[INTERFACEKEY_TOGGLE_FULLSCREEN].tokenname="TOGGLE_FULLSCREEN";
	interfacekey[INTERFACEKEY_WORLD_PARAM_ADD].tokenname="WORLD_PARAM_ADD";
	interfacekey[INTERFACEKEY_WORLD_PARAM_COPY].tokenname="WORLD_PARAM_COPY";
	interfacekey[INTERFACEKEY_WORLD_PARAM_DELETE].tokenname="WORLD_PARAM_DELETE";
	interfacekey[INTERFACEKEY_WORLD_PARAM_TITLE].tokenname="WORLD_PARAM_TITLE";
	interfacekey[INTERFACEKEY_WORLD_PARAM_NAME_RANDOM].tokenname="WORLD_PARAM_NAME_RANDOM";
	interfacekey[INTERFACEKEY_WORLD_PARAM_NAME_ENTER].tokenname="WORLD_PARAM_NAME_ENTER";
	interfacekey[INTERFACEKEY_WORLD_PARAM_SEED_RANDOM].tokenname="WORLD_PARAM_SEED_RANDOM";
	interfacekey[INTERFACEKEY_WORLD_PARAM_SEED_ENTER].tokenname="WORLD_PARAM_SEED_ENTER";
	interfacekey[INTERFACEKEY_WORLD_PARAM_LOAD].tokenname="WORLD_PARAM_LOAD";
	interfacekey[INTERFACEKEY_WORLD_PARAM_SAVE].tokenname="WORLD_PARAM_SAVE";
	interfacekey[INTERFACEKEY_WORLD_PARAM_DIM_X_UP].tokenname="WORLD_PARAM_DIM_X_UP";
	interfacekey[INTERFACEKEY_WORLD_PARAM_DIM_X_DOWN].tokenname="WORLD_PARAM_DIM_X_DOWN";
	interfacekey[INTERFACEKEY_WORLD_PARAM_DIM_Y_UP].tokenname="WORLD_PARAM_DIM_Y_UP";
	interfacekey[INTERFACEKEY_WORLD_PARAM_DIM_Y_DOWN].tokenname="WORLD_PARAM_DIM_Y_DOWN";
	interfacekey[INTERFACEKEY_WORLD_PARAM_SET].tokenname="WORLD_PARAM_SET";
	interfacekey[INTERFACEKEY_WORLD_PARAM_INCREASE].tokenname="WORLD_PARAM_INCREASE";
	interfacekey[INTERFACEKEY_WORLD_PARAM_DECREASE].tokenname="WORLD_PARAM_DECREASE";
	interfacekey[INTERFACEKEY_WORLD_PARAM_ENTER_VALUE].tokenname="WORLD_PARAM_ENTER_VALUE";
	interfacekey[INTERFACEKEY_WORLD_PARAM_NULLIFY].tokenname="WORLD_PARAM_NULLIFY";
	interfacekey[INTERFACEKEY_WORLD_PARAM_PRESET].tokenname="WORLD_PARAM_PRESET";
	interfacekey[INTERFACEKEY_WORLD_PARAM_REJECT_CONTINUE].tokenname="WORLD_PARAM_REJECT_CONTINUE";
	interfacekey[INTERFACEKEY_WORLD_PARAM_REJECT_ABORT].tokenname="WORLD_PARAM_REJECT_ABORT";
	interfacekey[INTERFACEKEY_WORLD_PARAM_REJECT_ALLOW_THIS].tokenname="WORLD_PARAM_REJECT_ALLOW_THIS";
	interfacekey[INTERFACEKEY_WORLD_PARAM_REJECT_ALLOW_ALL].tokenname="WORLD_PARAM_REJECT_ALLOW_ALL";
	interfacekey[INTERFACEKEY_WORLD_GEN_CONTINUE].tokenname="WORLD_GEN_CONTINUE";
	interfacekey[INTERFACEKEY_WORLD_GEN_USE].tokenname="WORLD_GEN_USE";
	interfacekey[INTERFACEKEY_WORLD_GEN_ABORT].tokenname="WORLD_GEN_ABORT";
	interfacekey[INTERFACEKEY_SETUP_EMBARK].tokenname="SETUP_EMBARK";
	interfacekey[INTERFACEKEY_SETUP_NAME_FORT].tokenname="SETUP_NAME_FORT";
	interfacekey[INTERFACEKEY_SETUP_NAME_GROUP].tokenname="SETUP_NAME_GROUP";
	interfacekey[INTERFACEKEY_SETUP_RECLAIM].tokenname="SETUP_RECLAIM";
	interfacekey[INTERFACEKEY_SETUP_FIND].tokenname="SETUP_FIND";
	interfacekey[INTERFACEKEY_SETUP_NOTES].tokenname="SETUP_NOTES";
	interfacekey[INTERFACEKEY_SETUP_NOTES_TAKE_NOTES].tokenname="SETUP_NOTES_TAKE_NOTES";
	interfacekey[INTERFACEKEY_SETUP_NOTES_DELETE_NOTE].tokenname="SETUP_NOTES_DELETE_NOTE";
	interfacekey[INTERFACEKEY_SETUP_NOTES_CHANGE_SYMBOL_SELECTION].tokenname="SETUP_NOTES_CHANGE_SYMBOL_SELECTION";
	interfacekey[INTERFACEKEY_SETUP_NOTES_ADOPT_SYMBOL].tokenname="SETUP_NOTES_ADOPT_SYMBOL";
	interfacekey[INTERFACEKEY_SETUP_LOCAL_Y_UP].tokenname="SETUP_LOCAL_Y_UP";
	interfacekey[INTERFACEKEY_SETUP_LOCAL_Y_DOWN].tokenname="SETUP_LOCAL_Y_DOWN";
	interfacekey[INTERFACEKEY_SETUP_LOCAL_X_UP].tokenname="SETUP_LOCAL_X_UP";
	interfacekey[INTERFACEKEY_SETUP_LOCAL_X_DOWN].tokenname="SETUP_LOCAL_X_DOWN";
	interfacekey[INTERFACEKEY_SETUP_LOCAL_Y_MUP].tokenname="SETUP_LOCAL_Y_MUP";
	interfacekey[INTERFACEKEY_SETUP_LOCAL_Y_MDOWN].tokenname="SETUP_LOCAL_Y_MDOWN";
	interfacekey[INTERFACEKEY_SETUP_LOCAL_X_MUP].tokenname="SETUP_LOCAL_X_MUP";
	interfacekey[INTERFACEKEY_SETUP_LOCAL_X_MDOWN].tokenname="SETUP_LOCAL_X_MDOWN";
	interfacekey[INTERFACEKEY_SETUP_BIOME_1].tokenname="SETUP_BIOME_1";
	interfacekey[INTERFACEKEY_SETUP_BIOME_2].tokenname="SETUP_BIOME_2";
	interfacekey[INTERFACEKEY_SETUP_BIOME_3].tokenname="SETUP_BIOME_3";
	interfacekey[INTERFACEKEY_SETUP_BIOME_4].tokenname="SETUP_BIOME_4";
	interfacekey[INTERFACEKEY_SETUP_BIOME_5].tokenname="SETUP_BIOME_5";
	interfacekey[INTERFACEKEY_SETUP_BIOME_6].tokenname="SETUP_BIOME_6";
	interfacekey[INTERFACEKEY_SETUP_BIOME_7].tokenname="SETUP_BIOME_7";
	interfacekey[INTERFACEKEY_SETUP_BIOME_8].tokenname="SETUP_BIOME_8";
	interfacekey[INTERFACEKEY_SETUP_BIOME_9].tokenname="SETUP_BIOME_9";
	interfacekey[INTERFACEKEY_CHOOSE_NAME_RANDOM].tokenname="CHOOSE_NAME_RANDOM";
	interfacekey[INTERFACEKEY_CHOOSE_NAME_CLEAR].tokenname="CHOOSE_NAME_CLEAR";
	interfacekey[INTERFACEKEY_CHOOSE_NAME_TYPE].tokenname="CHOOSE_NAME_TYPE";
	interfacekey[INTERFACEKEY_ITEM_DESCRIPTION].tokenname="ITEM_DESCRIPTION";
	interfacekey[INTERFACEKEY_ITEM_FORBID].tokenname="ITEM_FORBID";
	interfacekey[INTERFACEKEY_ITEM_MELT].tokenname="ITEM_MELT";
	interfacekey[INTERFACEKEY_ITEM_DUMP].tokenname="ITEM_DUMP";
	interfacekey[INTERFACEKEY_ITEM_HIDE].tokenname="ITEM_HIDE";
	interfacekey[INTERFACEKEY_HELP].tokenname="HELP";
	interfacekey[INTERFACEKEY_MOVIES].tokenname="MOVIES";
	interfacekey[INTERFACEKEY_OPTIONS].tokenname="OPTIONS";
	interfacekey[INTERFACEKEY_OPTION_EXPORT].tokenname="OPTION_EXPORT";
	interfacekey[INTERFACEKEY_CHANGETAB].tokenname="CHANGETAB";
	interfacekey[INTERFACEKEY_STANDARDSCROLL_LEFT].tokenname="STANDARDSCROLL_LEFT";
	interfacekey[INTERFACEKEY_STANDARDSCROLL_RIGHT].tokenname="STANDARDSCROLL_RIGHT";
	interfacekey[INTERFACEKEY_STANDARDSCROLL_UP].tokenname="STANDARDSCROLL_UP";
	interfacekey[INTERFACEKEY_STANDARDSCROLL_DOWN].tokenname="STANDARDSCROLL_DOWN";
	interfacekey[INTERFACEKEY_STANDARDSCROLL_PAGEUP].tokenname="STANDARDSCROLL_PAGEUP";
	interfacekey[INTERFACEKEY_STANDARDSCROLL_PAGEDOWN].tokenname="STANDARDSCROLL_PAGEDOWN";
	interfacekey[INTERFACEKEY_SECONDSCROLL_UP].tokenname="SECONDSCROLL_UP";
	interfacekey[INTERFACEKEY_SECONDSCROLL_DOWN].tokenname="SECONDSCROLL_DOWN";
	interfacekey[INTERFACEKEY_SECONDSCROLL_PAGEUP].tokenname="SECONDSCROLL_PAGEUP";
	interfacekey[INTERFACEKEY_SECONDSCROLL_PAGEDOWN].tokenname="SECONDSCROLL_PAGEDOWN";
	interfacekey[INTERFACEKEY_CURSOR_UP].tokenname="CURSOR_UP";
	interfacekey[INTERFACEKEY_CURSOR_DOWN].tokenname="CURSOR_DOWN";
	interfacekey[INTERFACEKEY_CURSOR_LEFT].tokenname="CURSOR_LEFT";
	interfacekey[INTERFACEKEY_CURSOR_RIGHT].tokenname="CURSOR_RIGHT";
	interfacekey[INTERFACEKEY_CURSOR_UPLEFT].tokenname="CURSOR_UPLEFT";
	interfacekey[INTERFACEKEY_CURSOR_UPRIGHT].tokenname="CURSOR_UPRIGHT";
	interfacekey[INTERFACEKEY_CURSOR_DOWNLEFT].tokenname="CURSOR_DOWNLEFT";
	interfacekey[INTERFACEKEY_CURSOR_DOWNRIGHT].tokenname="CURSOR_DOWNRIGHT";
	interfacekey[INTERFACEKEY_CURSOR_UP_FAST].tokenname="CURSOR_UP_FAST";
	interfacekey[INTERFACEKEY_CURSOR_DOWN_FAST].tokenname="CURSOR_DOWN_FAST";
	interfacekey[INTERFACEKEY_CURSOR_LEFT_FAST].tokenname="CURSOR_LEFT_FAST";
	interfacekey[INTERFACEKEY_CURSOR_RIGHT_FAST].tokenname="CURSOR_RIGHT_FAST";
	interfacekey[INTERFACEKEY_CURSOR_UPLEFT_FAST].tokenname="CURSOR_UPLEFT_FAST";
	interfacekey[INTERFACEKEY_CURSOR_UPRIGHT_FAST].tokenname="CURSOR_UPRIGHT_FAST";
	interfacekey[INTERFACEKEY_CURSOR_DOWNLEFT_FAST].tokenname="CURSOR_DOWNLEFT_FAST";
	interfacekey[INTERFACEKEY_CURSOR_DOWNRIGHT_FAST].tokenname="CURSOR_DOWNRIGHT_FAST";
	interfacekey[INTERFACEKEY_CURSOR_UP_Z].tokenname="CURSOR_UP_Z";
	interfacekey[INTERFACEKEY_CURSOR_DOWN_Z].tokenname="CURSOR_DOWN_Z";
	interfacekey[INTERFACEKEY_CURSOR_UP_Z_AUX].tokenname="CURSOR_UP_Z_AUX";
	interfacekey[INTERFACEKEY_CURSOR_DOWN_Z_AUX].tokenname="CURSOR_DOWN_Z_AUX";
	interfacekey[INTERFACEKEY_A_MOVE_N].tokenname="A_MOVE_N";
	interfacekey[INTERFACEKEY_A_MOVE_S].tokenname="A_MOVE_S";
	interfacekey[INTERFACEKEY_A_MOVE_E].tokenname="A_MOVE_E";
	interfacekey[INTERFACEKEY_A_MOVE_W].tokenname="A_MOVE_W";
	interfacekey[INTERFACEKEY_A_MOVE_NW].tokenname="A_MOVE_NW";
	interfacekey[INTERFACEKEY_A_MOVE_NE].tokenname="A_MOVE_NE";
	interfacekey[INTERFACEKEY_A_MOVE_SW].tokenname="A_MOVE_SW";
	interfacekey[INTERFACEKEY_A_MOVE_SE].tokenname="A_MOVE_SE";
	interfacekey[INTERFACEKEY_A_MOVE_WAIT].tokenname="A_MOVE_WAIT";
	interfacekey[INTERFACEKEY_A_CARE_MOVE_N].tokenname="A_CARE_MOVE_N";
	interfacekey[INTERFACEKEY_A_CARE_MOVE_S].tokenname="A_CARE_MOVE_S";
	interfacekey[INTERFACEKEY_A_CARE_MOVE_E].tokenname="A_CARE_MOVE_E";
	interfacekey[INTERFACEKEY_A_CARE_MOVE_W].tokenname="A_CARE_MOVE_W";
	interfacekey[INTERFACEKEY_A_CARE_MOVE_NW].tokenname="A_CARE_MOVE_NW";
	interfacekey[INTERFACEKEY_A_CARE_MOVE_NE].tokenname="A_CARE_MOVE_NE";
	interfacekey[INTERFACEKEY_A_CARE_MOVE_SW].tokenname="A_CARE_MOVE_SW";
	interfacekey[INTERFACEKEY_A_CARE_MOVE_SE].tokenname="A_CARE_MOVE_SE";
	interfacekey[INTERFACEKEY_A_CARE_MOVE_UPDOWN].tokenname="A_CARE_MOVE_UPDOWN";
	interfacekey[INTERFACEKEY_A_MOVE_N_UP].tokenname="A_MOVE_N_UP";
	interfacekey[INTERFACEKEY_A_MOVE_S_UP].tokenname="A_MOVE_S_UP";
	interfacekey[INTERFACEKEY_A_MOVE_E_UP].tokenname="A_MOVE_E_UP";
	interfacekey[INTERFACEKEY_A_MOVE_W_UP].tokenname="A_MOVE_W_UP";
	interfacekey[INTERFACEKEY_A_MOVE_NW_UP].tokenname="A_MOVE_NW_UP";
	interfacekey[INTERFACEKEY_A_MOVE_NE_UP].tokenname="A_MOVE_NE_UP";
	interfacekey[INTERFACEKEY_A_MOVE_SW_UP].tokenname="A_MOVE_SW_UP";
	interfacekey[INTERFACEKEY_A_MOVE_SE_UP].tokenname="A_MOVE_SE_UP";
	interfacekey[INTERFACEKEY_A_MOVE_UP].tokenname="A_MOVE_UP";
	interfacekey[INTERFACEKEY_A_MOVE_N_DOWN].tokenname="A_MOVE_N_DOWN";
	interfacekey[INTERFACEKEY_A_MOVE_S_DOWN].tokenname="A_MOVE_S_DOWN";
	interfacekey[INTERFACEKEY_A_MOVE_E_DOWN].tokenname="A_MOVE_E_DOWN";
	interfacekey[INTERFACEKEY_A_MOVE_W_DOWN].tokenname="A_MOVE_W_DOWN";
	interfacekey[INTERFACEKEY_A_MOVE_NW_DOWN].tokenname="A_MOVE_NW_DOWN";
	interfacekey[INTERFACEKEY_A_MOVE_NE_DOWN].tokenname="A_MOVE_NE_DOWN";
	interfacekey[INTERFACEKEY_A_MOVE_SW_DOWN].tokenname="A_MOVE_SW_DOWN";
	interfacekey[INTERFACEKEY_A_MOVE_SE_DOWN].tokenname="A_MOVE_SE_DOWN";
	interfacekey[INTERFACEKEY_A_MOVE_DOWN].tokenname="A_MOVE_DOWN";
	interfacekey[INTERFACEKEY_A_MOVE_UP_AUX].tokenname="A_MOVE_UP_AUX";
	interfacekey[INTERFACEKEY_A_MOVE_DOWN_AUX].tokenname="A_MOVE_DOWN_AUX";
	interfacekey[INTERFACEKEY_WORLDGEN_EXPORT_MAP].tokenname="WORLDGEN_EXPORT_MAP";
	interfacekey[INTERFACEKEY_LEGENDS_EXPORT_MAP].tokenname="LEGENDS_EXPORT_MAP";
	interfacekey[INTERFACEKEY_LEGENDS_EXPORT_DETAILED_MAP].tokenname="LEGENDS_EXPORT_DETAILED_MAP";
	interfacekey[INTERFACEKEY_LEGENDS_TOGGLE_CIVSITE].tokenname="LEGENDS_TOGGLE_CIVSITE";
	interfacekey[INTERFACEKEY_LEGENDS_STRING_FILTER].tokenname="LEGENDS_STRING_FILTER";
	interfacekey[INTERFACEKEY_A_COMBAT_ATTACK].tokenname="A_COMBAT_ATTACK";
	interfacekey[INTERFACEKEY_A_COMBAT_DODGE].tokenname="A_COMBAT_DODGE";
	interfacekey[INTERFACEKEY_A_COMBAT_CHARGEDEF].tokenname="A_COMBAT_CHARGEDEF";
	interfacekey[INTERFACEKEY_A_STATUS].tokenname="A_STATUS";
	interfacekey[INTERFACEKEY_A_STATUS_WRESTLE].tokenname="A_STATUS_WRESTLE";
	interfacekey[INTERFACEKEY_A_STATUS_CUSTOMIZE].tokenname="A_STATUS_CUSTOMIZE";
	interfacekey[INTERFACEKEY_A_STATUS_KILLS].tokenname="A_STATUS_KILLS";
	interfacekey[INTERFACEKEY_UNITVIEW_CUSTOMIZE].tokenname="UNITVIEW_CUSTOMIZE";
	interfacekey[INTERFACEKEY_UNITVIEW_RELATIONSHIPS].tokenname="UNITVIEW_RELATIONSHIPS";
	interfacekey[INTERFACEKEY_UNITVIEW_RELATIONSHIPS_ZOOM].tokenname="UNITVIEW_RELATIONSHIPS_ZOOM";
	interfacekey[INTERFACEKEY_UNITVIEW_RELATIONSHIPS_VIEW].tokenname="UNITVIEW_RELATIONSHIPS_VIEW";
	interfacekey[INTERFACEKEY_UNITVIEW_KILLS].tokenname="UNITVIEW_KILLS";
	interfacekey[INTERFACEKEY_CUSTOMIZE_UNIT_NICKNAME].tokenname="CUSTOMIZE_UNIT_NICKNAME";
	interfacekey[INTERFACEKEY_CUSTOMIZE_UNIT_PROFNAME].tokenname="CUSTOMIZE_UNIT_PROFNAME";
	interfacekey[INTERFACEKEY_A_SLEEP].tokenname="A_SLEEP";
	interfacekey[INTERFACEKEY_A_WAIT].tokenname="A_WAIT";
	interfacekey[INTERFACEKEY_A_ATTACK].tokenname="A_ATTACK";
	interfacekey[INTERFACEKEY_A_LOOK].tokenname="A_LOOK";
	interfacekey[INTERFACEKEY_A_SEARCH].tokenname="A_SEARCH";
	interfacekey[INTERFACEKEY_A_TALK].tokenname="A_TALK";
	interfacekey[INTERFACEKEY_A_INTERACT].tokenname="A_INTERACT";
	interfacekey[INTERFACEKEY_A_INV_LOOK].tokenname="A_INV_LOOK";
	interfacekey[INTERFACEKEY_A_INV_REMOVE].tokenname="A_INV_REMOVE";
	interfacekey[INTERFACEKEY_A_INV_WEAR].tokenname="A_INV_WEAR";
	interfacekey[INTERFACEKEY_A_INV_EATDRINK].tokenname="A_INV_EATDRINK";
	interfacekey[INTERFACEKEY_A_INV_PUTIN].tokenname="A_INV_PUTIN";
	interfacekey[INTERFACEKEY_A_INV_DROP].tokenname="A_INV_DROP";
	interfacekey[INTERFACEKEY_A_GROUND].tokenname="A_GROUND";
	interfacekey[INTERFACEKEY_A_THROW].tokenname="A_THROW";
	interfacekey[INTERFACEKEY_A_SHOOT].tokenname="A_SHOOT";
	interfacekey[INTERFACEKEY_A_ANNOUNCEMENTS].tokenname="A_ANNOUNCEMENTS";
	interfacekey[INTERFACEKEY_A_COMBAT].tokenname="A_COMBAT";
	interfacekey[INTERFACEKEY_A_MOVEMENT].tokenname="A_MOVEMENT";
	interfacekey[INTERFACEKEY_A_MOVEMENT_SWIM].tokenname="A_MOVEMENT_SWIM";
	interfacekey[INTERFACEKEY_A_SNEAK].tokenname="A_SNEAK";
	interfacekey[INTERFACEKEY_A_CENTER].tokenname="A_CENTER";
	interfacekey[INTERFACEKEY_A_BUILDING].tokenname="A_BUILDING";
	interfacekey[INTERFACEKEY_A_TRAVEL].tokenname="A_TRAVEL";
	interfacekey[INTERFACEKEY_A_DATE].tokenname="A_DATE";
	interfacekey[INTERFACEKEY_A_WEATHER].tokenname="A_WEATHER";
	interfacekey[INTERFACEKEY_A_TEMPERATURE].tokenname="A_TEMPERATURE";
	interfacekey[INTERFACEKEY_A_STANCE].tokenname="A_STANCE";
	interfacekey[INTERFACEKEY_A_OPTION1].tokenname="A_OPTION1";
	interfacekey[INTERFACEKEY_A_OPTION2].tokenname="A_OPTION2";
	interfacekey[INTERFACEKEY_A_OPTION3].tokenname="A_OPTION3";
	interfacekey[INTERFACEKEY_A_OPTION4].tokenname="A_OPTION4";
	interfacekey[INTERFACEKEY_A_OPTION5].tokenname="A_OPTION5";
	interfacekey[INTERFACEKEY_A_OPTION6].tokenname="A_OPTION6";
	interfacekey[INTERFACEKEY_A_OPTION7].tokenname="A_OPTION7";
	interfacekey[INTERFACEKEY_A_OPTION8].tokenname="A_OPTION8";
	interfacekey[INTERFACEKEY_A_OPTION9].tokenname="A_OPTION9";
	interfacekey[INTERFACEKEY_A_OPTION10].tokenname="A_OPTION10";
	interfacekey[INTERFACEKEY_A_OPTION11].tokenname="A_OPTION11";
	interfacekey[INTERFACEKEY_A_OPTION12].tokenname="A_OPTION12";
	interfacekey[INTERFACEKEY_A_OPTION13].tokenname="A_OPTION13";
	interfacekey[INTERFACEKEY_A_OPTION14].tokenname="A_OPTION14";
	interfacekey[INTERFACEKEY_A_OPTION15].tokenname="A_OPTION15";
	interfacekey[INTERFACEKEY_A_OPTION16].tokenname="A_OPTION16";
	interfacekey[INTERFACEKEY_A_OPTION17].tokenname="A_OPTION17";
	interfacekey[INTERFACEKEY_A_OPTION18].tokenname="A_OPTION18";
	interfacekey[INTERFACEKEY_A_OPTION19].tokenname="A_OPTION19";
	interfacekey[INTERFACEKEY_A_OPTION20].tokenname="A_OPTION20";
	interfacekey[INTERFACEKEY_HOTKEY_MAKE_ASH].tokenname="HOTKEY_MAKE_ASH";
	interfacekey[INTERFACEKEY_HOTKEY_MAKE_CHARCOAL].tokenname="HOTKEY_MAKE_CHARCOAL";
	interfacekey[INTERFACEKEY_HOTKEY_MELT_OBJECT].tokenname="HOTKEY_MELT_OBJECT";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_GREEN].tokenname="HOTKEY_GLASS_GREEN";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_CLEAR].tokenname="HOTKEY_GLASS_CLEAR";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_CRYSTAL].tokenname="HOTKEY_GLASS_CRYSTAL";
	interfacekey[INTERFACEKEY_HOTKEY_COLLECT_SAND].tokenname="HOTKEY_COLLECT_SAND";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_ROUGH].tokenname="HOTKEY_GLASS_ROUGH";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_ARMORSTAND].tokenname="HOTKEY_GLASS_ARMORSTAND";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_BOX].tokenname="HOTKEY_GLASS_BOX";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_CABINET].tokenname="HOTKEY_GLASS_CABINET";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_COFFIN].tokenname="HOTKEY_GLASS_COFFIN";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_FLOODGATE].tokenname="HOTKEY_GLASS_FLOODGATE";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_HATCH_COVER].tokenname="HOTKEY_GLASS_HATCH_COVER";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_GRATE].tokenname="HOTKEY_GLASS_GRATE";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_GOBLET].tokenname="HOTKEY_GLASS_GOBLET";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_TOY].tokenname="HOTKEY_GLASS_TOY";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_INSTRUMENT].tokenname="HOTKEY_GLASS_INSTRUMENT";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_DOOR].tokenname="HOTKEY_GLASS_DOOR";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_STATUE].tokenname="HOTKEY_GLASS_STATUE";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_TABLE].tokenname="HOTKEY_GLASS_TABLE";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_CAGE].tokenname="HOTKEY_GLASS_CAGE";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_CHAIR].tokenname="HOTKEY_GLASS_CHAIR";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_BLOCKS].tokenname="HOTKEY_GLASS_BLOCKS";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_FLASK].tokenname="HOTKEY_GLASS_FLASK";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_WEAPONRACK].tokenname="HOTKEY_GLASS_WEAPONRACK";
	interfacekey[INTERFACEKEY_HOTKEY_GLASS_WINDOW].tokenname="HOTKEY_GLASS_WINDOW";
	interfacekey[INTERFACEKEY_HOTKEY_MAKE_PEARLASH].tokenname="HOTKEY_MAKE_PEARLASH";
	interfacekey[INTERFACEKEY_HOTKEY_ASHERY_LYE].tokenname="HOTKEY_ASHERY_LYE";
	interfacekey[INTERFACEKEY_HOTKEY_ASHERY_POTASH].tokenname="HOTKEY_ASHERY_POTASH";
	interfacekey[INTERFACEKEY_HOTKEY_ASHERY_POTASH_DIRECT].tokenname="HOTKEY_ASHERY_POTASH_DIRECT";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_BARREL].tokenname="HOTKEY_CARPENTER_BARREL";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_BLOCKS].tokenname="HOTKEY_CARPENTER_BLOCKS";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_BUCKET].tokenname="HOTKEY_CARPENTER_BUCKET";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_TRAP_ANIMAL].tokenname="HOTKEY_CARPENTER_TRAP_ANIMAL";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_CAGE].tokenname="HOTKEY_CARPENTER_CAGE";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_ARMORSTAND].tokenname="HOTKEY_CARPENTER_ARMORSTAND";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_BED].tokenname="HOTKEY_CARPENTER_BED";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_CHAIR].tokenname="HOTKEY_CARPENTER_CHAIR";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_COFFIN].tokenname="HOTKEY_CARPENTER_COFFIN";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_DOOR].tokenname="HOTKEY_CARPENTER_DOOR";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_FLOODGATE].tokenname="HOTKEY_CARPENTER_FLOODGATE";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_HATCH_COVER].tokenname="HOTKEY_CARPENTER_HATCH_COVER";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_GRATE].tokenname="HOTKEY_CARPENTER_GRATE";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_CABINET].tokenname="HOTKEY_CARPENTER_CABINET";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_BIN].tokenname="HOTKEY_CARPENTER_BIN";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_BOX].tokenname="HOTKEY_CARPENTER_BOX";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_WEAPONRACK].tokenname="HOTKEY_CARPENTER_WEAPONRACK";
	interfacekey[INTERFACEKEY_HOTKEY_CARPENTER_TABLE].tokenname="HOTKEY_CARPENTER_TABLE";
	interfacekey[INTERFACEKEY_HOTKEY_SIEGE_BALLISTA].tokenname="HOTKEY_SIEGE_BALLISTA";
	interfacekey[INTERFACEKEY_HOTKEY_SIEGE_CATAPULT].tokenname="HOTKEY_SIEGE_CATAPULT";
	interfacekey[INTERFACEKEY_HOTKEY_LEATHER_BOX].tokenname="HOTKEY_LEATHER_BOX";
	interfacekey[INTERFACEKEY_HOTKEY_LEATHER_FLASK].tokenname="HOTKEY_LEATHER_FLASK";
	interfacekey[INTERFACEKEY_HOTKEY_LEATHER_SHIRT].tokenname="HOTKEY_LEATHER_SHIRT";
	interfacekey[INTERFACEKEY_HOTKEY_LEATHER_CLOAK].tokenname="HOTKEY_LEATHER_CLOAK";
	interfacekey[INTERFACEKEY_HOTKEY_LEATHER_BACKPACK].tokenname="HOTKEY_LEATHER_BACKPACK";
	interfacekey[INTERFACEKEY_HOTKEY_LEATHER_QUIVER].tokenname="HOTKEY_LEATHER_QUIVER";
	interfacekey[INTERFACEKEY_HOTKEY_LEATHER_IMAGE].tokenname="HOTKEY_LEATHER_IMAGE";
	interfacekey[INTERFACEKEY_HOTKEY_CLOTHES_MAT_PLANT].tokenname="HOTKEY_CLOTHES_MAT_PLANT";
	interfacekey[INTERFACEKEY_HOTKEY_CLOTHES_MAT_SILK].tokenname="HOTKEY_CLOTHES_MAT_SILK";
	interfacekey[INTERFACEKEY_HOTKEY_CLOTHES_SHIRT].tokenname="HOTKEY_CLOTHES_SHIRT";
	interfacekey[INTERFACEKEY_HOTKEY_CLOTHES_CLOAK].tokenname="HOTKEY_CLOTHES_CLOAK";
	interfacekey[INTERFACEKEY_HOTKEY_CLOTHES_BOX].tokenname="HOTKEY_CLOTHES_BOX";
	interfacekey[INTERFACEKEY_HOTKEY_CLOTHES_CHAIN].tokenname="HOTKEY_CLOTHES_CHAIN";
	interfacekey[INTERFACEKEY_HOTKEY_CLOTHES_IMAGE].tokenname="HOTKEY_CLOTHES_IMAGE";
	interfacekey[INTERFACEKEY_HOTKEY_CRAFTS_MAT_STONE].tokenname="HOTKEY_CRAFTS_MAT_STONE";
	interfacekey[INTERFACEKEY_HOTKEY_CRAFTS_MAT_WOOD].tokenname="HOTKEY_CRAFTS_MAT_WOOD";
	interfacekey[INTERFACEKEY_HOTKEY_CRAFTS_DEC_BONE].tokenname="HOTKEY_CRAFTS_DEC_BONE";
	interfacekey[INTERFACEKEY_HOTKEY_CRAFTS_DEC_SHELL].tokenname="HOTKEY_CRAFTS_DEC_SHELL";
	interfacekey[INTERFACEKEY_HOTKEY_CRAFTS_TOTEM].tokenname="HOTKEY_CRAFTS_TOTEM";
	interfacekey[INTERFACEKEY_HOTKEY_CRAFTS_CLOTH].tokenname="HOTKEY_CRAFTS_CLOTH";
	interfacekey[INTERFACEKEY_HOTKEY_CRAFTS_SILK].tokenname="HOTKEY_CRAFTS_SILK";
	interfacekey[INTERFACEKEY_HOTKEY_CRAFTS_SHELL].tokenname="HOTKEY_CRAFTS_SHELL";
	interfacekey[INTERFACEKEY_HOTKEY_CRAFTS_BONE].tokenname="HOTKEY_CRAFTS_BONE";
	interfacekey[INTERFACEKEY_HOTKEY_CRAFTS_LEATHER].tokenname="HOTKEY_CRAFTS_LEATHER";
	interfacekey[INTERFACEKEY_HOTKEY_CRAFTS_MAT_CRAFTS].tokenname="HOTKEY_CRAFTS_MAT_CRAFTS";
	interfacekey[INTERFACEKEY_HOTKEY_CRAFTS_MAT_GOBLET].tokenname="HOTKEY_CRAFTS_MAT_GOBLET";
	interfacekey[INTERFACEKEY_HOTKEY_CRAFTS_MAT_INSTRUMENT].tokenname="HOTKEY_CRAFTS_MAT_INSTRUMENT";
	interfacekey[INTERFACEKEY_HOTKEY_CRAFTS_MAT_TOY].tokenname="HOTKEY_CRAFTS_MAT_TOY";
	interfacekey[INTERFACEKEY_HOTKEY_SMITH_WEAPON].tokenname="HOTKEY_SMITH_WEAPON";
	interfacekey[INTERFACEKEY_HOTKEY_SMITH_ARMOR].tokenname="HOTKEY_SMITH_ARMOR";
	interfacekey[INTERFACEKEY_HOTKEY_SMITH_FURNITURE].tokenname="HOTKEY_SMITH_FURNITURE";
	interfacekey[INTERFACEKEY_HOTKEY_SMITH_SIEGE].tokenname="HOTKEY_SMITH_SIEGE";
	interfacekey[INTERFACEKEY_HOTKEY_SMITH_TRAP].tokenname="HOTKEY_SMITH_TRAP";
	interfacekey[INTERFACEKEY_HOTKEY_SMITH_OTHER].tokenname="HOTKEY_SMITH_OTHER";
	interfacekey[INTERFACEKEY_HOTKEY_SMITH_METAL].tokenname="HOTKEY_SMITH_METAL";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_ARMORSTAND].tokenname="HOTKEY_BUILDING_ARMORSTAND";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_BED].tokenname="HOTKEY_BUILDING_BED";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_CHAIR].tokenname="HOTKEY_BUILDING_CHAIR";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_COFFIN].tokenname="HOTKEY_BUILDING_COFFIN";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_DOOR].tokenname="HOTKEY_BUILDING_DOOR";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_FLOODGATE].tokenname="HOTKEY_BUILDING_FLOODGATE";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_HATCH].tokenname="HOTKEY_BUILDING_HATCH";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_GRATE_WALL].tokenname="HOTKEY_BUILDING_GRATE_WALL";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_GRATE_FLOOR].tokenname="HOTKEY_BUILDING_GRATE_FLOOR";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_BARS_VERTICAL].tokenname="HOTKEY_BUILDING_BARS_VERTICAL";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_BARS_FLOOR].tokenname="HOTKEY_BUILDING_BARS_FLOOR";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_CABINET].tokenname="HOTKEY_BUILDING_CABINET";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_BOX].tokenname="HOTKEY_BUILDING_BOX";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_KENNEL].tokenname="HOTKEY_BUILDING_KENNEL";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_FARMPLOT].tokenname="HOTKEY_BUILDING_FARMPLOT";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WEAPONRACK].tokenname="HOTKEY_BUILDING_WEAPONRACK";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_STATUE].tokenname="HOTKEY_BUILDING_STATUE";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_TABLE].tokenname="HOTKEY_BUILDING_TABLE";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_ROAD_DIRT].tokenname="HOTKEY_BUILDING_ROAD_DIRT";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_ROAD_PAVED].tokenname="HOTKEY_BUILDING_ROAD_PAVED";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_BRIDGE].tokenname="HOTKEY_BUILDING_BRIDGE";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WELL].tokenname="HOTKEY_BUILDING_WELL";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_SIEGEENGINE].tokenname="HOTKEY_BUILDING_SIEGEENGINE";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP].tokenname="HOTKEY_BUILDING_WORKSHOP";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_FURNACE].tokenname="HOTKEY_BUILDING_FURNACE";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WINDOW_GLASS].tokenname="HOTKEY_BUILDING_WINDOW_GLASS";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WINDOW_GEM].tokenname="HOTKEY_BUILDING_WINDOW_GEM";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_SHOP].tokenname="HOTKEY_BUILDING_SHOP";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_ANIMALTRAP].tokenname="HOTKEY_BUILDING_ANIMALTRAP";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_CHAIN].tokenname="HOTKEY_BUILDING_CHAIN";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_CAGE].tokenname="HOTKEY_BUILDING_CAGE";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_TRADEDEPOT].tokenname="HOTKEY_BUILDING_TRADEDEPOT";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_TRAP].tokenname="HOTKEY_BUILDING_TRAP";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_MACHINE].tokenname="HOTKEY_BUILDING_MACHINE";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_SUPPORT].tokenname="HOTKEY_BUILDING_SUPPORT";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_ARCHERYTARGET].tokenname="HOTKEY_BUILDING_ARCHERYTARGET";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_MACHINE_SCREW_PUMP].tokenname="HOTKEY_BUILDING_MACHINE_SCREW_PUMP";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_MACHINE_WATER_WHEEL].tokenname="HOTKEY_BUILDING_MACHINE_WATER_WHEEL";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_MACHINE_WINDMILL].tokenname="HOTKEY_BUILDING_MACHINE_WINDMILL";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_MACHINE_GEAR_ASSEMBLY].tokenname="HOTKEY_BUILDING_MACHINE_GEAR_ASSEMBLY";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_MACHINE_AXLE_HORIZONTAL].tokenname="HOTKEY_BUILDING_MACHINE_AXLE_HORIZONTAL";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_MACHINE_AXLE_VERTICAL].tokenname="HOTKEY_BUILDING_MACHINE_AXLE_VERTICAL";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_SIEGEENGINE_BALLISTA].tokenname="HOTKEY_BUILDING_SIEGEENGINE_BALLISTA";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_SIEGEENGINE_CATAPULT].tokenname="HOTKEY_BUILDING_SIEGEENGINE_CATAPULT";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_TRAP_STONE].tokenname="HOTKEY_BUILDING_TRAP_STONE";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_TRAP_WEAPON].tokenname="HOTKEY_BUILDING_TRAP_WEAPON";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_TRAP_LEVER].tokenname="HOTKEY_BUILDING_TRAP_LEVER";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_TRAP_TRIGGER].tokenname="HOTKEY_BUILDING_TRAP_TRIGGER";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_TRAP_CAGE].tokenname="HOTKEY_BUILDING_TRAP_CAGE";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_TRAP_SPIKE].tokenname="HOTKEY_BUILDING_TRAP_SPIKE";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_CONSTRUCTION].tokenname="HOTKEY_BUILDING_CONSTRUCTION";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_CONSTRUCTION_FORTIFICATION].tokenname="HOTKEY_BUILDING_CONSTRUCTION_FORTIFICATION";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_CONSTRUCTION_WALL].tokenname="HOTKEY_BUILDING_CONSTRUCTION_WALL";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_CONSTRUCTION_FLOOR].tokenname="HOTKEY_BUILDING_CONSTRUCTION_FLOOR";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_CONSTRUCTION_RAMP].tokenname="HOTKEY_BUILDING_CONSTRUCTION_RAMP";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_CONSTRUCTION_STAIR_UP].tokenname="HOTKEY_BUILDING_CONSTRUCTION_STAIR_UP";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_CONSTRUCTION_STAIR_DOWN].tokenname="HOTKEY_BUILDING_CONSTRUCTION_STAIR_DOWN";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_CONSTRUCTION_STAIR_UPDOWN].tokenname="HOTKEY_BUILDING_CONSTRUCTION_STAIR_UPDOWN";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_LEATHER].tokenname="HOTKEY_BUILDING_WORKSHOP_LEATHER";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_QUERN].tokenname="HOTKEY_BUILDING_WORKSHOP_QUERN";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_MILLSTONE].tokenname="HOTKEY_BUILDING_WORKSHOP_MILLSTONE";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_LOOM].tokenname="HOTKEY_BUILDING_WORKSHOP_LOOM";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_CLOTHES].tokenname="HOTKEY_BUILDING_WORKSHOP_CLOTHES";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_BOWYER].tokenname="HOTKEY_BUILDING_WORKSHOP_BOWYER";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_CARPENTER].tokenname="HOTKEY_BUILDING_WORKSHOP_CARPENTER";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_METALSMITH].tokenname="HOTKEY_BUILDING_WORKSHOP_METALSMITH";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_LAVAMILL].tokenname="HOTKEY_BUILDING_WORKSHOP_LAVAMILL";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_JEWELER].tokenname="HOTKEY_BUILDING_WORKSHOP_JEWELER";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_MASON].tokenname="HOTKEY_BUILDING_WORKSHOP_MASON";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_BUTCHER].tokenname="HOTKEY_BUILDING_WORKSHOP_BUTCHER";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_TANNER].tokenname="HOTKEY_BUILDING_WORKSHOP_TANNER";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_DYER].tokenname="HOTKEY_BUILDING_WORKSHOP_DYER";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_CRAFTSMAN].tokenname="HOTKEY_BUILDING_WORKSHOP_CRAFTSMAN";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_SIEGE].tokenname="HOTKEY_BUILDING_WORKSHOP_SIEGE";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_MECHANIC].tokenname="HOTKEY_BUILDING_WORKSHOP_MECHANIC";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_STILL].tokenname="HOTKEY_BUILDING_WORKSHOP_STILL";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_FARMER].tokenname="HOTKEY_BUILDING_WORKSHOP_FARMER";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_KITCHEN].tokenname="HOTKEY_BUILDING_WORKSHOP_KITCHEN";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_FISHERY].tokenname="HOTKEY_BUILDING_WORKSHOP_FISHERY";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_LAB].tokenname="HOTKEY_BUILDING_WORKSHOP_LAB";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_WORKSHOP_ASHERY].tokenname="HOTKEY_BUILDING_WORKSHOP_ASHERY";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_FURNACE_WOOD].tokenname="HOTKEY_BUILDING_FURNACE_WOOD";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_FURNACE_SMELTER].tokenname="HOTKEY_BUILDING_FURNACE_SMELTER";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_FURNACE_GLASS].tokenname="HOTKEY_BUILDING_FURNACE_GLASS";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_FURNACE_KILN].tokenname="HOTKEY_BUILDING_FURNACE_KILN";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_FURNACE_SMELTER_LAVA].tokenname="HOTKEY_BUILDING_FURNACE_SMELTER_LAVA";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_FURNACE_GLASS_LAVA].tokenname="HOTKEY_BUILDING_FURNACE_GLASS_LAVA";
	interfacekey[INTERFACEKEY_HOTKEY_BUILDING_FURNACE_KILN_LAVA].tokenname="HOTKEY_BUILDING_FURNACE_KILN_LAVA";
	interfacekey[INTERFACEKEY_D_ONESTEP].tokenname="D_ONESTEP";
	interfacekey[INTERFACEKEY_D_PAUSE].tokenname="D_PAUSE";
	interfacekey[INTERFACEKEY_D_DEPOT].tokenname="D_DEPOT";
	interfacekey[INTERFACEKEY_D_HOT_KEYS].tokenname="D_HOT_KEYS";
	interfacekey[INTERFACEKEY_D_HOTKEY1].tokenname="D_HOTKEY1";
	interfacekey[INTERFACEKEY_D_HOTKEY2].tokenname="D_HOTKEY2";
	interfacekey[INTERFACEKEY_D_HOTKEY3].tokenname="D_HOTKEY3";
	interfacekey[INTERFACEKEY_D_HOTKEY4].tokenname="D_HOTKEY4";
	interfacekey[INTERFACEKEY_D_HOTKEY5].tokenname="D_HOTKEY5";
	interfacekey[INTERFACEKEY_D_HOTKEY6].tokenname="D_HOTKEY6";
	interfacekey[INTERFACEKEY_D_HOTKEY7].tokenname="D_HOTKEY7";
	interfacekey[INTERFACEKEY_D_HOTKEY8].tokenname="D_HOTKEY8";
	interfacekey[INTERFACEKEY_D_HOTKEY9].tokenname="D_HOTKEY9";
	interfacekey[INTERFACEKEY_D_HOTKEY10].tokenname="D_HOTKEY10";
	interfacekey[INTERFACEKEY_D_HOTKEY11].tokenname="D_HOTKEY11";
	interfacekey[INTERFACEKEY_D_HOTKEY12].tokenname="D_HOTKEY12";
	interfacekey[INTERFACEKEY_D_HOTKEY13].tokenname="D_HOTKEY13";
	interfacekey[INTERFACEKEY_D_HOTKEY14].tokenname="D_HOTKEY14";
	interfacekey[INTERFACEKEY_D_HOTKEY15].tokenname="D_HOTKEY15";
	interfacekey[INTERFACEKEY_D_HOTKEY16].tokenname="D_HOTKEY16";
	interfacekey[INTERFACEKEY_D_HOTKEY_CHANGE_NAME].tokenname="D_HOTKEY_CHANGE_NAME";
	interfacekey[INTERFACEKEY_D_HOTKEY_ZOOM].tokenname="D_HOTKEY_ZOOM";
	interfacekey[INTERFACEKEY_D_ANNOUNCE].tokenname="D_ANNOUNCE";
	interfacekey[INTERFACEKEY_D_BUILDING].tokenname="D_BUILDING";
	interfacekey[INTERFACEKEY_D_CIVLIST].tokenname="D_CIVLIST";
	interfacekey[INTERFACEKEY_D_DESIGNATE].tokenname="D_DESIGNATE";
	interfacekey[INTERFACEKEY_D_ARTLIST].tokenname="D_ARTLIST";
	interfacekey[INTERFACEKEY_D_NOBLES].tokenname="D_NOBLES";
	interfacekey[INTERFACEKEY_D_ORDERS].tokenname="D_ORDERS";
	interfacekey[INTERFACEKEY_D_MILITARY].tokenname="D_MILITARY";
	interfacekey[INTERFACEKEY_D_ROOMS].tokenname="D_ROOMS";
	interfacekey[INTERFACEKEY_BUILDINGLIST_ZOOM_T].tokenname="BUILDINGLIST_ZOOM_T";
	interfacekey[INTERFACEKEY_BUILDINGLIST_ZOOM_Q].tokenname="BUILDINGLIST_ZOOM_Q";
	interfacekey[INTERFACEKEY_D_SQUADS].tokenname="D_SQUADS";
	interfacekey[INTERFACEKEY_D_STOCKPILES].tokenname="D_STOCKPILES";
	interfacekey[INTERFACEKEY_D_CIVZONE].tokenname="D_CIVZONE";
	interfacekey[INTERFACEKEY_D_VIEWUNIT].tokenname="D_VIEWUNIT";
	interfacekey[INTERFACEKEY_D_JOBLIST].tokenname="D_JOBLIST";
	interfacekey[INTERFACEKEY_D_UNITLIST].tokenname="D_UNITLIST";
	interfacekey[INTERFACEKEY_D_LOOK].tokenname="D_LOOK";
	interfacekey[INTERFACEKEY_D_NOTE].tokenname="D_NOTE";
	interfacekey[INTERFACEKEY_D_NOTE_PLACE].tokenname="D_NOTE_PLACE";
	interfacekey[INTERFACEKEY_D_NOTE_DELETE].tokenname="D_NOTE_DELETE";
	interfacekey[INTERFACEKEY_D_NOTE_ENTER].tokenname="D_NOTE_ENTER";
	interfacekey[INTERFACEKEY_D_NOTE_ADOPT_SYMBOL].tokenname="D_NOTE_ADOPT_SYMBOL";
	interfacekey[INTERFACEKEY_D_NOTE_CHANGE_SELECTION].tokenname="D_NOTE_CHANGE_SELECTION";
	interfacekey[INTERFACEKEY_D_BUILDJOB].tokenname="D_BUILDJOB";
	interfacekey[INTERFACEKEY_D_STATUS].tokenname="D_STATUS";
	interfacekey[INTERFACEKEY_D_BUILDITEM].tokenname="D_BUILDITEM";
	interfacekey[INTERFACEKEY_D_VISUALIZE].tokenname="D_VISUALIZE";
	interfacekey[INTERFACEKEY_D_BITEM_FORBID].tokenname="D_BITEM_FORBID";
	interfacekey[INTERFACEKEY_D_BITEM_DUMP].tokenname="D_BITEM_DUMP";
	interfacekey[INTERFACEKEY_D_BITEM_HIDE].tokenname="D_BITEM_HIDE";
	interfacekey[INTERFACEKEY_D_BITEM_MELT].tokenname="D_BITEM_MELT";
	interfacekey[INTERFACEKEY_D_LOOK_FORBID].tokenname="D_LOOK_FORBID";
	interfacekey[INTERFACEKEY_D_LOOK_DUMP].tokenname="D_LOOK_DUMP";
	interfacekey[INTERFACEKEY_D_LOOK_MELT].tokenname="D_LOOK_MELT";
	interfacekey[INTERFACEKEY_D_LOOK_HIDE].tokenname="D_LOOK_HIDE";
	interfacekey[INTERFACEKEY_A_ENTER_NAME].tokenname="A_ENTER_NAME";
	interfacekey[INTERFACEKEY_A_CUST_NAME].tokenname="A_CUST_NAME";
	interfacekey[INTERFACEKEY_A_RANDOM_NAME].tokenname="A_RANDOM_NAME";
	interfacekey[INTERFACEKEY_A_CHANGE_GENDER].tokenname="A_CHANGE_GENDER";
	interfacekey[INTERFACEKEY_A_END_TRAVEL].tokenname="A_END_TRAVEL";
	interfacekey[INTERFACEKEY_A_TRAVEL_CLOUDS].tokenname="A_TRAVEL_CLOUDS";
	interfacekey[INTERFACEKEY_A_LOG].tokenname="A_LOG";
	interfacekey[INTERFACEKEY_A_TRAVEL_LOG].tokenname="A_TRAVEL_LOG";
	interfacekey[INTERFACEKEY_A_LOG_TASKS].tokenname="A_LOG_TASKS";
	interfacekey[INTERFACEKEY_A_LOG_ENTITIES].tokenname="A_LOG_ENTITIES";
	interfacekey[INTERFACEKEY_A_LOG_SITES].tokenname="A_LOG_SITES";
	interfacekey[INTERFACEKEY_A_LOG_SUBREGIONS].tokenname="A_LOG_SUBREGIONS";
	interfacekey[INTERFACEKEY_A_LOG_ZOOM_CURRENT_LOCATION].tokenname="A_LOG_ZOOM_CURRENT_LOCATION";
	interfacekey[INTERFACEKEY_A_LOG_ZOOM_SELECTED].tokenname="A_LOG_ZOOM_SELECTED";
	interfacekey[INTERFACEKEY_A_LOG_LINE].tokenname="A_LOG_LINE";
	interfacekey[INTERFACEKEY_A_LOG_MAP].tokenname="A_LOG_MAP";
	interfacekey[INTERFACEKEY_ORDERS_AUTOFORBID].tokenname="ORDERS_AUTOFORBID";
	interfacekey[INTERFACEKEY_ORDERS_FORBID_PROJECTILE].tokenname="ORDERS_FORBID_PROJECTILE";
	interfacekey[INTERFACEKEY_ORDERS_FORBID_YOUR_CORPSE].tokenname="ORDERS_FORBID_YOUR_CORPSE";
	interfacekey[INTERFACEKEY_ORDERS_FORBID_YOUR_ITEMS].tokenname="ORDERS_FORBID_YOUR_ITEMS";
	interfacekey[INTERFACEKEY_ORDERS_FORBID_OTHER_CORPSE].tokenname="ORDERS_FORBID_OTHER_CORPSE";
	interfacekey[INTERFACEKEY_ORDERS_FORBID_OTHER_ITEMS].tokenname="ORDERS_FORBID_OTHER_ITEMS";
	interfacekey[INTERFACEKEY_ORDERS_REFUSE_GATHER].tokenname="ORDERS_REFUSE_GATHER";
	interfacekey[INTERFACEKEY_ORDERS_REFUSE_OUTSIDE].tokenname="ORDERS_REFUSE_OUTSIDE";
	interfacekey[INTERFACEKEY_ORDERS_REFUSE_DUMP_CORPSE].tokenname="ORDERS_REFUSE_DUMP_CORPSE";
	interfacekey[INTERFACEKEY_ORDERS_REFUSE_DUMP_SKULL].tokenname="ORDERS_REFUSE_DUMP_SKULL";
	interfacekey[INTERFACEKEY_ORDERS_REFUSE_DUMP_SKIN].tokenname="ORDERS_REFUSE_DUMP_SKIN";
	interfacekey[INTERFACEKEY_ORDERS_REFUSE_DUMP_BONE].tokenname="ORDERS_REFUSE_DUMP_BONE";
	interfacekey[INTERFACEKEY_ORDERS_REFUSE_DUMP_SHELL].tokenname="ORDERS_REFUSE_DUMP_SHELL";
	interfacekey[INTERFACEKEY_ORDERS_REFUSE_DUMP_OTHER].tokenname="ORDERS_REFUSE_DUMP_OTHER";
	interfacekey[INTERFACEKEY_ORDERS_GATHER_FURNITURE].tokenname="ORDERS_GATHER_FURNITURE";
	interfacekey[INTERFACEKEY_ORDERS_GATHER_ANIMALS].tokenname="ORDERS_GATHER_ANIMALS";
	interfacekey[INTERFACEKEY_ORDERS_GATHER_FOOD].tokenname="ORDERS_GATHER_FOOD";
	interfacekey[INTERFACEKEY_ORDERS_GATHER_BODIES].tokenname="ORDERS_GATHER_BODIES";
	interfacekey[INTERFACEKEY_ORDERS_REFUSE].tokenname="ORDERS_REFUSE";
	interfacekey[INTERFACEKEY_ORDERS_GATHER_STONE].tokenname="ORDERS_GATHER_STONE";
	interfacekey[INTERFACEKEY_ORDERS_GATHER_WOOD].tokenname="ORDERS_GATHER_WOOD";
	interfacekey[INTERFACEKEY_ORDERS_ALL_HARVEST].tokenname="ORDERS_ALL_HARVEST";
	interfacekey[INTERFACEKEY_ORDERS_INDOORS].tokenname="ORDERS_INDOORS";
	interfacekey[INTERFACEKEY_ORDERS_SAMEPILE].tokenname="ORDERS_SAMEPILE";
	interfacekey[INTERFACEKEY_ORDERS_MIXFOODS].tokenname="ORDERS_MIXFOODS";
	interfacekey[INTERFACEKEY_ORDERS_EXCEPTIONS].tokenname="ORDERS_EXCEPTIONS";
	interfacekey[INTERFACEKEY_ORDERS_LOOM].tokenname="ORDERS_LOOM";
	interfacekey[INTERFACEKEY_ORDERS_DYED_CLOTH].tokenname="ORDERS_DYED_CLOTH";
	interfacekey[INTERFACEKEY_ORDERS_WORKSHOP].tokenname="ORDERS_WORKSHOP";
	interfacekey[INTERFACEKEY_ORDERS_COLLECT_WEB].tokenname="ORDERS_COLLECT_WEB";
	interfacekey[INTERFACEKEY_ORDERS_SLAUGHTER].tokenname="ORDERS_SLAUGHTER";
	interfacekey[INTERFACEKEY_ORDERS_BUTCHER].tokenname="ORDERS_BUTCHER";
	interfacekey[INTERFACEKEY_ORDERS_TAN].tokenname="ORDERS_TAN";
	interfacekey[INTERFACEKEY_ORDERS_ZONE].tokenname="ORDERS_ZONE";
	interfacekey[INTERFACEKEY_ORDERS_ZONE_DRINKING].tokenname="ORDERS_ZONE_DRINKING";
	interfacekey[INTERFACEKEY_ORDERS_ZONE_FISHING].tokenname="ORDERS_ZONE_FISHING";
	interfacekey[INTERFACEKEY_DESTROYBUILDING].tokenname="DESTROYBUILDING";
	interfacekey[INTERFACEKEY_SUSPENDBUILDING].tokenname="SUSPENDBUILDING";
	interfacekey[INTERFACEKEY_MENU_CONFIRM].tokenname="MENU_CONFIRM";
	interfacekey[INTERFACEKEY_SAVE_BINDINGS].tokenname="SAVE_BINDINGS";
	interfacekey[INTERFACEKEY_CHANGE_BINDINGS].tokenname="CHANGE_BINDINGS";
	interfacekey[INTERFACEKEY_HOTKEY_ALCHEMIST_SOAP].tokenname="HOTKEY_ALCHEMIST_SOAP";
	interfacekey[INTERFACEKEY_HOTKEY_STILL_BREW].tokenname="HOTKEY_STILL_BREW";
	interfacekey[INTERFACEKEY_HOTKEY_STILL_EXTRACT].tokenname="HOTKEY_STILL_EXTRACT";
	interfacekey[INTERFACEKEY_HOTKEY_LOOM_COLLECT_SILK].tokenname="HOTKEY_LOOM_COLLECT_SILK";
	interfacekey[INTERFACEKEY_HOTKEY_LOOM_WEAVE_CLOTH].tokenname="HOTKEY_LOOM_WEAVE_CLOTH";
	interfacekey[INTERFACEKEY_HOTKEY_LOOM_WEAVE_SILK].tokenname="HOTKEY_LOOM_WEAVE_SILK";
	interfacekey[INTERFACEKEY_HOTKEY_LOOM_WEAVE_METAL].tokenname="HOTKEY_LOOM_WEAVE_METAL";
	interfacekey[INTERFACEKEY_HOTKEY_KITCHEN_COOK_2].tokenname="HOTKEY_KITCHEN_COOK_2";
	interfacekey[INTERFACEKEY_HOTKEY_KITCHEN_COOK_3].tokenname="HOTKEY_KITCHEN_COOK_3";
	interfacekey[INTERFACEKEY_HOTKEY_KITCHEN_COOK_4].tokenname="HOTKEY_KITCHEN_COOK_4";
	interfacekey[INTERFACEKEY_HOTKEY_KITCHEN_RENDER_FAT].tokenname="HOTKEY_KITCHEN_RENDER_FAT";
	interfacekey[INTERFACEKEY_HOTKEY_FARMER_PROCESS].tokenname="HOTKEY_FARMER_PROCESS";
	interfacekey[INTERFACEKEY_HOTKEY_FARMER_PROCESS_VIAL].tokenname="HOTKEY_FARMER_PROCESS_VIAL";
	interfacekey[INTERFACEKEY_HOTKEY_FARMER_PROCESS_BAG].tokenname="HOTKEY_FARMER_PROCESS_BAG";
	interfacekey[INTERFACEKEY_HOTKEY_FARMER_PROCESS_BARREL].tokenname="HOTKEY_FARMER_PROCESS_BARREL";
	interfacekey[INTERFACEKEY_HOTKEY_FARMER_CHEESE].tokenname="HOTKEY_FARMER_CHEESE";
	interfacekey[INTERFACEKEY_HOTKEY_FARMER_MILK].tokenname="HOTKEY_FARMER_MILK";
	interfacekey[INTERFACEKEY_HOTKEY_MILL_MILL].tokenname="HOTKEY_MILL_MILL";
	interfacekey[INTERFACEKEY_HOTKEY_KENNEL_TRAIN_HUNT].tokenname="HOTKEY_KENNEL_TRAIN_HUNT";
	interfacekey[INTERFACEKEY_HOTKEY_KENNEL_CATCH_VERMIN].tokenname="HOTKEY_KENNEL_CATCH_VERMIN";
	interfacekey[INTERFACEKEY_HOTKEY_KENNEL_TAME_VERMIN].tokenname="HOTKEY_KENNEL_TAME_VERMIN";
	interfacekey[INTERFACEKEY_HOTKEY_KENNEL_TAME_UNIT].tokenname="HOTKEY_KENNEL_TAME_UNIT";
	interfacekey[INTERFACEKEY_HOTKEY_KENNEL_TRAIN_WAR].tokenname="HOTKEY_KENNEL_TRAIN_WAR";
	interfacekey[INTERFACEKEY_HOTKEY_FISHERY_PROCESS].tokenname="HOTKEY_FISHERY_PROCESS";
	interfacekey[INTERFACEKEY_HOTKEY_FISHERY_EXTRACT].tokenname="HOTKEY_FISHERY_EXTRACT";
	interfacekey[INTERFACEKEY_HOTKEY_FISHERY_CATCH].tokenname="HOTKEY_FISHERY_CATCH";
	interfacekey[INTERFACEKEY_HOTKEY_BUTCHER_BUTCHER].tokenname="HOTKEY_BUTCHER_BUTCHER";
	interfacekey[INTERFACEKEY_HOTKEY_BUTCHER_EXTRACT].tokenname="HOTKEY_BUTCHER_EXTRACT";
	interfacekey[INTERFACEKEY_HOTKEY_BUTCHER_CATCH].tokenname="HOTKEY_BUTCHER_CATCH";
	interfacekey[INTERFACEKEY_HOTKEY_TANNER_TAN].tokenname="HOTKEY_TANNER_TAN";
	interfacekey[INTERFACEKEY_HOTKEY_DYER_THREAD].tokenname="HOTKEY_DYER_THREAD";
	interfacekey[INTERFACEKEY_HOTKEY_DYER_CLOTH].tokenname="HOTKEY_DYER_CLOTH";
	interfacekey[INTERFACEKEY_HOTKEY_JEWELER_FURNITURE].tokenname="HOTKEY_JEWELER_FURNITURE";
	interfacekey[INTERFACEKEY_HOTKEY_JEWELER_FINISHED].tokenname="HOTKEY_JEWELER_FINISHED";
	interfacekey[INTERFACEKEY_HOTKEY_JEWELER_AMMO].tokenname="HOTKEY_JEWELER_AMMO";
	interfacekey[INTERFACEKEY_HOTKEY_JEWELER_CUT].tokenname="HOTKEY_JEWELER_CUT";
	interfacekey[INTERFACEKEY_HOTKEY_JEWELER_ENCRUST].tokenname="HOTKEY_JEWELER_ENCRUST";
	interfacekey[INTERFACEKEY_HOTKEY_MECHANIC_PARTS].tokenname="HOTKEY_MECHANIC_PARTS";
	interfacekey[INTERFACEKEY_HOTKEY_MASON_ARMORSTAND].tokenname="HOTKEY_MASON_ARMORSTAND";
	interfacekey[INTERFACEKEY_HOTKEY_MASON_BLOCKS].tokenname="HOTKEY_MASON_BLOCKS";
	interfacekey[INTERFACEKEY_HOTKEY_MASON_CHAIR].tokenname="HOTKEY_MASON_CHAIR";
	interfacekey[INTERFACEKEY_HOTKEY_MASON_COFFIN].tokenname="HOTKEY_MASON_COFFIN";
	interfacekey[INTERFACEKEY_HOTKEY_MASON_DOOR].tokenname="HOTKEY_MASON_DOOR";
	interfacekey[INTERFACEKEY_HOTKEY_MASON_FLOODGATE].tokenname="HOTKEY_MASON_FLOODGATE";
	interfacekey[INTERFACEKEY_HOTKEY_MASON_HATCH_COVER].tokenname="HOTKEY_MASON_HATCH_COVER";
	interfacekey[INTERFACEKEY_HOTKEY_MASON_GRATE].tokenname="HOTKEY_MASON_GRATE";
	interfacekey[INTERFACEKEY_HOTKEY_MASON_CABINET].tokenname="HOTKEY_MASON_CABINET";
	interfacekey[INTERFACEKEY_HOTKEY_MASON_BOX].tokenname="HOTKEY_MASON_BOX";
	interfacekey[INTERFACEKEY_HOTKEY_MASON_STATUE].tokenname="HOTKEY_MASON_STATUE";
	interfacekey[INTERFACEKEY_HOTKEY_MASON_QUERN].tokenname="HOTKEY_MASON_QUERN";
	interfacekey[INTERFACEKEY_HOTKEY_MASON_MILLSTONE].tokenname="HOTKEY_MASON_MILLSTONE";
	interfacekey[INTERFACEKEY_HOTKEY_MASON_TABLE].tokenname="HOTKEY_MASON_TABLE";
	interfacekey[INTERFACEKEY_HOTKEY_MASON_WEAPONRACK].tokenname="HOTKEY_MASON_WEAPONRACK";
	interfacekey[INTERFACEKEY_HOTKEY_TRAP_BRIDGE].tokenname="HOTKEY_TRAP_BRIDGE";
	interfacekey[INTERFACEKEY_HOTKEY_TRAP_GEAR_ASSEMBLY].tokenname="HOTKEY_TRAP_GEAR_ASSEMBLY";
	interfacekey[INTERFACEKEY_HOTKEY_TRAP_DOOR].tokenname="HOTKEY_TRAP_DOOR";
	interfacekey[INTERFACEKEY_HOTKEY_TRAP_FLOODGATE].tokenname="HOTKEY_TRAP_FLOODGATE";
	interfacekey[INTERFACEKEY_HOTKEY_TRAP_SPIKE].tokenname="HOTKEY_TRAP_SPIKE";
	interfacekey[INTERFACEKEY_HOTKEY_TRAP_HATCH].tokenname="HOTKEY_TRAP_HATCH";
	interfacekey[INTERFACEKEY_HOTKEY_TRAP_GRATE_WALL].tokenname="HOTKEY_TRAP_GRATE_WALL";
	interfacekey[INTERFACEKEY_HOTKEY_TRAP_GRATE_FLOOR].tokenname="HOTKEY_TRAP_GRATE_FLOOR";
	interfacekey[INTERFACEKEY_HOTKEY_TRAP_BARS_VERTICAL].tokenname="HOTKEY_TRAP_BARS_VERTICAL";
	interfacekey[INTERFACEKEY_HOTKEY_TRAP_BARS_FLOOR].tokenname="HOTKEY_TRAP_BARS_FLOOR";
	interfacekey[INTERFACEKEY_HOTKEY_TRAP_SUPPORT].tokenname="HOTKEY_TRAP_SUPPORT";
	interfacekey[INTERFACEKEY_HOTKEY_TRAP_CHAIN].tokenname="HOTKEY_TRAP_CHAIN";
	interfacekey[INTERFACEKEY_HOTKEY_TRAP_CAGE].tokenname="HOTKEY_TRAP_CAGE";
	interfacekey[INTERFACEKEY_HOTKEY_TRAP_PULL_LEVER].tokenname="HOTKEY_TRAP_PULL_LEVER";
	interfacekey[INTERFACEKEY_BUILDJOB_ADD].tokenname="BUILDJOB_ADD";
	interfacekey[INTERFACEKEY_BUILDJOB_CANCEL].tokenname="BUILDJOB_CANCEL";
	interfacekey[INTERFACEKEY_BUILDJOB_PROMOTE].tokenname="BUILDJOB_PROMOTE";
	interfacekey[INTERFACEKEY_BUILDJOB_REPEAT].tokenname="BUILDJOB_REPEAT";
	interfacekey[INTERFACEKEY_BUILDJOB_SUSPEND].tokenname="BUILDJOB_SUSPEND";
	interfacekey[INTERFACEKEY_BUILDJOB_WORKSHOP_PROFILE].tokenname="BUILDJOB_WORKSHOP_PROFILE";
	interfacekey[INTERFACEKEY_BUILDJOB_WELL_FREE].tokenname="BUILDJOB_WELL_FREE";
	interfacekey[INTERFACEKEY_BUILDJOB_WELL_SIZE].tokenname="BUILDJOB_WELL_SIZE";
	interfacekey[INTERFACEKEY_BUILDJOB_TARGET_FREE].tokenname="BUILDJOB_TARGET_FREE";
	interfacekey[INTERFACEKEY_BUILDJOB_TARGET_SIZE].tokenname="BUILDJOB_TARGET_SIZE";
	interfacekey[INTERFACEKEY_BUILDJOB_TARGET_DOWN].tokenname="BUILDJOB_TARGET_DOWN";
	interfacekey[INTERFACEKEY_BUILDJOB_TARGET_UP].tokenname="BUILDJOB_TARGET_UP";
	interfacekey[INTERFACEKEY_BUILDJOB_TARGET_RIGHT].tokenname="BUILDJOB_TARGET_RIGHT";
	interfacekey[INTERFACEKEY_BUILDJOB_TARGET_LEFT].tokenname="BUILDJOB_TARGET_LEFT";
	interfacekey[INTERFACEKEY_BUILDJOB_STATUE_ASSIGN].tokenname="BUILDJOB_STATUE_ASSIGN";
	interfacekey[INTERFACEKEY_BUILDJOB_STATUE_FREE].tokenname="BUILDJOB_STATUE_FREE";
	interfacekey[INTERFACEKEY_BUILDJOB_STATUE_SIZE].tokenname="BUILDJOB_STATUE_SIZE";
	interfacekey[INTERFACEKEY_BUILDJOB_CAGE_JUSTICE].tokenname="BUILDJOB_CAGE_JUSTICE";
	interfacekey[INTERFACEKEY_BUILDJOB_CAGE_FREE].tokenname="BUILDJOB_CAGE_FREE";
	interfacekey[INTERFACEKEY_BUILDJOB_CAGE_SIZE].tokenname="BUILDJOB_CAGE_SIZE";
	interfacekey[INTERFACEKEY_BUILDJOB_CAGE_ASSIGN_OCC].tokenname="BUILDJOB_CAGE_ASSIGN_OCC";
	interfacekey[INTERFACEKEY_BUILDJOB_CAGE_WATER].tokenname="BUILDJOB_CAGE_WATER";
	interfacekey[INTERFACEKEY_BUILDJOB_CAGE_ASSIGN].tokenname="BUILDJOB_CAGE_ASSIGN";
	interfacekey[INTERFACEKEY_BUILDJOB_CHAIN_ASSIGN_OCC].tokenname="BUILDJOB_CHAIN_ASSIGN_OCC";
	interfacekey[INTERFACEKEY_BUILDJOB_CHAIN_JUSTICE].tokenname="BUILDJOB_CHAIN_JUSTICE";
	interfacekey[INTERFACEKEY_BUILDJOB_CHAIN_ASSIGN].tokenname="BUILDJOB_CHAIN_ASSIGN";
	interfacekey[INTERFACEKEY_BUILDJOB_CHAIN_FREE].tokenname="BUILDJOB_CHAIN_FREE";
	interfacekey[INTERFACEKEY_BUILDJOB_CHAIN_SIZE].tokenname="BUILDJOB_CHAIN_SIZE";
	interfacekey[INTERFACEKEY_BUILDJOB_SIEGE_FIRING].tokenname="BUILDJOB_SIEGE_FIRING";
	interfacekey[INTERFACEKEY_BUILDJOB_SIEGE_ORIENT].tokenname="BUILDJOB_SIEGE_ORIENT";
	interfacekey[INTERFACEKEY_BUILDJOB_DOOR_INTERNAL].tokenname="BUILDJOB_DOOR_INTERNAL";
	interfacekey[INTERFACEKEY_BUILDJOB_DOOR_LOCK].tokenname="BUILDJOB_DOOR_LOCK";
	interfacekey[INTERFACEKEY_BUILDJOB_DOOR_AJAR].tokenname="BUILDJOB_DOOR_AJAR";
	interfacekey[INTERFACEKEY_BUILDJOB_COFFIN_ASSIGN].tokenname="BUILDJOB_COFFIN_ASSIGN";
	interfacekey[INTERFACEKEY_BUILDJOB_COFFIN_FREE].tokenname="BUILDJOB_COFFIN_FREE";
	interfacekey[INTERFACEKEY_BUILDJOB_COFFIN_SIZE].tokenname="BUILDJOB_COFFIN_SIZE";
	interfacekey[INTERFACEKEY_BUILDJOB_COFFIN_BURIAL].tokenname="BUILDJOB_COFFIN_BURIAL";
	interfacekey[INTERFACEKEY_BUILDJOB_COFFIN_CIV].tokenname="BUILDJOB_COFFIN_CIV";
	interfacekey[INTERFACEKEY_BUILDJOB_COFFIN_PET].tokenname="BUILDJOB_COFFIN_PET";
	interfacekey[INTERFACEKEY_BUILDJOB_CHAIR_ASSIGN].tokenname="BUILDJOB_CHAIR_ASSIGN";
	interfacekey[INTERFACEKEY_BUILDJOB_CHAIR_FREE].tokenname="BUILDJOB_CHAIR_FREE";
	interfacekey[INTERFACEKEY_BUILDJOB_CHAIR_SIZE].tokenname="BUILDJOB_CHAIR_SIZE";
	interfacekey[INTERFACEKEY_BUILDJOB_TABLE_ASSIGN].tokenname="BUILDJOB_TABLE_ASSIGN";
	interfacekey[INTERFACEKEY_BUILDJOB_TABLE_HALL].tokenname="BUILDJOB_TABLE_HALL";
	interfacekey[INTERFACEKEY_BUILDJOB_TABLE_FREE].tokenname="BUILDJOB_TABLE_FREE";
	interfacekey[INTERFACEKEY_BUILDJOB_TABLE_SIZE].tokenname="BUILDJOB_TABLE_SIZE";
	interfacekey[INTERFACEKEY_BUILDJOB_BED_ASSIGN].tokenname="BUILDJOB_BED_ASSIGN";
	interfacekey[INTERFACEKEY_BUILDJOB_BED_FREE].tokenname="BUILDJOB_BED_FREE";
	interfacekey[INTERFACEKEY_BUILDJOB_BED_BARRACKS].tokenname="BUILDJOB_BED_BARRACKS";
	interfacekey[INTERFACEKEY_BUILDJOB_BED_RENT].tokenname="BUILDJOB_BED_RENT";
	interfacekey[INTERFACEKEY_BUILDJOB_BED_SIZE].tokenname="BUILDJOB_BED_SIZE";
	interfacekey[INTERFACEKEY_BUILDJOB_DEPOT_BRING].tokenname="BUILDJOB_DEPOT_BRING";
	interfacekey[INTERFACEKEY_BUILDJOB_DEPOT_TRADE].tokenname="BUILDJOB_DEPOT_TRADE";
	interfacekey[INTERFACEKEY_BUILDJOB_DEPOT_REQUEST_TRADER].tokenname="BUILDJOB_DEPOT_REQUEST_TRADER";
	interfacekey[INTERFACEKEY_BUILDJOB_DEPOT_BROKER_ONLY].tokenname="BUILDJOB_DEPOT_BROKER_ONLY";
	interfacekey[INTERFACEKEY_BUILDJOB_ANIMALTRAP_BAIT_NONE].tokenname="BUILDJOB_ANIMALTRAP_BAIT_NONE";
	interfacekey[INTERFACEKEY_BUILDJOB_ANIMALTRAP_BAIT_MEAT].tokenname="BUILDJOB_ANIMALTRAP_BAIT_MEAT";
	interfacekey[INTERFACEKEY_BUILDJOB_ANIMALTRAP_BAIT_FISH].tokenname="BUILDJOB_ANIMALTRAP_BAIT_FISH";
	interfacekey[INTERFACEKEY_BUILDJOB_ANIMALTRAP_BAIT_GEM].tokenname="BUILDJOB_ANIMALTRAP_BAIT_GEM";
	interfacekey[INTERFACEKEY_BUILDJOB_FARM_FALLOW].tokenname="BUILDJOB_FARM_FALLOW";
	interfacekey[INTERFACEKEY_BUILDJOB_FARM_FERTILIZE].tokenname="BUILDJOB_FARM_FERTILIZE";
	interfacekey[INTERFACEKEY_BUILDJOB_FARM_SEASFERT].tokenname="BUILDJOB_FARM_SEASFERT";
	interfacekey[INTERFACEKEY_BUILDJOB_FARM_SPRING].tokenname="BUILDJOB_FARM_SPRING";
	interfacekey[INTERFACEKEY_BUILDJOB_FARM_SUMMER].tokenname="BUILDJOB_FARM_SUMMER";
	interfacekey[INTERFACEKEY_BUILDJOB_FARM_AUTUMN].tokenname="BUILDJOB_FARM_AUTUMN";
	interfacekey[INTERFACEKEY_BUILDJOB_FARM_WINTER].tokenname="BUILDJOB_FARM_WINTER";
	interfacekey[INTERFACEKEY_BUILDJOB_STOCKPILE_MASTER].tokenname="BUILDJOB_STOCKPILE_MASTER";
	interfacekey[INTERFACEKEY_BUILDJOB_STOCKPILE_DELETE_CHILD].tokenname="BUILDJOB_STOCKPILE_DELETE_CHILD";
	interfacekey[INTERFACEKEY_BUILDJOB_STOCKPILE_SETTINGS].tokenname="BUILDJOB_STOCKPILE_SETTINGS";
	interfacekey[INTERFACEKEY_BUILDJOB_STOCKPILE_BARREL_UP].tokenname="BUILDJOB_STOCKPILE_BARREL_UP";
	interfacekey[INTERFACEKEY_BUILDJOB_STOCKPILE_BARREL_DOWN].tokenname="BUILDJOB_STOCKPILE_BARREL_DOWN";
	interfacekey[INTERFACEKEY_BUILDJOB_STOCKPILE_BARREL_ZERO].tokenname="BUILDJOB_STOCKPILE_BARREL_ZERO";
	interfacekey[INTERFACEKEY_BUILDJOB_STOCKPILE_BARREL_MAX].tokenname="BUILDJOB_STOCKPILE_BARREL_MAX";
	interfacekey[INTERFACEKEY_BUILDJOB_STOCKPILE_BIN_UP].tokenname="BUILDJOB_STOCKPILE_BIN_UP";
	interfacekey[INTERFACEKEY_BUILDJOB_STOCKPILE_BIN_DOWN].tokenname="BUILDJOB_STOCKPILE_BIN_DOWN";
	interfacekey[INTERFACEKEY_BUILDJOB_STOCKPILE_BIN_ZERO].tokenname="BUILDJOB_STOCKPILE_BIN_ZERO";
	interfacekey[INTERFACEKEY_BUILDJOB_STOCKPILE_BIN_MAX].tokenname="BUILDJOB_STOCKPILE_BIN_MAX";
	interfacekey[INTERFACEKEY_BUILDJOB_RACK_MAT_BONE].tokenname="BUILDJOB_RACK_MAT_BONE";
	interfacekey[INTERFACEKEY_BUILDJOB_RACK_MAT_BRONZE].tokenname="BUILDJOB_RACK_MAT_BRONZE";
	interfacekey[INTERFACEKEY_BUILDJOB_RACK_MAT_COPPER].tokenname="BUILDJOB_RACK_MAT_COPPER";
	interfacekey[INTERFACEKEY_BUILDJOB_RACK_MAT_IRON].tokenname="BUILDJOB_RACK_MAT_IRON";
	interfacekey[INTERFACEKEY_BUILDJOB_RACK_MAT_STEEL].tokenname="BUILDJOB_RACK_MAT_STEEL";
	interfacekey[INTERFACEKEY_BUILDJOB_RACK_MAT_WOOD].tokenname="BUILDJOB_RACK_MAT_WOOD";
	interfacekey[INTERFACEKEY_BUILDJOB_RACK_MAT_METAL].tokenname="BUILDJOB_RACK_MAT_METAL";
	interfacekey[INTERFACEKEY_BUILDJOB_STAND_MAT_BONE].tokenname="BUILDJOB_STAND_MAT_BONE";
	interfacekey[INTERFACEKEY_BUILDJOB_STAND_MAT_LEATHER].tokenname="BUILDJOB_STAND_MAT_LEATHER";
	interfacekey[INTERFACEKEY_BUILDJOB_STAND_MAT_COPPER].tokenname="BUILDJOB_STAND_MAT_COPPER";
	interfacekey[INTERFACEKEY_BUILDJOB_STAND_MAT_BRONZE].tokenname="BUILDJOB_STAND_MAT_BRONZE";
	interfacekey[INTERFACEKEY_BUILDJOB_STAND_MAT_STEEL].tokenname="BUILDJOB_STAND_MAT_STEEL";
	interfacekey[INTERFACEKEY_BUILDJOB_STAND_MAT_IRON].tokenname="BUILDJOB_STAND_MAT_IRON";
	interfacekey[INTERFACEKEY_BUILDJOB_STAND_MAT_WOOD].tokenname="BUILDJOB_STAND_MAT_WOOD";
	interfacekey[INTERFACEKEY_BUILDJOB_STAND_MAT_METAL].tokenname="BUILDJOB_STAND_MAT_METAL";
	interfacekey[INTERFACEKEY_BUILDJOB_RACKSTAND_ASSIGN].tokenname="BUILDJOB_RACKSTAND_ASSIGN";
	interfacekey[INTERFACEKEY_BUILDJOB_RACKSTAND_FREE].tokenname="BUILDJOB_RACKSTAND_FREE";
	interfacekey[INTERFACEKEY_BUILDJOB_RACKSTAND_SIZE].tokenname="BUILDJOB_RACKSTAND_SIZE";
	interfacekey[INTERFACEKEY_BUILDJOB_RACKSTAND_ITEM].tokenname="BUILDJOB_RACKSTAND_ITEM";
	interfacekey[INTERFACEKEY_BUILDJOB_RACKSTAND_MAT].tokenname="BUILDJOB_RACKSTAND_MAT";
	interfacekey[INTERFACEKEY_BUILDJOB_RACKSTAND_DEFAULTS1].tokenname="BUILDJOB_RACKSTAND_DEFAULTS1";
	interfacekey[INTERFACEKEY_BUILDJOB_RACKSTAND_KILL1].tokenname="BUILDJOB_RACKSTAND_KILL1";
	interfacekey[INTERFACEKEY_BUILDJOB_RACKSTAND_DEFAULTS2].tokenname="BUILDJOB_RACKSTAND_DEFAULTS2";
	interfacekey[INTERFACEKEY_BUILDJOB_RACKSTAND_KILL2].tokenname="BUILDJOB_RACKSTAND_KILL2";
	interfacekey[INTERFACEKEY_STOCKPILE_SETTINGS_ENABLE].tokenname="STOCKPILE_SETTINGS_ENABLE";
	interfacekey[INTERFACEKEY_STOCKPILE_SETTINGS_DISABLE].tokenname="STOCKPILE_SETTINGS_DISABLE";
	interfacekey[INTERFACEKEY_STOCKPILE_SETTINGS_PERMIT_ALL].tokenname="STOCKPILE_SETTINGS_PERMIT_ALL";
	interfacekey[INTERFACEKEY_STOCKPILE_SETTINGS_FORBID_ALL].tokenname="STOCKPILE_SETTINGS_FORBID_ALL";
	interfacekey[INTERFACEKEY_STOCKPILE_SETTINGS_PERMIT_SUB].tokenname="STOCKPILE_SETTINGS_PERMIT_SUB";
	interfacekey[INTERFACEKEY_STOCKPILE_SETTINGS_FORBID_SUB].tokenname="STOCKPILE_SETTINGS_FORBID_SUB";
	interfacekey[INTERFACEKEY_STOCKPILE_SETTINGS_SPECIFIC1].tokenname="STOCKPILE_SETTINGS_SPECIFIC1";
	interfacekey[INTERFACEKEY_STOCKPILE_SETTINGS_SPECIFIC2].tokenname="STOCKPILE_SETTINGS_SPECIFIC2";
	interfacekey[INTERFACEKEY_SQUAD_LOCK].tokenname="SQUAD_LOCK";
	interfacekey[INTERFACEKEY_SQUAD_STATION].tokenname="SQUAD_STATION";
	interfacekey[INTERFACEKEY_SQUAD_CLEAR_PATROL].tokenname="SQUAD_CLEAR_PATROL";
	interfacekey[INTERFACEKEY_SQUAD_PATROL].tokenname="SQUAD_PATROL";
	interfacekey[INTERFACEKEY_SQUAD_ZOOM].tokenname="SQUAD_ZOOM";
	interfacekey[INTERFACEKEY_MOVIE_RECORD].tokenname="MOVIE_RECORD";
	interfacekey[INTERFACEKEY_MOVIE_PLAY].tokenname="MOVIE_PLAY";
	interfacekey[INTERFACEKEY_MOVIE_SAVE].tokenname="MOVIE_SAVE";
	interfacekey[INTERFACEKEY_MOVIE_LOAD].tokenname="MOVIE_LOAD";
	interfacekey[INTERFACEKEY_ASSIGNTRADE_VIEW].tokenname="ASSIGNTRADE_VIEW";
	interfacekey[INTERFACEKEY_ASSIGNTRADE_STRING].tokenname="ASSIGNTRADE_STRING";
	interfacekey[INTERFACEKEY_ASSIGNTRADE_EXCLUDE_PROHIBITED].tokenname="ASSIGNTRADE_EXCLUDE_PROHIBITED";
	interfacekey[INTERFACEKEY_ASSIGNTRADE_PENDING].tokenname="ASSIGNTRADE_PENDING";
	interfacekey[INTERFACEKEY_ASSIGNTRADE_SORT].tokenname="ASSIGNTRADE_SORT";
	interfacekey[INTERFACEKEY_NOBLELIST_REPLACE].tokenname="NOBLELIST_REPLACE";
	interfacekey[INTERFACEKEY_NOBLELIST_SETTINGS].tokenname="NOBLELIST_SETTINGS";
	interfacekey[INTERFACEKEY_NOBLELIST_VIEW_CANDIDATE].tokenname="NOBLELIST_VIEW_CANDIDATE";
	interfacekey[INTERFACEKEY_A_BARTER_VIEW].tokenname="A_BARTER_VIEW";
	interfacekey[INTERFACEKEY_A_BARTER_CURRENCY_1].tokenname="A_BARTER_CURRENCY_1";
	interfacekey[INTERFACEKEY_A_BARTER_CURRENCY_2].tokenname="A_BARTER_CURRENCY_2";
	interfacekey[INTERFACEKEY_A_BARTER_CURRENCY_3].tokenname="A_BARTER_CURRENCY_3";
	interfacekey[INTERFACEKEY_A_BARTER_CURRENCY_4].tokenname="A_BARTER_CURRENCY_4";
	interfacekey[INTERFACEKEY_A_BARTER_CURRENCY_5].tokenname="A_BARTER_CURRENCY_5";
	interfacekey[INTERFACEKEY_A_BARTER_CURRENCY_6].tokenname="A_BARTER_CURRENCY_6";
	interfacekey[INTERFACEKEY_A_BARTER_CURRENCY_7].tokenname="A_BARTER_CURRENCY_7";
	interfacekey[INTERFACEKEY_A_BARTER_CURRENCY_8].tokenname="A_BARTER_CURRENCY_8";
	interfacekey[INTERFACEKEY_A_BARTER_CURRENCY_9].tokenname="A_BARTER_CURRENCY_9";
	interfacekey[INTERFACEKEY_A_BARTER_TRADE].tokenname="A_BARTER_TRADE";
	interfacekey[INTERFACEKEY_TRADE_VIEW].tokenname="TRADE_VIEW";
	interfacekey[INTERFACEKEY_TRADE_TRADE].tokenname="TRADE_TRADE";
	interfacekey[INTERFACEKEY_TRADE_OFFER].tokenname="TRADE_OFFER";
	interfacekey[INTERFACEKEY_TRADE_SEIZE].tokenname="TRADE_SEIZE";
	interfacekey[INTERFACEKEY_STORES_VIEW].tokenname="STORES_VIEW";
	interfacekey[INTERFACEKEY_STORES_ZOOM].tokenname="STORES_ZOOM";
	interfacekey[INTERFACEKEY_STORES_FORBID].tokenname="STORES_FORBID";
	interfacekey[INTERFACEKEY_STORES_MELT].tokenname="STORES_MELT";
	interfacekey[INTERFACEKEY_STORES_DUMP].tokenname="STORES_DUMP";
	interfacekey[INTERFACEKEY_STORES_HIDE].tokenname="STORES_HIDE";
	interfacekey[INTERFACEKEY_MILITARY_ACTIVATE].tokenname="MILITARY_ACTIVATE";
	interfacekey[INTERFACEKEY_MILITARY_VIEW].tokenname="MILITARY_VIEW";
	interfacekey[INTERFACEKEY_MILITARY_WEAPON].tokenname="MILITARY_WEAPON";
	interfacekey[INTERFACEKEY_MILITARY_ZOOM].tokenname="MILITARY_ZOOM";
	interfacekey[INTERFACEKEY_SQUADINFO_FOOD].tokenname="SQUADINFO_FOOD";
	interfacekey[INTERFACEKEY_SQUADINFO_SLEEP].tokenname="SQUADINFO_SLEEP";
	interfacekey[INTERFACEKEY_SQUADINFO_WATER].tokenname="SQUADINFO_WATER";
	interfacekey[INTERFACEKEY_SQUADINFO_STAYCLOSE].tokenname="SQUADINFO_STAYCLOSE";
	interfacekey[INTERFACEKEY_SQUADINFO_ATTACKWILDS].tokenname="SQUADINFO_ATTACKWILDS";
	interfacekey[INTERFACEKEY_SQUADINFO_STANDDOWN].tokenname="SQUADINFO_STANDDOWN";
	interfacekey[INTERFACEKEY_SQUADINFO_ZOOM].tokenname="SQUADINFO_ZOOM";
	interfacekey[INTERFACEKEY_UNITJOB_REMOVE_CRE].tokenname="UNITJOB_REMOVE_CRE";
	interfacekey[INTERFACEKEY_UNITJOB_ZOOM_CRE].tokenname="UNITJOB_ZOOM_CRE";
	interfacekey[INTERFACEKEY_UNITJOB_ZOOM_BUILD].tokenname="UNITJOB_ZOOM_BUILD";
	interfacekey[INTERFACEKEY_UNITJOB_VIEW].tokenname="UNITJOB_VIEW";
	interfacekey[INTERFACEKEY_UNITJOB_MANAGER].tokenname="UNITJOB_MANAGER";
	interfacekey[INTERFACEKEY_MANAGER_NEW_ORDER].tokenname="MANAGER_NEW_ORDER";
	interfacekey[INTERFACEKEY_MANAGER_REMOVE].tokenname="MANAGER_REMOVE";
	interfacekey[INTERFACEKEY_MANAGER_PROMOTE].tokenname="MANAGER_PROMOTE";
	interfacekey[INTERFACEKEY_MANAGER_MAX].tokenname="MANAGER_MAX";
	interfacekey[INTERFACEKEY_MANAGER_WAGES].tokenname="MANAGER_WAGES";
	interfacekey[INTERFACEKEY_PET_BUTCHER].tokenname="PET_BUTCHER";
	interfacekey[INTERFACEKEY_KITCHEN_COOK].tokenname="KITCHEN_COOK";
	interfacekey[INTERFACEKEY_KITCHEN_BREW].tokenname="KITCHEN_BREW";
	interfacekey[INTERFACEKEY_SETUPGAME_NEW].tokenname="SETUPGAME_NEW";
	interfacekey[INTERFACEKEY_SETUPGAME_VIEW].tokenname="SETUPGAME_VIEW";
	interfacekey[INTERFACEKEY_SETUPGAME_CUSTOMIZE_UNIT].tokenname="SETUPGAME_CUSTOMIZE_UNIT";
	interfacekey[INTERFACEKEY_SETUPGAME_SAVE_PROFILE].tokenname="SETUPGAME_SAVE_PROFILE";
	interfacekey[INTERFACEKEY_SETUPGAME_SAVE_PROFILE_ABORT].tokenname="SETUPGAME_SAVE_PROFILE_ABORT";
	interfacekey[INTERFACEKEY_SETUPGAME_SAVE_PROFILE_GO].tokenname="SETUPGAME_SAVE_PROFILE_GO";
	interfacekey[INTERFACEKEY_SETUPGAME_VIEW_PROFILE_PROBLEMS].tokenname="SETUPGAME_VIEW_PROFILE_PROBLEMS";
	interfacekey[INTERFACEKEY_CIVZONE_REMOVE].tokenname="CIVZONE_REMOVE";
	interfacekey[INTERFACEKEY_CIVZONE_POND_OPTIONS].tokenname="CIVZONE_POND_OPTIONS";
	interfacekey[INTERFACEKEY_CIVZONE_WATER_SOURCE].tokenname="CIVZONE_WATER_SOURCE";
	interfacekey[INTERFACEKEY_CIVZONE_FISH].tokenname="CIVZONE_FISH";
	interfacekey[INTERFACEKEY_CIVZONE_DUMP].tokenname="CIVZONE_DUMP";
	interfacekey[INTERFACEKEY_CIVZONE_POND].tokenname="CIVZONE_POND";
	interfacekey[INTERFACEKEY_CIVZONE_SAND_COLLECT].tokenname="CIVZONE_SAND_COLLECT";
	interfacekey[INTERFACEKEY_CIVZONE_ACTIVE].tokenname="CIVZONE_ACTIVE";
	interfacekey[INTERFACEKEY_CIVZONE_MEETING].tokenname="CIVZONE_MEETING";
	interfacekey[INTERFACEKEY_CIVZONE_POND_WATER].tokenname="CIVZONE_POND_WATER";
	interfacekey[INTERFACEKEY_STOCKPILE_ANIMAL].tokenname="STOCKPILE_ANIMAL";
	interfacekey[INTERFACEKEY_STOCKPILE_FOOD].tokenname="STOCKPILE_FOOD";
	interfacekey[INTERFACEKEY_STOCKPILE_WEAPON].tokenname="STOCKPILE_WEAPON";
	interfacekey[INTERFACEKEY_STOCKPILE_ARMOR].tokenname="STOCKPILE_ARMOR";
	interfacekey[INTERFACEKEY_STOCKPILE_CUSTOM].tokenname="STOCKPILE_CUSTOM";
	interfacekey[INTERFACEKEY_STOCKPILE_CUSTOM_SETTINGS].tokenname="STOCKPILE_CUSTOM_SETTINGS";
	interfacekey[INTERFACEKEY_STOCKPILE_FURNITURE].tokenname="STOCKPILE_FURNITURE";
	interfacekey[INTERFACEKEY_STOCKPILE_GRAVEYARD].tokenname="STOCKPILE_GRAVEYARD";
	interfacekey[INTERFACEKEY_STOCKPILE_REFUSE].tokenname="STOCKPILE_REFUSE";
	interfacekey[INTERFACEKEY_STOCKPILE_WOOD].tokenname="STOCKPILE_WOOD";
	interfacekey[INTERFACEKEY_STOCKPILE_STONE].tokenname="STOCKPILE_STONE";
	interfacekey[INTERFACEKEY_STOCKPILE_GEM].tokenname="STOCKPILE_GEM";
	interfacekey[INTERFACEKEY_STOCKPILE_BARBLOCK].tokenname="STOCKPILE_BARBLOCK";
	interfacekey[INTERFACEKEY_STOCKPILE_CLOTH].tokenname="STOCKPILE_CLOTH";
	interfacekey[INTERFACEKEY_STOCKPILE_LEATHER].tokenname="STOCKPILE_LEATHER";
	interfacekey[INTERFACEKEY_STOCKPILE_AMMO].tokenname="STOCKPILE_AMMO";
	interfacekey[INTERFACEKEY_STOCKPILE_COINS].tokenname="STOCKPILE_COINS";
	interfacekey[INTERFACEKEY_STOCKPILE_FINISHED].tokenname="STOCKPILE_FINISHED";
	interfacekey[INTERFACEKEY_STOCKPILE_NONE].tokenname="STOCKPILE_NONE";
	interfacekey[INTERFACEKEY_DESIGNATE_BITEM].tokenname="DESIGNATE_BITEM";
	interfacekey[INTERFACEKEY_DESIGNATE_CLAIM].tokenname="DESIGNATE_CLAIM";
	interfacekey[INTERFACEKEY_DESIGNATE_UNCLAIM].tokenname="DESIGNATE_UNCLAIM";
	interfacekey[INTERFACEKEY_DESIGNATE_MELT].tokenname="DESIGNATE_MELT";
	interfacekey[INTERFACEKEY_DESIGNATE_NO_MELT].tokenname="DESIGNATE_NO_MELT";
	interfacekey[INTERFACEKEY_DESIGNATE_DUMP].tokenname="DESIGNATE_DUMP";
	interfacekey[INTERFACEKEY_DESIGNATE_NO_DUMP].tokenname="DESIGNATE_NO_DUMP";
	interfacekey[INTERFACEKEY_DESIGNATE_HIDE].tokenname="DESIGNATE_HIDE";
	interfacekey[INTERFACEKEY_DESIGNATE_NO_HIDE].tokenname="DESIGNATE_NO_HIDE";
	interfacekey[INTERFACEKEY_DESIGNATE_TRAFFIC].tokenname="DESIGNATE_TRAFFIC";
	interfacekey[INTERFACEKEY_DESIGNATE_TRAFFIC_HIGH].tokenname="DESIGNATE_TRAFFIC_HIGH";
	interfacekey[INTERFACEKEY_DESIGNATE_TRAFFIC_NORMAL].tokenname="DESIGNATE_TRAFFIC_NORMAL";
	interfacekey[INTERFACEKEY_DESIGNATE_TRAFFIC_LOW].tokenname="DESIGNATE_TRAFFIC_LOW";
	interfacekey[INTERFACEKEY_DESIGNATE_TRAFFIC_RESTRICTED].tokenname="DESIGNATE_TRAFFIC_RESTRICTED";
	interfacekey[INTERFACEKEY_DESIGNATE_TRAFFIC_INCREASE_WEIGHT].tokenname="DESIGNATE_TRAFFIC_INCREASE_WEIGHT";
	interfacekey[INTERFACEKEY_DESIGNATE_TRAFFIC_DECREASE_WEIGHT].tokenname="DESIGNATE_TRAFFIC_DECREASE_WEIGHT";
	interfacekey[INTERFACEKEY_DESIGNATE_TRAFFIC_INCREASE_WEIGHT_MORE].tokenname="DESIGNATE_TRAFFIC_INCREASE_WEIGHT_MORE";
	interfacekey[INTERFACEKEY_DESIGNATE_TRAFFIC_DECREASE_WEIGHT_MORE].tokenname="DESIGNATE_TRAFFIC_DECREASE_WEIGHT_MORE";
	interfacekey[INTERFACEKEY_DESIGNATE_DIG].tokenname="DESIGNATE_DIG";
	interfacekey[INTERFACEKEY_DESIGNATE_DIG_REMOVE_STAIRS_RAMPS].tokenname="DESIGNATE_DIG_REMOVE_STAIRS_RAMPS";
	interfacekey[INTERFACEKEY_DESIGNATE_STAIR_UP].tokenname="DESIGNATE_STAIR_UP";
	interfacekey[INTERFACEKEY_DESIGNATE_STAIR_DOWN].tokenname="DESIGNATE_STAIR_DOWN";
	interfacekey[INTERFACEKEY_DESIGNATE_STAIR_UPDOWN].tokenname="DESIGNATE_STAIR_UPDOWN";
	interfacekey[INTERFACEKEY_DESIGNATE_RAMP].tokenname="DESIGNATE_RAMP";
	interfacekey[INTERFACEKEY_DESIGNATE_CHANNEL].tokenname="DESIGNATE_CHANNEL";
	interfacekey[INTERFACEKEY_DESIGNATE_CHOP].tokenname="DESIGNATE_CHOP";
	interfacekey[INTERFACEKEY_DESIGNATE_PLANTS].tokenname="DESIGNATE_PLANTS";
	interfacekey[INTERFACEKEY_DESIGNATE_SMOOTH].tokenname="DESIGNATE_SMOOTH";
	interfacekey[INTERFACEKEY_DESIGNATE_ENGRAVE].tokenname="DESIGNATE_ENGRAVE";
	interfacekey[INTERFACEKEY_DESIGNATE_FORTIFY].tokenname="DESIGNATE_FORTIFY";
	interfacekey[INTERFACEKEY_DESIGNATE_TOGGLE_ENGRAVING].tokenname="DESIGNATE_TOGGLE_ENGRAVING";
	interfacekey[INTERFACEKEY_DESIGNATE_UNDO].tokenname="DESIGNATE_UNDO";
	interfacekey[INTERFACEKEY_DESIGNATE_REMOVE_CONSTRUCTION].tokenname="DESIGNATE_REMOVE_CONSTRUCTION";
	interfacekey[INTERFACEKEY_BUILDING_DIM_Y_UP].tokenname="BUILDING_DIM_Y_UP";
	interfacekey[INTERFACEKEY_BUILDING_DIM_Y_DOWN].tokenname="BUILDING_DIM_Y_DOWN";
	interfacekey[INTERFACEKEY_BUILDING_DIM_X_UP].tokenname="BUILDING_DIM_X_UP";
	interfacekey[INTERFACEKEY_BUILDING_DIM_X_DOWN].tokenname="BUILDING_DIM_X_DOWN";
	interfacekey[INTERFACEKEY_BUILDING_ORIENT_UP].tokenname="BUILDING_ORIENT_UP";
	interfacekey[INTERFACEKEY_BUILDING_ORIENT_LEFT].tokenname="BUILDING_ORIENT_LEFT";
	interfacekey[INTERFACEKEY_BUILDING_ORIENT_RIGHT].tokenname="BUILDING_ORIENT_RIGHT";
	interfacekey[INTERFACEKEY_BUILDING_ORIENT_DOWN].tokenname="BUILDING_ORIENT_DOWN";
	interfacekey[INTERFACEKEY_BUILDING_ORIENT_NONE].tokenname="BUILDING_ORIENT_NONE";
	interfacekey[INTERFACEKEY_BUILDING_VIEW_ITEM].tokenname="BUILDING_VIEW_ITEM";
	interfacekey[INTERFACEKEY_BUILDING_ADVANCE_STAGE].tokenname="BUILDING_ADVANCE_STAGE";
	interfacekey[INTERFACEKEY_BUILDING_EXPAND_CONTRACT].tokenname="BUILDING_EXPAND_CONTRACT";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_ENABLE_WATER].tokenname="BUILDING_TRIGGER_ENABLE_WATER";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_MIN_WATER_UP].tokenname="BUILDING_TRIGGER_MIN_WATER_UP";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_MIN_WATER_DOWN].tokenname="BUILDING_TRIGGER_MIN_WATER_DOWN";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_MAX_WATER_UP].tokenname="BUILDING_TRIGGER_MAX_WATER_UP";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_MAX_WATER_DOWN].tokenname="BUILDING_TRIGGER_MAX_WATER_DOWN";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_ENABLE_MAGMA].tokenname="BUILDING_TRIGGER_ENABLE_MAGMA";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_MIN_MAGMA_UP].tokenname="BUILDING_TRIGGER_MIN_MAGMA_UP";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_MIN_MAGMA_DOWN].tokenname="BUILDING_TRIGGER_MIN_MAGMA_DOWN";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_MAX_MAGMA_UP].tokenname="BUILDING_TRIGGER_MAX_MAGMA_UP";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_MAX_MAGMA_DOWN].tokenname="BUILDING_TRIGGER_MAX_MAGMA_DOWN";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_ENABLE_CREATURE].tokenname="BUILDING_TRIGGER_ENABLE_CREATURE";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_ENABLE_LOCALS].tokenname="BUILDING_TRIGGER_ENABLE_LOCALS";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_RESETS].tokenname="BUILDING_TRIGGER_RESETS";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_MIN_SIZE_UP].tokenname="BUILDING_TRIGGER_MIN_SIZE_UP";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_MIN_SIZE_DOWN].tokenname="BUILDING_TRIGGER_MIN_SIZE_DOWN";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_MAX_SIZE_UP].tokenname="BUILDING_TRIGGER_MAX_SIZE_UP";
	interfacekey[INTERFACEKEY_BUILDING_TRIGGER_MAX_SIZE_DOWN].tokenname="BUILDING_TRIGGER_MAX_SIZE_DOWN";
	interfacekey[INTERFACEKEY_UNITVIEW_GEN].tokenname="UNITVIEW_GEN";
	interfacekey[INTERFACEKEY_UNITVIEW_INV].tokenname="UNITVIEW_INV";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF].tokenname="UNITVIEW_PRF";
	interfacekey[INTERFACEKEY_UNITVIEW_WND].tokenname="UNITVIEW_WND";
	interfacekey[INTERFACEKEY_UNITVIEW_NEXT].tokenname="UNITVIEW_NEXT";
	interfacekey[INTERFACEKEY_UNITVIEW_SLAUGHTER].tokenname="UNITVIEW_SLAUGHTER";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_PROF].tokenname="UNITVIEW_PRF_PROF";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_PET].tokenname="UNITVIEW_PRF_PET";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_SOLDIER].tokenname="UNITVIEW_PRF_SOLDIER";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_CASTLE].tokenname="UNITVIEW_PRF_CASTLE";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_ROYAL].tokenname="UNITVIEW_PRF_ROYAL";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_TOGGLESQUAD].tokenname="UNITVIEW_PRF_TOGGLESQUAD";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_PET_HUNT].tokenname="UNITVIEW_PRF_PET_HUNT";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_PET_WAR].tokenname="UNITVIEW_PRF_PET_WAR";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_SOLDIER_UNARMED].tokenname="UNITVIEW_PRF_SOLDIER_UNARMED";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_SOLDIER_AXE].tokenname="UNITVIEW_PRF_SOLDIER_AXE";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_SOLDIER_CROSSBOW].tokenname="UNITVIEW_PRF_SOLDIER_CROSSBOW";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_SOLDIER_HAMMER].tokenname="UNITVIEW_PRF_SOLDIER_HAMMER";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_SOLDIER_MACE].tokenname="UNITVIEW_PRF_SOLDIER_MACE";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_SOLDIER_SPEAR].tokenname="UNITVIEW_PRF_SOLDIER_SPEAR";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_SOLDIER_SWORD].tokenname="UNITVIEW_PRF_SOLDIER_SWORD";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_SOLDIER_WEAPONNUM].tokenname="UNITVIEW_PRF_SOLDIER_WEAPONNUM";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_SOLDIER_ARMOR].tokenname="UNITVIEW_PRF_SOLDIER_ARMOR";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_SOLDIER_SHIELD].tokenname="UNITVIEW_PRF_SOLDIER_SHIELD";
	interfacekey[INTERFACEKEY_UNITVIEW_PRF_VIEW].tokenname="UNITVIEW_PRF_VIEW";

	interfacekey[INTERFACEKEY_STRING_A000].tokenname="STRING_A000";
	interfacekey[INTERFACEKEY_STRING_A001].tokenname="STRING_A001";
	interfacekey[INTERFACEKEY_STRING_A002].tokenname="STRING_A002";
	interfacekey[INTERFACEKEY_STRING_A003].tokenname="STRING_A003";
	interfacekey[INTERFACEKEY_STRING_A004].tokenname="STRING_A004";
	interfacekey[INTERFACEKEY_STRING_A005].tokenname="STRING_A005";
	interfacekey[INTERFACEKEY_STRING_A006].tokenname="STRING_A006";
	interfacekey[INTERFACEKEY_STRING_A007].tokenname="STRING_A007";
	interfacekey[INTERFACEKEY_STRING_A008].tokenname="STRING_A008";
	interfacekey[INTERFACEKEY_STRING_A009].tokenname="STRING_A009";
	interfacekey[INTERFACEKEY_STRING_A010].tokenname="STRING_A010";
	interfacekey[INTERFACEKEY_STRING_A011].tokenname="STRING_A011";
	interfacekey[INTERFACEKEY_STRING_A012].tokenname="STRING_A012";
	interfacekey[INTERFACEKEY_STRING_A013].tokenname="STRING_A013";
	interfacekey[INTERFACEKEY_STRING_A014].tokenname="STRING_A014";
	interfacekey[INTERFACEKEY_STRING_A015].tokenname="STRING_A015";
	interfacekey[INTERFACEKEY_STRING_A016].tokenname="STRING_A016";
	interfacekey[INTERFACEKEY_STRING_A017].tokenname="STRING_A017";
	interfacekey[INTERFACEKEY_STRING_A018].tokenname="STRING_A018";
	interfacekey[INTERFACEKEY_STRING_A019].tokenname="STRING_A019";
	interfacekey[INTERFACEKEY_STRING_A020].tokenname="STRING_A020";
	interfacekey[INTERFACEKEY_STRING_A021].tokenname="STRING_A021";
	interfacekey[INTERFACEKEY_STRING_A022].tokenname="STRING_A022";
	interfacekey[INTERFACEKEY_STRING_A023].tokenname="STRING_A023";
	interfacekey[INTERFACEKEY_STRING_A024].tokenname="STRING_A024";
	interfacekey[INTERFACEKEY_STRING_A025].tokenname="STRING_A025";
	interfacekey[INTERFACEKEY_STRING_A026].tokenname="STRING_A026";
	interfacekey[INTERFACEKEY_STRING_A027].tokenname="STRING_A027";
	interfacekey[INTERFACEKEY_STRING_A028].tokenname="STRING_A028";
	interfacekey[INTERFACEKEY_STRING_A029].tokenname="STRING_A029";
	interfacekey[INTERFACEKEY_STRING_A030].tokenname="STRING_A030";
	interfacekey[INTERFACEKEY_STRING_A031].tokenname="STRING_A031";
	interfacekey[INTERFACEKEY_STRING_A032].tokenname="STRING_A032";
	interfacekey[INTERFACEKEY_STRING_A033].tokenname="STRING_A033";
	interfacekey[INTERFACEKEY_STRING_A034].tokenname="STRING_A034";
	interfacekey[INTERFACEKEY_STRING_A035].tokenname="STRING_A035";
	interfacekey[INTERFACEKEY_STRING_A036].tokenname="STRING_A036";
	interfacekey[INTERFACEKEY_STRING_A037].tokenname="STRING_A037";
	interfacekey[INTERFACEKEY_STRING_A038].tokenname="STRING_A038";
	interfacekey[INTERFACEKEY_STRING_A039].tokenname="STRING_A039";
	interfacekey[INTERFACEKEY_STRING_A040].tokenname="STRING_A040";
	interfacekey[INTERFACEKEY_STRING_A041].tokenname="STRING_A041";
	interfacekey[INTERFACEKEY_STRING_A042].tokenname="STRING_A042";
	interfacekey[INTERFACEKEY_STRING_A043].tokenname="STRING_A043";
	interfacekey[INTERFACEKEY_STRING_A044].tokenname="STRING_A044";
	interfacekey[INTERFACEKEY_STRING_A045].tokenname="STRING_A045";
	interfacekey[INTERFACEKEY_STRING_A046].tokenname="STRING_A046";
	interfacekey[INTERFACEKEY_STRING_A047].tokenname="STRING_A047";
	interfacekey[INTERFACEKEY_STRING_A048].tokenname="STRING_A048";
	interfacekey[INTERFACEKEY_STRING_A049].tokenname="STRING_A049";
	interfacekey[INTERFACEKEY_STRING_A050].tokenname="STRING_A050";
	interfacekey[INTERFACEKEY_STRING_A051].tokenname="STRING_A051";
	interfacekey[INTERFACEKEY_STRING_A052].tokenname="STRING_A052";
	interfacekey[INTERFACEKEY_STRING_A053].tokenname="STRING_A053";
	interfacekey[INTERFACEKEY_STRING_A054].tokenname="STRING_A054";
	interfacekey[INTERFACEKEY_STRING_A055].tokenname="STRING_A055";
	interfacekey[INTERFACEKEY_STRING_A056].tokenname="STRING_A056";
	interfacekey[INTERFACEKEY_STRING_A057].tokenname="STRING_A057";
	interfacekey[INTERFACEKEY_STRING_A058].tokenname="STRING_A058";
	interfacekey[INTERFACEKEY_STRING_A059].tokenname="STRING_A059";
	interfacekey[INTERFACEKEY_STRING_A060].tokenname="STRING_A060";
	interfacekey[INTERFACEKEY_STRING_A061].tokenname="STRING_A061";
	interfacekey[INTERFACEKEY_STRING_A062].tokenname="STRING_A062";
	interfacekey[INTERFACEKEY_STRING_A063].tokenname="STRING_A063";
	interfacekey[INTERFACEKEY_STRING_A064].tokenname="STRING_A064";
	interfacekey[INTERFACEKEY_STRING_A065].tokenname="STRING_A065";
	interfacekey[INTERFACEKEY_STRING_A066].tokenname="STRING_A066";
	interfacekey[INTERFACEKEY_STRING_A067].tokenname="STRING_A067";
	interfacekey[INTERFACEKEY_STRING_A068].tokenname="STRING_A068";
	interfacekey[INTERFACEKEY_STRING_A069].tokenname="STRING_A069";
	interfacekey[INTERFACEKEY_STRING_A070].tokenname="STRING_A070";
	interfacekey[INTERFACEKEY_STRING_A071].tokenname="STRING_A071";
	interfacekey[INTERFACEKEY_STRING_A072].tokenname="STRING_A072";
	interfacekey[INTERFACEKEY_STRING_A073].tokenname="STRING_A073";
	interfacekey[INTERFACEKEY_STRING_A074].tokenname="STRING_A074";
	interfacekey[INTERFACEKEY_STRING_A075].tokenname="STRING_A075";
	interfacekey[INTERFACEKEY_STRING_A076].tokenname="STRING_A076";
	interfacekey[INTERFACEKEY_STRING_A077].tokenname="STRING_A077";
	interfacekey[INTERFACEKEY_STRING_A078].tokenname="STRING_A078";
	interfacekey[INTERFACEKEY_STRING_A079].tokenname="STRING_A079";
	interfacekey[INTERFACEKEY_STRING_A080].tokenname="STRING_A080";
	interfacekey[INTERFACEKEY_STRING_A081].tokenname="STRING_A081";
	interfacekey[INTERFACEKEY_STRING_A082].tokenname="STRING_A082";
	interfacekey[INTERFACEKEY_STRING_A083].tokenname="STRING_A083";
	interfacekey[INTERFACEKEY_STRING_A084].tokenname="STRING_A084";
	interfacekey[INTERFACEKEY_STRING_A085].tokenname="STRING_A085";
	interfacekey[INTERFACEKEY_STRING_A086].tokenname="STRING_A086";
	interfacekey[INTERFACEKEY_STRING_A087].tokenname="STRING_A087";
	interfacekey[INTERFACEKEY_STRING_A088].tokenname="STRING_A088";
	interfacekey[INTERFACEKEY_STRING_A089].tokenname="STRING_A089";
	interfacekey[INTERFACEKEY_STRING_A090].tokenname="STRING_A090";
	interfacekey[INTERFACEKEY_STRING_A091].tokenname="STRING_A091";
	interfacekey[INTERFACEKEY_STRING_A092].tokenname="STRING_A092";
	interfacekey[INTERFACEKEY_STRING_A093].tokenname="STRING_A093";
	interfacekey[INTERFACEKEY_STRING_A094].tokenname="STRING_A094";
	interfacekey[INTERFACEKEY_STRING_A095].tokenname="STRING_A095";
	interfacekey[INTERFACEKEY_STRING_A096].tokenname="STRING_A096";
	interfacekey[INTERFACEKEY_STRING_A097].tokenname="STRING_A097";
	interfacekey[INTERFACEKEY_STRING_A098].tokenname="STRING_A098";
	interfacekey[INTERFACEKEY_STRING_A099].tokenname="STRING_A099";
	interfacekey[INTERFACEKEY_STRING_A100].tokenname="STRING_A100";
	interfacekey[INTERFACEKEY_STRING_A101].tokenname="STRING_A101";
	interfacekey[INTERFACEKEY_STRING_A102].tokenname="STRING_A102";
	interfacekey[INTERFACEKEY_STRING_A103].tokenname="STRING_A103";
	interfacekey[INTERFACEKEY_STRING_A104].tokenname="STRING_A104";
	interfacekey[INTERFACEKEY_STRING_A105].tokenname="STRING_A105";
	interfacekey[INTERFACEKEY_STRING_A106].tokenname="STRING_A106";
	interfacekey[INTERFACEKEY_STRING_A107].tokenname="STRING_A107";
	interfacekey[INTERFACEKEY_STRING_A108].tokenname="STRING_A108";
	interfacekey[INTERFACEKEY_STRING_A109].tokenname="STRING_A109";
	interfacekey[INTERFACEKEY_STRING_A110].tokenname="STRING_A110";
	interfacekey[INTERFACEKEY_STRING_A111].tokenname="STRING_A111";
	interfacekey[INTERFACEKEY_STRING_A112].tokenname="STRING_A112";
	interfacekey[INTERFACEKEY_STRING_A113].tokenname="STRING_A113";
	interfacekey[INTERFACEKEY_STRING_A114].tokenname="STRING_A114";
	interfacekey[INTERFACEKEY_STRING_A115].tokenname="STRING_A115";
	interfacekey[INTERFACEKEY_STRING_A116].tokenname="STRING_A116";
	interfacekey[INTERFACEKEY_STRING_A117].tokenname="STRING_A117";
	interfacekey[INTERFACEKEY_STRING_A118].tokenname="STRING_A118";
	interfacekey[INTERFACEKEY_STRING_A119].tokenname="STRING_A119";
	interfacekey[INTERFACEKEY_STRING_A120].tokenname="STRING_A120";
	interfacekey[INTERFACEKEY_STRING_A121].tokenname="STRING_A121";
	interfacekey[INTERFACEKEY_STRING_A122].tokenname="STRING_A122";
	interfacekey[INTERFACEKEY_STRING_A123].tokenname="STRING_A123";
	interfacekey[INTERFACEKEY_STRING_A124].tokenname="STRING_A124";
	interfacekey[INTERFACEKEY_STRING_A125].tokenname="STRING_A125";
	interfacekey[INTERFACEKEY_STRING_A126].tokenname="STRING_A126";
	interfacekey[INTERFACEKEY_STRING_A127].tokenname="STRING_A127";
	interfacekey[INTERFACEKEY_STRING_A128].tokenname="STRING_A128";
	interfacekey[INTERFACEKEY_STRING_A129].tokenname="STRING_A129";
	interfacekey[INTERFACEKEY_STRING_A130].tokenname="STRING_A130";
	interfacekey[INTERFACEKEY_STRING_A131].tokenname="STRING_A131";
	interfacekey[INTERFACEKEY_STRING_A132].tokenname="STRING_A132";
	interfacekey[INTERFACEKEY_STRING_A133].tokenname="STRING_A133";
	interfacekey[INTERFACEKEY_STRING_A134].tokenname="STRING_A134";
	interfacekey[INTERFACEKEY_STRING_A135].tokenname="STRING_A135";
	interfacekey[INTERFACEKEY_STRING_A136].tokenname="STRING_A136";
	interfacekey[INTERFACEKEY_STRING_A137].tokenname="STRING_A137";
	interfacekey[INTERFACEKEY_STRING_A138].tokenname="STRING_A138";
	interfacekey[INTERFACEKEY_STRING_A139].tokenname="STRING_A139";
	interfacekey[INTERFACEKEY_STRING_A140].tokenname="STRING_A140";
	interfacekey[INTERFACEKEY_STRING_A141].tokenname="STRING_A141";
	interfacekey[INTERFACEKEY_STRING_A142].tokenname="STRING_A142";
	interfacekey[INTERFACEKEY_STRING_A143].tokenname="STRING_A143";
	interfacekey[INTERFACEKEY_STRING_A144].tokenname="STRING_A144";
	interfacekey[INTERFACEKEY_STRING_A145].tokenname="STRING_A145";
	interfacekey[INTERFACEKEY_STRING_A146].tokenname="STRING_A146";
	interfacekey[INTERFACEKEY_STRING_A147].tokenname="STRING_A147";
	interfacekey[INTERFACEKEY_STRING_A148].tokenname="STRING_A148";
	interfacekey[INTERFACEKEY_STRING_A149].tokenname="STRING_A149";
	interfacekey[INTERFACEKEY_STRING_A150].tokenname="STRING_A150";
	interfacekey[INTERFACEKEY_STRING_A151].tokenname="STRING_A151";
	interfacekey[INTERFACEKEY_STRING_A152].tokenname="STRING_A152";
	interfacekey[INTERFACEKEY_STRING_A153].tokenname="STRING_A153";
	interfacekey[INTERFACEKEY_STRING_A154].tokenname="STRING_A154";
	interfacekey[INTERFACEKEY_STRING_A155].tokenname="STRING_A155";
	interfacekey[INTERFACEKEY_STRING_A156].tokenname="STRING_A156";
	interfacekey[INTERFACEKEY_STRING_A157].tokenname="STRING_A157";
	interfacekey[INTERFACEKEY_STRING_A158].tokenname="STRING_A158";
	interfacekey[INTERFACEKEY_STRING_A159].tokenname="STRING_A159";
	interfacekey[INTERFACEKEY_STRING_A160].tokenname="STRING_A160";
	interfacekey[INTERFACEKEY_STRING_A161].tokenname="STRING_A161";
	interfacekey[INTERFACEKEY_STRING_A162].tokenname="STRING_A162";
	interfacekey[INTERFACEKEY_STRING_A163].tokenname="STRING_A163";
	interfacekey[INTERFACEKEY_STRING_A164].tokenname="STRING_A164";
	interfacekey[INTERFACEKEY_STRING_A165].tokenname="STRING_A165";
	interfacekey[INTERFACEKEY_STRING_A166].tokenname="STRING_A166";
	interfacekey[INTERFACEKEY_STRING_A167].tokenname="STRING_A167";
	interfacekey[INTERFACEKEY_STRING_A168].tokenname="STRING_A168";
	interfacekey[INTERFACEKEY_STRING_A169].tokenname="STRING_A169";
	interfacekey[INTERFACEKEY_STRING_A170].tokenname="STRING_A170";
	interfacekey[INTERFACEKEY_STRING_A171].tokenname="STRING_A171";
	interfacekey[INTERFACEKEY_STRING_A172].tokenname="STRING_A172";
	interfacekey[INTERFACEKEY_STRING_A173].tokenname="STRING_A173";
	interfacekey[INTERFACEKEY_STRING_A174].tokenname="STRING_A174";
	interfacekey[INTERFACEKEY_STRING_A175].tokenname="STRING_A175";
	interfacekey[INTERFACEKEY_STRING_A176].tokenname="STRING_A176";
	interfacekey[INTERFACEKEY_STRING_A177].tokenname="STRING_A177";
	interfacekey[INTERFACEKEY_STRING_A178].tokenname="STRING_A178";
	interfacekey[INTERFACEKEY_STRING_A179].tokenname="STRING_A179";
	interfacekey[INTERFACEKEY_STRING_A180].tokenname="STRING_A180";
	interfacekey[INTERFACEKEY_STRING_A181].tokenname="STRING_A181";
	interfacekey[INTERFACEKEY_STRING_A182].tokenname="STRING_A182";
	interfacekey[INTERFACEKEY_STRING_A183].tokenname="STRING_A183";
	interfacekey[INTERFACEKEY_STRING_A184].tokenname="STRING_A184";
	interfacekey[INTERFACEKEY_STRING_A185].tokenname="STRING_A185";
	interfacekey[INTERFACEKEY_STRING_A186].tokenname="STRING_A186";
	interfacekey[INTERFACEKEY_STRING_A187].tokenname="STRING_A187";
	interfacekey[INTERFACEKEY_STRING_A188].tokenname="STRING_A188";
	interfacekey[INTERFACEKEY_STRING_A189].tokenname="STRING_A189";
	interfacekey[INTERFACEKEY_STRING_A190].tokenname="STRING_A190";
	interfacekey[INTERFACEKEY_STRING_A191].tokenname="STRING_A191";
	interfacekey[INTERFACEKEY_STRING_A192].tokenname="STRING_A192";
	interfacekey[INTERFACEKEY_STRING_A193].tokenname="STRING_A193";
	interfacekey[INTERFACEKEY_STRING_A194].tokenname="STRING_A194";
	interfacekey[INTERFACEKEY_STRING_A195].tokenname="STRING_A195";
	interfacekey[INTERFACEKEY_STRING_A196].tokenname="STRING_A196";
	interfacekey[INTERFACEKEY_STRING_A197].tokenname="STRING_A197";
	interfacekey[INTERFACEKEY_STRING_A198].tokenname="STRING_A198";
	interfacekey[INTERFACEKEY_STRING_A199].tokenname="STRING_A199";
	interfacekey[INTERFACEKEY_STRING_A200].tokenname="STRING_A200";
	interfacekey[INTERFACEKEY_STRING_A201].tokenname="STRING_A201";
	interfacekey[INTERFACEKEY_STRING_A202].tokenname="STRING_A202";
	interfacekey[INTERFACEKEY_STRING_A203].tokenname="STRING_A203";
	interfacekey[INTERFACEKEY_STRING_A204].tokenname="STRING_A204";
	interfacekey[INTERFACEKEY_STRING_A205].tokenname="STRING_A205";
	interfacekey[INTERFACEKEY_STRING_A206].tokenname="STRING_A206";
	interfacekey[INTERFACEKEY_STRING_A207].tokenname="STRING_A207";
	interfacekey[INTERFACEKEY_STRING_A208].tokenname="STRING_A208";
	interfacekey[INTERFACEKEY_STRING_A209].tokenname="STRING_A209";
	interfacekey[INTERFACEKEY_STRING_A210].tokenname="STRING_A210";
	interfacekey[INTERFACEKEY_STRING_A211].tokenname="STRING_A211";
	interfacekey[INTERFACEKEY_STRING_A212].tokenname="STRING_A212";
	interfacekey[INTERFACEKEY_STRING_A213].tokenname="STRING_A213";
	interfacekey[INTERFACEKEY_STRING_A214].tokenname="STRING_A214";
	interfacekey[INTERFACEKEY_STRING_A215].tokenname="STRING_A215";
	interfacekey[INTERFACEKEY_STRING_A216].tokenname="STRING_A216";
	interfacekey[INTERFACEKEY_STRING_A217].tokenname="STRING_A217";
	interfacekey[INTERFACEKEY_STRING_A218].tokenname="STRING_A218";
	interfacekey[INTERFACEKEY_STRING_A219].tokenname="STRING_A219";
	interfacekey[INTERFACEKEY_STRING_A220].tokenname="STRING_A220";
	interfacekey[INTERFACEKEY_STRING_A221].tokenname="STRING_A221";
	interfacekey[INTERFACEKEY_STRING_A222].tokenname="STRING_A222";
	interfacekey[INTERFACEKEY_STRING_A223].tokenname="STRING_A223";
	interfacekey[INTERFACEKEY_STRING_A224].tokenname="STRING_A224";
	interfacekey[INTERFACEKEY_STRING_A225].tokenname="STRING_A225";
	interfacekey[INTERFACEKEY_STRING_A226].tokenname="STRING_A226";
	interfacekey[INTERFACEKEY_STRING_A227].tokenname="STRING_A227";
	interfacekey[INTERFACEKEY_STRING_A228].tokenname="STRING_A228";
	interfacekey[INTERFACEKEY_STRING_A229].tokenname="STRING_A229";
	interfacekey[INTERFACEKEY_STRING_A230].tokenname="STRING_A230";
	interfacekey[INTERFACEKEY_STRING_A231].tokenname="STRING_A231";
	interfacekey[INTERFACEKEY_STRING_A232].tokenname="STRING_A232";
	interfacekey[INTERFACEKEY_STRING_A233].tokenname="STRING_A233";
	interfacekey[INTERFACEKEY_STRING_A234].tokenname="STRING_A234";
	interfacekey[INTERFACEKEY_STRING_A235].tokenname="STRING_A235";
	interfacekey[INTERFACEKEY_STRING_A236].tokenname="STRING_A236";
	interfacekey[INTERFACEKEY_STRING_A237].tokenname="STRING_A237";
	interfacekey[INTERFACEKEY_STRING_A238].tokenname="STRING_A238";
	interfacekey[INTERFACEKEY_STRING_A239].tokenname="STRING_A239";
	interfacekey[INTERFACEKEY_STRING_A240].tokenname="STRING_A240";
	interfacekey[INTERFACEKEY_STRING_A241].tokenname="STRING_A241";
	interfacekey[INTERFACEKEY_STRING_A242].tokenname="STRING_A242";
	interfacekey[INTERFACEKEY_STRING_A243].tokenname="STRING_A243";
	interfacekey[INTERFACEKEY_STRING_A244].tokenname="STRING_A244";
	interfacekey[INTERFACEKEY_STRING_A245].tokenname="STRING_A245";
	interfacekey[INTERFACEKEY_STRING_A246].tokenname="STRING_A246";
	interfacekey[INTERFACEKEY_STRING_A247].tokenname="STRING_A247";
	interfacekey[INTERFACEKEY_STRING_A248].tokenname="STRING_A248";
	interfacekey[INTERFACEKEY_STRING_A249].tokenname="STRING_A249";
	interfacekey[INTERFACEKEY_STRING_A250].tokenname="STRING_A250";
	interfacekey[INTERFACEKEY_STRING_A251].tokenname="STRING_A251";
	interfacekey[INTERFACEKEY_STRING_A252].tokenname="STRING_A252";
	interfacekey[INTERFACEKEY_STRING_A253].tokenname="STRING_A253";
	interfacekey[INTERFACEKEY_STRING_A254].tokenname="STRING_A254";
	interfacekey[INTERFACEKEY_STRING_A255].tokenname="STRING_A255";
}

interfacest::~interfacest()
{
	//GO AHEAD
	while(view.child!=NULL)
		{
		removescreen(view.child);
		}
}

char interfacest::keypress(short key)
{
	if(key<0||key>=INTERFACEKEYNUM)return 0;

	if(interfacekey[key].ctrl!=ctrl)return 0;
	if(interfacekey[key].alt!=alt)return 0;

	if(c>='A'&&c<='Z'&&caps)
		{
		if(interfacekey[key].binding==c&&interfacekey[key].shift==(!shift))return 1;
		}
	else if(interfacekey[key].binding==c&&interfacekey[key].shift==shift)return 1;

	return 0;
}

void getstring_interfacechar(string &str,interfacekeyst &key)
{
	if(!gview.valid_held_key(key.binding))
		{
		str="Not Supported";
		return;
		}

	short ind=0;
	if(key.shift)ind++;
	if(key.ctrl)ind+=2;
	if(key.alt)ind+=4;
	if(!gview.display_string[key.binding][ind].empty())
		{
		str=gview.display_string[key.binding][ind];
		return;
		}

	str.erase();

	if(key.ctrl)str+="Ctrl+";
	if(key.alt)str+="Alt+";
	if(key.shift)str+="Shft+";
	switch(key.binding)
		{
		case VK_BACK:str+="Backspace";break;
		case VK_TAB:str+="Tab";break;
		case VK_CLEAR:str+="Clear";break;
		case VK_RETURN:str+="Enter";break;
		case VK_SHIFT:str+="Shift";break;
		case VK_CONTROL:str+="Ctrl";break;
		case VK_MENU:str+="Alt";break;
		case VK_PAUSE:str+="Pause";break;
		case VK_CAPITAL:str+="Caps";break;
		case VK_ESCAPE:str+="Escape";break;
		case VK_SPACE:str+="Space";break;
		case VK_PRIOR:str+="Page Up";break;
		case VK_NEXT:str+="Page Down";break;
		case VK_END:str+="End";break;
		case VK_HOME:str+="Home";break;
		case VK_LEFT:str+="Left";break;
		case VK_UP:str+="Up";break;
		case VK_RIGHT:str+="Right";break;
		case VK_DOWN:str+="Down";break;
		case VK_SELECT:str+="Select";break;
		case VK_PRINT:str+="Print";break;
		case VK_EXECUTE:str+="Execute";break;
		case VK_SNAPSHOT:str+="Print Screen";break;
		case VK_INSERT:str+="Insert";break;
		case VK_DELETE:str+="Delete";break;
		case VK_HELP:str+="Help";break;
		case 0x30://0
		case 0x31:
		case 0x32:
		case 0x33:
		case 0x34:
		case 0x35:
		case 0x36:
		case 0x37:
		case 0x38:
		case 0x39://9
			str+=key.binding;
			break;
		case 0x41://A
		case 0x42:
		case 0x43:
		case 0x44:
		case 0x45:
		case 0x46://F
		case 0x47:
		case 0x48:
		case 0x49:
		case 0x4A:
		case 0x4B://K
		case 0x4C:
		case 0x4D:
		case 0x4E:
		case 0x4F:
		case 0x50://P
		case 0x51:
		case 0x52:
		case 0x53:
		case 0x54:
		case 0x55://U
		case 0x56:
		case 0x57:
		case 0x58:
		case 0x59:
		case 0x5A://Z
			str+=(key.binding-'A')+'a';
			break;
		case VK_NUMPAD0:str+="Numpad 0";break;
		case VK_NUMPAD1:str+="Numpad 1";break;
		case VK_NUMPAD2:str+="Numpad 2";break;
		case VK_NUMPAD3:str+="Numpad 3";break;
		case VK_NUMPAD4:str+="Numpad 4";break;
		case VK_NUMPAD5:str+="Numpad 5";break;
		case VK_NUMPAD6:str+="Numpad 6";break;
		case VK_NUMPAD7:str+="Numpad 7";break;
		case VK_NUMPAD8:str+="Numpad 8";break;
		case VK_NUMPAD9:str+="Numpad 9";break;
		case VK_SEPARATOR:str+="Separator";break;
		case VK_SUBTRACT:str+="Subtract";break;
		case VK_ADD:str+="Add";break;
		case VK_MULTIPLY:str+="Multiply";break;
		case VK_DECIMAL:str+="Decimal";break;
		case VK_DIVIDE:str+="Divide";break;
		case VK_F1:str+="F1";break;
		case VK_F2:str+="F2";break;
		case VK_F3:str+="F3";break;
		case VK_F4:str+="F4";break;
		case VK_F5:str+="F5";break;
		case VK_F6:str+="F6";break;
		case VK_F7:str+="F7";break;
		case VK_F8:str+="F8";break;
		case VK_F9:str+="F9";break;
		case VK_F10:str+="F10";break;
		case VK_F11:str+="F11";break;
		case VK_F12:str+="F12";break;
		case VK_F13:str+="F13";break;
		case VK_F14:str+="F14";break;
		case VK_F15:str+="F15";break;
		case VK_F16:str+="F16";break;
		case VK_F17:str+="F17";break;
		case VK_F18:str+="F18";break;
		case VK_F19:str+="F19";break;
		case VK_F20:str+="F20";break;
		case VK_F21:str+="F21";break;
		case VK_F22:str+="F22";break;
		case VK_F23:str+="F23";break;
		case VK_F24:str+="F24";break;
		case VK_NUMLOCK:str+="Numlock";break;
		case VK_SCROLL:str+="Scroll";break;
		case VK_LSHIFT:str+="L Shift";break;
		case VK_RSHIFT:str+="R Shift";break;
		case VK_LCONTROL:str+="L Control";break;
		case VK_RCONTROL:str+="R Control";break;
		case VK_LMENU:str+="L Menu";break;
		case VK_RMENU:str+="R Menu";break;
		case VK_BUTTON3:str+="Button 3";break;
		case VK_WHEELUP:str+="Wheel up";break;
		case VK_WHEELDOWN:str+="Wheel down";break;
		case VK_WHEELLEFT:str+="Wheel left";break;
		case VK_WHEELRIGHT:str+="Wheel right";break;
		case 0xBA:str+="Key BA";break;
		case 0xBB:str+="Plus";break;
		case 0xBC:str+="Comma";break;
		case 0xBD:str+="Minus";break;
		case 0xBE:str+="Period";break;
		case 0xBF:str+="Key BF";break;
		case 0xC0:str+="Key C0";break;
		case 0xDB:str+="Key DB";break;
		case 0xDC:str+="Key DC";break;
		case 0xDD:str+="Key DD";break;
		case 0xDE:str+="Key DE";break;
		case 0xDF:str+="Key DF";break;
		case 0xE1:str+="Key E1";break;
		case 0xE2:str+="Key E2";break;
		case 0xE3:str+="Key E3";break;
		case 0xE4:str+="Key E4";break;
		case 0xE9:str+="Key E9";break;
		case 0xEA:str+="Key EA";break;
		case 0xEB:str+="Key EB";break;
		case 0xEC:str+="Key EC";break;
		case 0xED:str+="Key ED";break;
		case 0xEE:str+="Key EE";break;
		case 0xEF:str+="Key EF";break;
		case 0xF1:str+="Key F1";break;
		case 0xF2:str+="Key F2";break;
		case 0xF3:str+="Key F3";break;
		case 0xF4:str+="Key F4";break;
		case 0xF5:str+="Key F5";break;
		case 0x15:str+="Key Hangul/Kana";break;
		case 0x17:str+="Key Junja";break;
		case 0x18:str+="Key Final";break;
		case 0x19:str+="Key Hanja/Kanji";break;
		case 0x1C:str+="Key Convert";break;
		case 0x1D:str+="Key Nonconvert";break;
		case 0x1E:str+="Key Accept";break;
		case 0x1F:str+="Key Mode Change";break;
		case 0x92:str+="Key 92";break;
		case 0x93:str+="Key 93";break;
		case 0x94:str+="Key 94";break;
		case 0x95:str+="Key 95";break;
		case 0x96:str+="Key 96";break;
		default:str="Not Supported";break;
		}
}

void getstring_interfacechar_save(string &str,interfacekeyst &key)
{
	if(!gview.valid_held_key(key.binding))
		{
		str="NOT_SUPPORTED";
		return;
		}

	str.erase();

	if(key.ctrl)str+="CTRL+";
	if(key.alt)str+="ALT+";
	if(key.shift)str+="SHIFT+";
	switch(key.binding)
		{
		case VK_BACK:str+="BACKSPACE";break;
		case VK_TAB:str+="TAB";break;
		case VK_CLEAR:str+="CLEAR";break;
		case VK_RETURN:str+="ENTER";break;
		case VK_SHIFT:str+="SHIFT";break;
		case VK_CONTROL:str+="CTRL";break;
		case VK_MENU:str+="ALT";break;
		case VK_PAUSE:str+="PAUSE";break;
		case VK_CAPITAL:str+="CAPS";break;
		case VK_ESCAPE:str+="ESCAPE";break;
		case VK_SPACE:str+="SPACE";break;
		case VK_PRIOR:str+="PAGEUP";break;
		case VK_NEXT:str+="PAGEDOWN";break;
		case VK_END:str+="END";break;
		case VK_HOME:str+="HOME";break;
		case VK_LEFT:str+="LEFT";break;
		case VK_UP:str+="UP";break;
		case VK_RIGHT:str+="RIGHT";break;
		case VK_DOWN:str+="DOWN";break;
		case VK_SELECT:str+="SELECT";break;
		case VK_PRINT:str+="PRINT";break;
		case VK_EXECUTE:str+="EXECUTE";break;
		case VK_SNAPSHOT:str+="PRINTSCREEN";break;
		case VK_INSERT:str+="INSERT";break;
		case VK_DELETE:str+="DELETE";break;
		case VK_HELP:str+="HELP";break;
		case 0x30://0
		case 0x31:
		case 0x32:
		case 0x33:
		case 0x34:
		case 0x35:
		case 0x36:
		case 0x37:
		case 0x38:
		case 0x39://9
		case 0x41://A
		case 0x42:
		case 0x43:
		case 0x44:
		case 0x45:
		case 0x46://F
		case 0x47:
		case 0x48:
		case 0x49:
		case 0x4A:
		case 0x4B://K
		case 0x4C:
		case 0x4D:
		case 0x4E:
		case 0x4F:
		case 0x50://P
		case 0x51:
		case 0x52:
		case 0x53:
		case 0x54:
		case 0x55://U
		case 0x56:
		case 0x57:
		case 0x58:
		case 0x59:
		case 0x5A://Z
			str+=key.binding;
			break;
		case VK_NUMPAD0:str+="NUMPAD0";break;
		case VK_NUMPAD1:str+="NUMPAD1";break;
		case VK_NUMPAD2:str+="NUMPAD2";break;
		case VK_NUMPAD3:str+="NUMPAD3";break;
		case VK_NUMPAD4:str+="NUMPAD4";break;
		case VK_NUMPAD5:str+="NUMPAD5";break;
		case VK_NUMPAD6:str+="NUMPAD6";break;
		case VK_NUMPAD7:str+="NUMPAD7";break;
		case VK_NUMPAD8:str+="NUMPAD8";break;
		case VK_NUMPAD9:str+="NUMPAD9";break;
		case VK_SEPARATOR:str+="SEPARATOR";break;
		case VK_SUBTRACT:str+="SUBTRACT";break;
		case VK_ADD:str+="ADD";break;
		case VK_MULTIPLY:str+="MULTIPLY";break;
		case VK_DECIMAL:str+="DECIMAL";break;
		case VK_DIVIDE:str+="DIVIDE";break;
		case VK_F1:str+="F1";break;
		case VK_F2:str+="F2";break;
		case VK_F3:str+="F3";break;
		case VK_F4:str+="F4";break;
		case VK_F5:str+="F5";break;
		case VK_F6:str+="F6";break;
		case VK_F7:str+="F7";break;
		case VK_F8:str+="F8";break;
		case VK_F9:str+="F9";break;
		case VK_F10:str+="F10";break;
		case VK_F11:str+="F11";break;
		case VK_F12:str+="F12";break;
		case VK_F13:str+="F13";break;
		case VK_F14:str+="F14";break;
		case VK_F15:str+="F15";break;
		case VK_F16:str+="F16";break;
		case VK_F17:str+="F17";break;
		case VK_F18:str+="F18";break;
		case VK_F19:str+="F19";break;
		case VK_F20:str+="F20";break;
		case VK_F21:str+="F21";break;
		case VK_F22:str+="F22";break;
		case VK_F23:str+="F23";break;
		case VK_F24:str+="F24";break;
		case VK_NUMLOCK:str+="NUMLOCK";break;
		case VK_SCROLL:str+="SCROLL";break;
		case VK_LSHIFT:str+="LSHIFT";break;
		case VK_RSHIFT:str+="RSHIFT";break;
		case VK_LCONTROL:str+="LCTRL";break;
		case VK_RCONTROL:str+="RCTRL";break;
		case VK_LMENU:str+="LMENU";break;
		case VK_RMENU:str+="RMENU";break;
		case VK_BUTTON3:str+="BTN3";break;
		case VK_WHEELUP:str+="WHLUP";break;
		case VK_WHEELDOWN:str+="WHLDN";break;
		case VK_WHEELLEFT:str+="WHLLE";break;
		case VK_WHEELRIGHT:str+="WHLRI";break;
		case 0xBA:str+="KEYBA";break;
		case 0xBB:str+="PLUS";break;
		case 0xBC:str+="COMMA";break;
		case 0xBD:str+="MINUS";break;
		case 0xBE:str+="PERIOD";break;
		case 0xBF:str+="KEYBF";break;
		case 0xC0:str+="KEYC0";break;
		case 0xDB:str+="KEYDB";break;
		case 0xDC:str+="KEYDC";break;
		case 0xDD:str+="KEYDD";break;
		case 0xDE:str+="KEYDE";break;
		case 0xDF:str+="KEYDF";break;
		case 0xE1:str+="KEYE1";break;
		case 0xE2:str+="KEYE2";break;
		case 0xE3:str+="KEYE3";break;
		case 0xE4:str+="KEYE4";break;
		case 0xE9:str+="KEYE9";break;
		case 0xEA:str+="KEYEA";break;
		case 0xEB:str+="KEYEB";break;
		case 0xEC:str+="KEYEC";break;
		case 0xED:str+="KEYED";break;
		case 0xEE:str+="KEYEE";break;
		case 0xEF:str+="KEYEF";break;
		case 0xF1:str+="KEYF1";break;
		case 0xF2:str+="KEYF2";break;
		case 0xF3:str+="KEYF3";break;
		case 0xF4:str+="KEYF4";break;
		case 0xF5:str+="KEYF5";break;
		case 0x15:str+="KEYHANGULKANA";break;
		case 0x17:str+="KEYJUNJA";break;
		case 0x18:str+="KEYFINAL";break;
		case 0x19:str+="KEYHANJAKANJI";break;
		case 0x1C:str+="KEYCONVERT";break;
		case 0x1D:str+="KEYNONCONVERT";break;
		case 0x1E:str+="KEYACCEPT";break;
		case 0x1F:str+="KEYMODECHANGE";break;
		case 0x92:str+="KEY92";break;
		case 0x93:str+="KEY93";break;
		case 0x94:str+="KEY94";break;
		case 0x95:str+="KEY95";break;
		case 0x96:str+="KEY96";break;
		default:str="NONE";break;
		}
}

void convertstring_interfacechar_save(string &str,interfacekeyst &key)
{
	if(str.length()==0)return;

	key.binding=0;
	key.shift=0;
	key.alt=0;
	key.ctrl=0;

	string::size_type wpos;
	string sr;

	sr="CTRL+";
	while ((wpos = str.find(sr)) != string::npos)
		{
		string str2="";
		str.replace(wpos,sr.size(),str2);
		key.ctrl=1;
		}
	sr="ALT+";
	while ((wpos = str.find(sr)) != string::npos)
		{
		string str2="";
		str.replace(wpos,sr.size(),str2);
		key.alt=1;
		}
	sr="SHIFT+";
	while ((wpos = str.find(sr)) != string::npos)
		{
		string str2="";
		str.replace(wpos,sr.size(),str2);
		key.shift=1;
		}

	if(!str.compare("BACKSPACE")){key.binding=VK_BACK;return;}
	if(!str.compare("TAB")){key.binding=VK_TAB;return;}
	if(!str.compare("CLEAR")){key.binding=VK_CLEAR;return;}
	if(!str.compare("ENTER")){key.binding=VK_RETURN;return;}
	if(!str.compare("SHIFT")){key.binding=VK_SHIFT;return;}
	if(!str.compare("CTRL")){key.binding=VK_CONTROL;return;}
	if(!str.compare("ALT")){key.binding=VK_MENU;return;}
	if(!str.compare("PAUSE")){key.binding=VK_PAUSE;return;}
	if(!str.compare("CAPS")){key.binding=VK_CAPITAL;return;}
	if(!str.compare("ESCAPE")){key.binding=VK_ESCAPE;return;}
	if(!str.compare("SPACE")){key.binding=VK_SPACE;return;}
	if(!str.compare("PAGEUP")){key.binding=VK_PRIOR;return;}
	if(!str.compare("PAGEDOWN")){key.binding=VK_NEXT;return;}
	if(!str.compare("END")){key.binding=VK_END;return;}
	if(!str.compare("HOME")){key.binding=VK_HOME;return;}
	if(!str.compare("LEFT")){key.binding=VK_LEFT;return;}
	if(!str.compare("UP")){key.binding=VK_UP;return;}
	if(!str.compare("RIGHT")){key.binding=VK_RIGHT;return;}
	if(!str.compare("DOWN")){key.binding=VK_DOWN;return;}
	if(!str.compare("SELECT")){key.binding=VK_SELECT;return;}
	if(!str.compare("PRINT")){key.binding=VK_PRINT;return;}
	if(!str.compare("EXECUTE")){key.binding=VK_EXECUTE;return;}
	if(!str.compare("PRINTSCREEN")){key.binding=VK_SNAPSHOT;return;}
	if(!str.compare("INSERT")){key.binding=VK_INSERT;return;}
	if(!str.compare("DELETE")){key.binding=VK_DELETE;return;}
	if(!str.compare("HELP")){key.binding=VK_HELP;return;}
	if(!str.compare("NUMPAD0")){key.binding=VK_NUMPAD0;return;}
	if(!str.compare("NUMPAD1")){key.binding=VK_NUMPAD1;return;}
	if(!str.compare("NUMPAD2")){key.binding=VK_NUMPAD2;return;}
	if(!str.compare("NUMPAD3")){key.binding=VK_NUMPAD3;return;}
	if(!str.compare("NUMPAD4")){key.binding=VK_NUMPAD4;return;}
	if(!str.compare("NUMPAD5")){key.binding=VK_NUMPAD5;return;}
	if(!str.compare("NUMPAD6")){key.binding=VK_NUMPAD6;return;}
	if(!str.compare("NUMPAD7")){key.binding=VK_NUMPAD7;return;}
	if(!str.compare("NUMPAD8")){key.binding=VK_NUMPAD8;return;}
	if(!str.compare("NUMPAD9")){key.binding=VK_NUMPAD9;return;}
	if(!str.compare("SEPARATOR")){key.binding=VK_SEPARATOR;return;}
	if(!str.compare("SUBTRACT")){key.binding=VK_SUBTRACT;return;}
	if(!str.compare("ADD")){key.binding=VK_ADD;return;}
	if(!str.compare("MULTIPLY")){key.binding=VK_MULTIPLY;return;}
	if(!str.compare("DECIMAL")){key.binding=VK_DECIMAL;return;}
	if(!str.compare("DIVIDE")){key.binding=VK_DIVIDE;return;}
	if(!str.compare("F1")){key.binding=VK_F1;return;}
	if(!str.compare("F2")){key.binding=VK_F2;return;}
	if(!str.compare("F3")){key.binding=VK_F3;return;}
	if(!str.compare("F4")){key.binding=VK_F4;return;}
	if(!str.compare("F5")){key.binding=VK_F5;return;}
	if(!str.compare("F6")){key.binding=VK_F6;return;}
	if(!str.compare("F7")){key.binding=VK_F7;return;}
	if(!str.compare("F8")){key.binding=VK_F8;return;}
	if(!str.compare("F9")){key.binding=VK_F9;return;}
	if(!str.compare("F10")){key.binding=VK_F10;return;}
	if(!str.compare("F11")){key.binding=VK_F11;return;}
	if(!str.compare("F12")){key.binding=VK_F12;return;}
	if(!str.compare("F13")){key.binding=VK_F13;return;}
	if(!str.compare("F14")){key.binding=VK_F14;return;}
	if(!str.compare("F15")){key.binding=VK_F15;return;}
	if(!str.compare("F16")){key.binding=VK_F16;return;}
	if(!str.compare("F17")){key.binding=VK_F17;return;}
	if(!str.compare("F18")){key.binding=VK_F18;return;}
	if(!str.compare("F19")){key.binding=VK_F19;return;}
	if(!str.compare("F20")){key.binding=VK_F20;return;}
	if(!str.compare("F21")){key.binding=VK_F21;return;}
	if(!str.compare("F22")){key.binding=VK_F22;return;}
	if(!str.compare("F23")){key.binding=VK_F23;return;}
	if(!str.compare("F24")){key.binding=VK_F24;return;}
	if(!str.compare("NUMLOCK")){key.binding=VK_NUMLOCK;return;}
	if(!str.compare("SCROLL")){key.binding=VK_SCROLL;return;}
	if(!str.compare("LSHIFT")){key.binding=VK_LSHIFT;return;}
	if(!str.compare("RSHIFT")){key.binding=VK_RSHIFT;return;}
	if(!str.compare("LCTRL")){key.binding=VK_LCONTROL;return;}
	if(!str.compare("RCTRL")){key.binding=VK_RCONTROL;return;}
	if(!str.compare("LMENU")){key.binding=VK_LMENU;return;}
	if(!str.compare("RMENU")){key.binding=VK_RMENU;return;}
	if(!str.compare("BTN3")){key.binding=VK_BUTTON3;return;}
	if(!str.compare("WHLUP")){key.binding=VK_WHEELUP;return;}
	if(!str.compare("WHLDN")){key.binding=VK_WHEELDOWN;return;}
	if(!str.compare("WHLLE")){key.binding=VK_WHEELLEFT;return;}
	if(!str.compare("WHLRI")){key.binding=VK_WHEELRIGHT;return;}
	if(!str.compare("KEYBA")){key.binding=0xBA;return;}
	if(!str.compare("PLUS")){key.binding=0xBB;return;}
	if(!str.compare("COMMA")){key.binding=0xBC;return;}
	if(!str.compare("MINUS")){key.binding=0xBD;return;}
	if(!str.compare("PERIOD")){key.binding=0xBE;return;}
	if(!str.compare("KEYBF")){key.binding=0xBF;return;}
	if(!str.compare("KEYC0")){key.binding=0xC0;return;}
	if(!str.compare("KEYDB")){key.binding=0xDB;return;}
	if(!str.compare("KEYDC")){key.binding=0xDC;return;}
	if(!str.compare("KEYDD")){key.binding=0xDD;return;}
	if(!str.compare("KEYDE")){key.binding=0xDE;return;}
	if(!str.compare("KEYDF")){key.binding=0xDF;return;}
	if(!str.compare("KEYE1")){key.binding=0xE1;return;}
	if(!str.compare("KEYE2")){key.binding=0xE2;return;}
	if(!str.compare("KEYE3")){key.binding=0xE3;return;}
	if(!str.compare("KEYE4")){key.binding=0xE4;return;}
	if(!str.compare("KEYE9")){key.binding=0xE9;return;}
	if(!str.compare("KEYEA")){key.binding=0xEA;return;}
	if(!str.compare("KEYEB")){key.binding=0xEB;return;}
	if(!str.compare("KEYEC")){key.binding=0xEC;return;}
	if(!str.compare("KEYED")){key.binding=0xED;return;}
	if(!str.compare("KEYEE")){key.binding=0xEE;return;}
	if(!str.compare("KEYEF")){key.binding=0xEF;return;}
	if(!str.compare("KEYF1")){key.binding=0xF1;return;}
	if(!str.compare("KEYF2")){key.binding=0xF2;return;}
	if(!str.compare("KEYF3")){key.binding=0xF3;return;}
	if(!str.compare("KEYF4")){key.binding=0xF4;return;}
	if(!str.compare("KEYF5")){key.binding=0xF5;return;}
	if(!str.compare("KEYHANGULKANA")){key.binding=0x15;return;}
	if(!str.compare("KEYJUNJA")){key.binding=0x17;return;}
	if(!str.compare("KEYFINAL")){key.binding=0x18;return;}
	if(!str.compare("KEYHANJAKANJI")){key.binding=0x19;return;}
	if(!str.compare("KEYCONVERT")){key.binding=0x1C;return;}
	if(!str.compare("KEYNONCONVERT")){key.binding=0x1D;return;}
	if(!str.compare("KEYACCEPT")){key.binding=0x1E;return;}
	if(!str.compare("KEYMODECHANGE")){key.binding=0x1F;return;}
	if(!str.compare("KEY92")){key.binding=0x92;return;}
	if(!str.compare("KEY93")){key.binding=0x93;return;}
	if(!str.compare("KEY94")){key.binding=0x94;return;}
	if(!str.compare("KEY95")){key.binding=0x95;return;}
	if(!str.compare("KEY96")){key.binding=0x96;return;}

	if(str.compare("NONE")&&str.compare("NOT_SUPPORTED"))
		{
		if(str[0]>='0'&&str[0]<='9')key.binding=str[0];
		if(str[0]>='A'&&str[0]<='Z')key.binding=str[0];
		return;
		}
}

void interfacest::addscreen(viewscreenst *scr,char pushtype,viewscreenst *relate)
{
	switch(pushtype)
		{
		case INTERFACE_PUSH_AS_PARENT:insertscreen_as_parent(scr,relate);break;
		case INTERFACE_PUSH_AS_CHILD:insertscreen_as_child(scr,relate);break;
		case INTERFACE_PUSH_AT_FRONT:insertscreen_at_front(scr);break;
		default:insertscreen_at_back(scr);break;
		}

	//WHENEVER A SCREEN IS ADDED, END ANNOUNCEMENTS
	if(gamemode==GAMEMODE_DWARF)dwarf_end_announcements();
}

void interfacest::insertscreen_as_parent(viewscreenst *scr,viewscreenst *child)
{
	if(child==NULL)
		{
		insertscreen_at_back(scr);
		return;
		}

	scr->child=child;
	scr->parent=child->parent;

	if(scr->parent!=NULL)scr->parent->child=scr;
	child->parent=scr;
}

void interfacest::insertscreen_as_child(viewscreenst *scr,viewscreenst *parent)
{
	if(parent==NULL)
		{
		insertscreen_at_back(scr);
		return;
		}

	scr->child=parent->child;
	scr->parent=parent;

	if(scr->child!=NULL)scr->child->parent=scr;
	parent->child=scr;
}

void interfacest::insertscreen_at_back(viewscreenst *scr)
{
	//GRAB CURRENT SCREEN AT THE END OF THE LIST
	viewscreenst *currentscreen=&view;
	while(currentscreen->child!=NULL)currentscreen=currentscreen->child;

	//PUT IT ON TO THE BACK SCREEN
	insertscreen_as_child(scr,currentscreen);
}

void interfacest::insertscreen_at_front(viewscreenst *scr)
{
	//PUT IT ON TO THE BASE
	insertscreen_as_child(scr,&view);
}

char interfacest::valid_held_key(unsigned char c)
{
	switch(c)
		{
		case VK_BACK:
		case VK_TAB:
		case VK_CLEAR:
		case VK_RETURN:
		//case VK_SHIFT:
		//case VK_CONTROL:
		//case VK_MENU:
		case VK_PAUSE:
		//case VK_CAPITAL:
		case VK_ESCAPE:
		case VK_SPACE:
		case VK_PRIOR:
		case VK_NEXT:
		case VK_END:
		case VK_HOME:
		case VK_LEFT:
		case VK_UP:
		case VK_RIGHT:
		case VK_DOWN:
		case VK_SELECT:
		case VK_PRINT:
		case VK_EXECUTE:
		case VK_SNAPSHOT:
		case VK_INSERT:
		case VK_DELETE:
		case VK_HELP:
		case 0x30://0
		case 0x31:
		case 0x32:
		case 0x33:
		case 0x34:
		case 0x35:
		case 0x36:
		case 0x37:
		case 0x38:
		case 0x39://9
		case 0x41://A
		case 0x42:
		case 0x43:
		case 0x44:
		case 0x45:
		case 0x46://F
		case 0x47:
		case 0x48:
		case 0x49:
		case 0x4A:
		case 0x4B://K
		case 0x4C:
		case 0x4D:
		case 0x4E:
		case 0x4F:
		case 0x50://P
		case 0x51:
		case 0x52:
		case 0x53:
		case 0x54:
		case 0x55://U
		case 0x56:
		case 0x57:
		case 0x58:
		case 0x59:
		case 0x5A://Z
		case VK_NUMPAD0:
		case VK_NUMPAD1:
		case VK_NUMPAD2:
		case VK_NUMPAD3:
		case VK_NUMPAD4:
		case VK_NUMPAD5:
		case VK_NUMPAD6:
		case VK_NUMPAD7:
		case VK_NUMPAD8:
		case VK_NUMPAD9:
		case VK_SEPARATOR:
		case VK_SUBTRACT:
		case VK_ADD:
		case VK_MULTIPLY:
		case VK_DECIMAL:
		case VK_DIVIDE:
		case VK_F1:
		case VK_F2:
		case VK_F3:
		case VK_F4:
		case VK_F5:
		case VK_F6:
		case VK_F7:
		case VK_F8:
		case VK_F9:
		case VK_F10:
		case VK_F11:
		case VK_F12:
		case VK_F13:
		case VK_F14:
		case VK_F15:
		case VK_F16:
		case VK_F17:
		case VK_F18:
		case VK_F19:
		case VK_F20:
		case VK_F21:
		case VK_F22:
		case VK_F23:
		case VK_F24:
		case VK_NUMLOCK:
		case VK_SCROLL:
		//case VK_LSHIFT:
		//case VK_RSHIFT:
		//case VK_LCONTROL:
		//case VK_RCONTROL:
		//case VK_LMENU:
		//case VK_RMENU:
		case VK_BUTTON3:
		case VK_WHEELUP:
		case VK_WHEELDOWN:
		case VK_WHEELLEFT:
		case VK_WHEELRIGHT:
		case 0xBA:
		case 0xBB:
		case 0xBC:
		case 0xBD:
		case 0xBE:
		case 0xBF:
		case 0xC0:
		case 0xDB:
		case 0xDC:
		case 0xDD:
		case 0xDE:
		case 0xDF:
		case 0xE1:
		case 0xE2:
		case 0xE3:
		case 0xE4:
		case 0xE9:
		case 0xEA:
		case 0xEB:
		case 0xEC:
		case 0xED:
		case 0xEE:
		case 0xEF:
		case 0xF1:
		case 0xF2:
		case 0xF3:
		case 0xF4:
		case 0xF5:
		case 0x15:
		case 0x17:
		case 0x18:
		case 0x19:
		case 0x1C:
		case 0x1D:
		case 0x1E:
		case 0x1F:
		case 0x92:
		case 0x93:
		case 0x94:
		case 0x95:
		case 0x96:
			return 1;
		}

	return 0;
}

char interfacest::loop()
{
#ifdef ENABLER
	if(shutdown_interface_for_ms>0)
		{
		//CLEAR ALL THE KEYS
		enabler.clear_input();

		//DONE
		if(enabler.now-shutdown_interface_tickcount>=shutdown_interface_for_ms||
			shutdown_interface_tickcount>enabler.now)
			{
			shutdown_interface_for_ms=0;
			}
		}

#endif

	//GRAB CURRENT SCREEN AT THE END OF THE LIST
	viewscreenst *currentscreen=&view;
	while(currentscreen->child!=NULL)currentscreen=currentscreen->child;

	//NO INTERFACE LEFT, QUIT
	if(currentscreen==&view)return 1;

	//MOVE SCREENS BACK
	switch(currentscreen->breakdownlevel)
		{
		case INTERFACE_BREAKDOWN_NONE:
			{
			//GATHER INPUT
			enabler_inputst* input;
            int count=enabler.inputcount();
			if(count)
				{
				if(c==0||!(flag & INTERFACEFLAG_RETAIN_NONZERO_INPUT))
					{
                    long i;
					long new_hold_time;
                    for (i=0;i<count;++i) {
					 input=enabler.getinput(i);
                     c=symToVkey[input->key];

					merge_binding(c);

                     if (enabler.now>=input->next_process) {
                      if ((input->processed)&&(!valid_held_key(c))) continue; //don't do it
					  caps=input->caps;
					  shift=input->shift;
                      upper=input->upper; //not using this yet
					  ctrl=input->ctrl;
					  alt=input->alt;
					if(shift)shift=1;
                      // No initial delay for cursor keys no matter what they are bound to
                      if (keypress(INTERFACEKEY_CURSOR_UP_FAST)) new_hold_time=init.input.hold_time;
                      else if (keypress(INTERFACEKEY_CURSOR_DOWN_FAST)) new_hold_time=init.input.hold_time;
                      else if (keypress(INTERFACEKEY_CURSOR_LEFT_FAST)) new_hold_time=init.input.hold_time;
                      else if (keypress(INTERFACEKEY_CURSOR_RIGHT_FAST)) new_hold_time=init.input.hold_time;
                      else if (keypress(INTERFACEKEY_CURSOR_UPLEFT_FAST)) new_hold_time=init.input.hold_time;
                      else if (keypress(INTERFACEKEY_CURSOR_UPRIGHT_FAST)) new_hold_time=init.input.hold_time;
                      else if (keypress(INTERFACEKEY_CURSOR_DOWNLEFT_FAST)) new_hold_time=init.input.hold_time;
                      else if (keypress(INTERFACEKEY_CURSOR_DOWNRIGHT_FAST)) new_hold_time=init.input.hold_time;
                      else if (keypress(INTERFACEKEY_STANDARDSCROLL_UP)) new_hold_time=init.input.hold_time;
                      else if (keypress(INTERFACEKEY_STANDARDSCROLL_DOWN)) new_hold_time=init.input.hold_time;
                      else if (keypress(INTERFACEKEY_STANDARDSCROLL_LEFT)) new_hold_time=init.input.hold_time;
                      else if (keypress(INTERFACEKEY_STANDARDSCROLL_RIGHT)) new_hold_time=init.input.hold_time;
                      else if (keypress(INTERFACEKEY_STANDARDSCROLL_PAGEUP)) new_hold_time=init.input.hold_time;
                      else if (keypress(INTERFACEKEY_STANDARDSCROLL_PAGEDOWN)) new_hold_time=init.input.hold_time;
                      else if (keypress(INTERFACEKEY_SECONDSCROLL_UP)) new_hold_time=init.input.hold_time;
                      else if (keypress(INTERFACEKEY_SECONDSCROLL_DOWN)) new_hold_time=init.input.hold_time;
                      else if (keypress(INTERFACEKEY_SECONDSCROLL_PAGEUP)) new_hold_time=init.input.hold_time;
                      else if (keypress(INTERFACEKEY_SECONDSCROLL_PAGEDOWN)) new_hold_time=init.input.hold_time;
                      else if (keypress(INTERFACEKEY_CURSOR_UP)) new_hold_time=init.input.hold_time;
                      else if (keypress(INTERFACEKEY_CURSOR_DOWN)) new_hold_time=init.input.hold_time;
                      else if (keypress(INTERFACEKEY_CURSOR_LEFT)) new_hold_time=init.input.hold_time;
                      else if (keypress(INTERFACEKEY_CURSOR_RIGHT)) new_hold_time=init.input.hold_time;
                      else if (keypress(INTERFACEKEY_CURSOR_UPLEFT)) new_hold_time=init.input.hold_time;
                      else if (keypress(INTERFACEKEY_CURSOR_UPRIGHT)) new_hold_time=init.input.hold_time;
                      else if (keypress(INTERFACEKEY_CURSOR_DOWNLEFT)) new_hold_time=init.input.hold_time;
                      else if (keypress(INTERFACEKEY_CURSOR_DOWNRIGHT)) new_hold_time=init.input.hold_time;
                      else if (keypress(INTERFACEKEY_CURSOR_UP_Z)) new_hold_time=init.input.hold_time;
                      else if (keypress(INTERFACEKEY_CURSOR_DOWN_Z)) new_hold_time=init.input.hold_time;
                      else if (keypress(INTERFACEKEY_CURSOR_UP_Z_AUX)) new_hold_time=init.input.hold_time;
                      else if (keypress(INTERFACEKEY_CURSOR_DOWN_Z_AUX)) new_hold_time=init.input.hold_time;
					  else if (input->processed)new_hold_time=init.input.hold_time/2;
                      else new_hold_time=init.input.hold_time*3;
                      input->processed = enabler.now;
                      input->next_process = enabler.now + new_hold_time;
                      break; //we are keeping this one
                     }
                    }
                    if (i==count) c=0; //didn't find a good input
					}
				}
			else if(!(flag & INTERFACEFLAG_RETAIN_NONZERO_INPUT))c=0;
			flag&=~INTERFACEFLAG_RETAIN_NONZERO_INPUT;

			//TOGGLE SCREEN
			if(gview.keypress(INTERFACEKEY_TOGGLE_FULLSCREEN))
				{
				enabler.toggle_fullscreen();
				gview.c=0;
				}

			//FRAME COUNT
			if(gps.display_frames && !enabler.doing_buffer_draw())
				{
				QueryPerformanceCounter(&gps.print_time[gps.print_index]);
				gps.print_index=(gps.print_index+1)%100;
				}

			if(currentscreen->is_legacy_screen())
				{
				//GAME OPTIONS
				if(gview.keypress(INTERFACEKEY_OPTIONS)&&!currentscreen->key_conflict(INTERFACEKEY_OPTIONS))
					{
					//PEEL BACK ALL SCREENS TO THE CURRENT OPTION SCREEN IF THERE IS ONE
						//UNLESS THERE IS A BLOCKING SCREEN LIKE THE REGION MAKER
					viewscreenst *opscreen=&view;
					while(opscreen!=NULL)
						{
						if(opscreen->is_option_screen())
							{
							opscreen->option_key_pressed=1;
							while(opscreen->child!=NULL)
								{
								if(opscreen->child->is_option_screen()==2)
									{
									opscreen->child->option_key_pressed=1;
									opscreen->option_key_pressed=0;
									break;
									}
								removescreen(opscreen->child);
								}
							break;
							}

						opscreen=opscreen->child;
						}

					//NEED A NEW OPTIONS SCREEN?
					if(opscreen==NULL)dwarf_option_screen();

					//REFRESH CURRENT SCREEN
						//GRAB CURRENT SCREEN AT THE END OF THE LIST
					currentscreen=&view;
					while(currentscreen->child!=NULL)currentscreen=currentscreen->child;

						//NO INTERFACE LEFT, QUIT
					if(currentscreen==&view)return 1;

					c=0;
					}
				//DO MOVIE COMMANDS
				else if(gview.keypress(INTERFACEKEY_MOVIES)&&!currentscreen->key_conflict(INTERFACEKEY_MOVIES))
					{
					if(currentscreen->movies_okay())use_movie_input();
					}
				//RUN THE INTERFACE AND SEE HOW FAR TO PEEL IT BACK
				else if(gview.keypress(INTERFACEKEY_HELP)&&!currentscreen->key_conflict(INTERFACEKEY_HELP))
					{
					//WE INTERCEPT THE HELP KEY NO MATTER WHAT
					currentscreen->help();
					c=0;
					}
				else currentscreen->view();
				}
			else
				{
				//GAME OPTIONS
				if(gview.keypress(INTERFACEKEY_OPTIONS)&&!currentscreen->key_conflict(INTERFACEKEY_OPTIONS))
					{
					//PEEL BACK ALL SCREENS TO THE CURRENT OPTION SCREEN IF THERE IS ONE
						//UNLESS THERE IS A BLOCKING SCREEN LIKE THE REGION MAKER
					viewscreenst *opscreen=&view;
					while(opscreen!=NULL)
						{
						if(opscreen->is_option_screen())
							{
							opscreen->option_key_pressed=1;
							while(opscreen->child!=NULL)
								{
								if(opscreen->child->is_option_screen()==2)
									{
									opscreen->child->option_key_pressed=1;
									opscreen->option_key_pressed=0;
									break;
									}
								removescreen(opscreen->child);
								}
							break;
							}

						opscreen=opscreen->child;
						}

					//NEED A NEW OPTIONS SCREEN?
					if(opscreen==NULL)dwarf_option_screen();

					//REFRESH CURRENT SCREEN
						//GRAB CURRENT SCREEN AT THE END OF THE LIST
					currentscreen=&view;
					while(currentscreen->child!=NULL)currentscreen=currentscreen->child;

						//NO INTERFACE LEFT, QUIT
					if(currentscreen==&view)return 1;

					c=0;
					}
				//DO MOVIE COMMANDS
				else if(gview.keypress(INTERFACEKEY_MOVIES)&&!currentscreen->key_conflict(INTERFACEKEY_MOVIES))
					{
					if(currentscreen->movies_okay())use_movie_input();
					}
				//RUN THE INTERFACE AND SEE HOW FAR TO PEEL IT BACK
				else if(gview.keypress(INTERFACEKEY_HELP)&&!currentscreen->key_conflict(INTERFACEKEY_HELP))
					{
					//WE INTERCEPT THE HELP KEY NO MATTER WHAT
					currentscreen->help();
					c=0;
					}
				else
					{
					currentscreen->input();

					viewscreenst *n_currentscreen=&view;
					while(n_currentscreen->child!=NULL)n_currentscreen=n_currentscreen->child;

					if(currentscreen==n_currentscreen)currentscreen->logic();

					c=0;
					}
				}

			if(currentscreen->movies_okay())
				{
				//HANDLE MOVIES
				handlemovie(0);
				}

			//PRINT THE MOUSE
			if(!init.input.flag.has_flag(INIT_INPUT_FLAG_MOUSE_OFF))
			  {
			    if(enabler.tracking_on)
			      {
				if(init.input.flag.has_flag(INIT_INPUT_FLAG_MOUSE_PICTURE))
				  {
// 				    enabler.set_color(1,1,1);
// 				    enabler.locate(enabler.mouse_x,enabler.mouse_y);
				    // FIXME: Do something to display the mouse
				    //enabler.add_tile(gps.tex_pos[TEXTURE_MOUSE]);
				    enabler.set_tile(gps.tex_pos[TEXTURE_MOUSE],
						     TEXTURE_MOUSE,
						     enabler.mouse_x,
						     enabler.mouse_y);
				    while(Enabler_ShowCursor(FALSE)>0);
				  }
				else while(Enabler_ShowCursor(TRUE)<=0);
			      }
			    else while(Enabler_ShowCursor(TRUE)<=0);
			  }
			else if(enabler.create_full_screen)
			  {
			    while(Enabler_ShowCursor(FALSE)>0);
			  }
			else  while(Enabler_ShowCursor(TRUE)<=0);

			break;
			}
		case INTERFACE_BREAKDOWN_QUIT:
			{
			handlemovie(1);
			return 1;
			}
		case INTERFACE_BREAKDOWN_STOPSCREEN:
			if(currentscreen->movies_okay())
				{
				//HANDLE MOVIES
				handlemovie(0);
				}

			removescreen(currentscreen);
			break;
		case INTERFACE_BREAKDOWN_TOFIRST:
			if(currentscreen->movies_okay())
				{
				//HANDLE MOVIES
				handlemovie(0);
				}

			remove_to_first();
			break;
		}

	return 0;
}

void interfacest::remove_to_first()
{
	//GRAB LAST SCREEN AT THE END OF THE LIST
	viewscreenst *lastscreen=&view;
	while(lastscreen->child!=NULL)lastscreen=lastscreen->child;

	//NO INTERFACE LEFT
	if(lastscreen==&view)return;

	//GO AHEAD
	while(lastscreen->parent!=&view)
		{
		viewscreenst *par=lastscreen->parent;
		removescreen(lastscreen);
		lastscreen=par;
		}
}

void interfacest::removescreen(viewscreenst *scr)
{
	//THE MINIMAP IS EXPENSIVE, SO WE REFRESH IT WHENEVER INTERFACE GETS IN THE WAY
	if(gamemode==GAMEMODE_DWARF)dwarf_remove_screen();

	//FIX LINKS
	if(scr->parent!=NULL)scr->parent->child=scr->child;
	if(scr->child!=NULL)scr->child->parent=scr->parent;

	//WASTE SCREEN
	delete scr;
}

long interfacest::write_movie_chunk()
{
	long inputsize=supermovie_pos;
	if(inputsize>MOVIEBUFFSIZE)inputsize=MOVIEBUFFSIZE;

	//DUMP CURRENT BUFFER INTO A COMPRESSION STREAM
	z_stream c_stream;
	int err;

	c_stream.zalloc = (alloc_func)0;
	c_stream.zfree = (free_func)0;
	c_stream.opaque = (voidpf)0;

	err = deflateInit(&c_stream, 9);
	CHECK_ERR(err, "deflateInit");

	c_stream.next_out = (Bytef*)supermoviebuffer_comp;
	c_stream.avail_out = COMPMOVIEBUFFSIZE;

	c_stream.next_in  = (Bytef*)supermoviebuffer;
	c_stream.avail_in  = inputsize;

	while (c_stream.total_in != inputsize && c_stream.total_out < COMPMOVIEBUFFSIZE) {
		//c_stream.avail_in = c_stream.avail_out = 1; // force small buffers
		err = deflate(&c_stream, Z_NO_FLUSH);
		CHECK_ERR(err, "deflate");
		}

	// Finish the stream, still forcing small buffers:
	for (;;) {
		err = deflate(&c_stream, Z_FINISH);
		if (err == Z_STREAM_END) break;
		CHECK_ERR(err, "deflate");
		}

	err = deflateEnd(&c_stream);
	CHECK_ERR(err, "deflateEnd");

	long length=0;

	if(c_stream.total_out>0)
		{
		if(first_movie_write)
			{
			//GET RID OF ANY EXISTING MOVIES IF THIS IS THE FIRST TIME THROUGH
			unlink(movie_file.c_str());
			}

		//OPEN UP THE MOVIE FILE AND APPEND
		std::fstream f;
		f.open(movie_file.c_str(), fstream::out | fstream::binary | fstream::app);

		if(f.is_open())
			{
			//WRITE A HEADER
			if(first_movie_write)
				{
				long swp_l=byteswap(movie_version);
				f.write((const char *)&swp_l,sizeof(long));


				cursesmovie_headerst cmh;
					cmh.dimx=init.display.grid_x;
					cmh.dimy=init.display.grid_y;
					cmh.delayrate=supermovie_delayrate;
					cmh.dimx=byteswap(cmh.dimx);
					cmh.dimy=byteswap(cmh.dimy);
					cmh.delayrate=byteswap(cmh.delayrate);
				f.write((const char *)&cmh,sizeof(cursesmovie_headerst));

				long s;
				s=byteswap(gview.supermovie_sound.str.size());
				f.write((const char *)&s,sizeof(long));
				char buf[50];
				for(s=0;s<gview.supermovie_sound.str.size();s++)
					{
					strcpy(buf,gview.supermovie_sound.str[s]->dat.c_str());
					f.write(buf,sizeof(char)*50);
					}

				long i1,i2;
				for(i1=0;i1<200;i1++)
					{
					for(i2=0;i2<SOUND_CHANNELNUM;i2++)
						{
						swp_l=byteswap(gview.supermovie_sound_time[i1][i2]);
						f.write((const char *)&swp_l,sizeof(long));
						}
					}
				}

			//WRITE IT
			long compsize=byteswap(c_stream.total_out);
			f.write((const char *)&compsize,sizeof(long));
			f.write((const char *)supermoviebuffer_comp,c_stream.total_out);

			f.seekg(0,ios::beg);
			long beg=f.tellg();
			f.seekg(0,ios::end);
			long end=f.tellg();
			length=end-beg;

			f.close();
			}
		else supermovie_on=0;

		first_movie_write=0;
		}

	return length;
}

void interfacest::read_movie_chunk(long &maxmoviepos,char &is_playing)
{
	//OPEN UP THE MOVIE FILE AND MOVE TO CORRECT POSITION
	std::fstream f;
	f.open(movie_file.c_str(), fstream::in | fstream::out | fstream::binary);

	if(f.is_open())
		{
		f.seekg(0,ios::beg);
		long beg=f.tellg();
		f.seekg(0,ios::end);
		long end=f.tellg();
		long file_size=end-beg;

		if(gview.nextfilepos<file_size)
			{
			f.seekg(gview.nextfilepos,ios::beg);

			//LOAD THE HEADER
			char fail=0;
			if(gview.nextfilepos==0)
				{
				long loadversion;
				f.read((char *)&loadversion,sizeof(long));
				loadversion=byteswap(loadversion);

				if(loadversion>movie_version)fail=1;

				cursesmovie_headerst cmh;
				f.read((char *)&cmh,sizeof(cursesmovie_headerst));
					cmh.dimx=byteswap(cmh.dimx);
					cmh.dimy=byteswap(cmh.dimy);
					cmh.delayrate=byteswap(cmh.delayrate);

				init.display.grid_x=cmh.dimx;
				init.display.grid_y=cmh.dimy;
				gps.prepare_rect(0);

				gview.supermovie_delayrate=cmh.delayrate;
				gview.supermovie_delaystep=cmh.delayrate;

				gview.supermovie_sound.clean();
				if(loadversion>=10001)
					{
					long num;
					f.read((char *)&num,sizeof(long));
					num=byteswap(num);
					gview.nextfilepos+=sizeof(long);
					char buf[50];
					long s;
					for(s=0;s<num;s++)
						{
						f.read(buf,sizeof(char)*50);
						string str=buf;
						gview.supermovie_sound.add_string(str);
						gview.nextfilepos+=sizeof(char)*50;
						}

					long i1,i2,swp_l;
					for(i1=0;i1<200;i1++)
						{
						for(i2=0;i2<SOUND_CHANNELNUM;i2++)
							{
							f.read((char *)&swp_l,sizeof(long));
							gview.supermovie_sound_time[i1][i2]=byteswap(swp_l);
							}
						}

					gview.nextfilepos+=sizeof(long)*200*SOUND_CHANNELNUM;
					}
				else
					{
					long i,c;
					for(i=0;i<200;i++)
						{
						for(c=0;c<SOUND_CHANNELNUM;c++)gview.supermovie_sound_time[i][c]=-1;
						}
					}

				gview.nextfilepos+=sizeof(long)+sizeof(cursesmovie_headerst);

#ifndef NO_FMOD
				//HANDLE SOUND LOADING
				long s;
				for(s=0;s<gview.supermovie_sound.str.size();s++)
					{
					string filename="data/sound/";
					filename+=gview.supermovie_sound.str[s]->dat;
					filename+=".ogg";
					
					musicsound.set_sound(filename,s);
					}
#endif
				}

			if(!fail)
				{
				//READ IT
				f.read((char *)&gview.currentblocksize,sizeof(long));
				gview.currentblocksize=byteswap(gview.currentblocksize);
				f.read((char *)gview.supermoviebuffer_comp,gview.currentblocksize);

				gview.nextfilepos+=gview.currentblocksize+sizeof(long);

				//UNCOMPRESS IT
				z_stream d_stream; // decompression stream

				d_stream.zalloc = (alloc_func)0;
				d_stream.zfree = (free_func)0;
				d_stream.opaque = (voidpf)0;

				d_stream.next_in  = (Bytef*)gview.supermoviebuffer_comp;
				d_stream.avail_in = gview.currentblocksize;

				int err = inflateInit(&d_stream);
				CHECK_ERR(err, "inflateInit");

				d_stream.next_out = gview.supermoviebuffer;
				d_stream.avail_out = MOVIEBUFFSIZE;

				while (d_stream.total_out < MOVIEBUFFSIZE && d_stream.total_in < gview.currentblocksize) {
					//d_stream.avail_in = d_stream.avail_out = 1; // force small buffers
					err = inflate(&d_stream, Z_NO_FLUSH);
					if (err == Z_STREAM_END) break;
					CHECK_ERR(err, "inflate");
					}

				err = inflateEnd(&d_stream);
				CHECK_ERR(err, "inflateEnd");

				gview.supermovie_pos=0;
				maxmoviepos=d_stream.total_out;
				}
			else
				{
				is_playing=0;
				init.display.grid_x=init.display.orig_grid_x;
				init.display.grid_y=init.display.orig_grid_y;
				gps.prepare_rect(1);
				}
			}
		else
			{
			is_playing=0;
			init.display.grid_x=init.display.orig_grid_x;
			init.display.grid_y=init.display.orig_grid_y;
			gps.prepare_rect(1);
			}

		f.close();
		}
	else
		{
		is_playing=0;
		init.display.grid_x=init.display.orig_grid_x;
		init.display.grid_y=init.display.orig_grid_y;
		gps.prepare_rect(1);
		}
}

void interfacest::handlemovie(char flushall)
{
	//SAVE A MOVIE FRAME INTO THE CURRENT MOVIE BUFFER
	if(supermovie_on==1)
		{
		if(supermovie_delaystep>0&&!flushall)supermovie_delaystep--;
		else
			{
			if(!flushall)supermovie_delaystep=supermovie_delayrate;

			if(!flushall||supermovie_delaystep==0)
				{
				//SAVING CHARACTERS, THEN COLORS
				short x2,y2;
				for(x2=0;x2<init.display.grid_x;x2++)
					{
					for(y2=0;y2<init.display.grid_y;y2++)
						{
						//Core50Core50Core50Core50Core50Core50
						supermoviebuffer[supermovie_pos]=gps.screen[x2][y2][0];

						supermovie_pos++;
						}
					}
				char frame_col;
				for(x2=0;x2<init.display.grid_x;x2++)
					{
					for(y2=0;y2<init.display.grid_y;y2++)
						{
						//Core50Core50Core50Core50Core50Core50
						frame_col=gps.screen[x2][y2][1];
						frame_col|=(gps.screen[x2][y2][2]<<3);
						if(gps.screen[x2][y2][3])frame_col|=64;
						supermoviebuffer[supermovie_pos]=frame_col;

						supermovie_pos++;
						}
					}
				}

			long frame_size=init.display.grid_x*init.display.grid_y*2;
			if(supermovie_pos+frame_size>=MOVIEBUFFSIZE||flushall)
				{
				long length=write_movie_chunk();

				if(length>5000000)
					{
					finish_movie();
					}
				else supermovie_pos=0;
				}
			}
		}
}

void interfacest::print_interface_token(InterfaceKey key)
{
	short o_screenf=gps.screenf,o_screenb=gps.screenb,o_screenbright=gps.screenbright;
	gps.changecolor(2,0,1);
	string tok;
	getstring_interfacechar(tok,interfacekey[key]);
	gps.addst(tok);
	gps.changecolor(o_screenf,o_screenb,o_screenbright);
}

char standardstringentry(char *str,int maxlen,unsigned long flag)
{
	string str2;
	str2=str;
	char ret=standardstringentry(str2,maxlen,flag);
	strcpy(str,str2.c_str());
	return ret;
}

char standardstringentry(string &str,int maxlen,unsigned long flag)
{
	unsigned char entry=255;
	if(flag & STRINGENTRY_LETTERS)
		{
		if(gview.keypress(INTERFACEKEY_STRING_A097))entry='a';
		if(gview.keypress(INTERFACEKEY_STRING_A098))entry='b';
		if(gview.keypress(INTERFACEKEY_STRING_A099))entry='c';
		if(gview.keypress(INTERFACEKEY_STRING_A100))entry='d';
		if(gview.keypress(INTERFACEKEY_STRING_A101))entry='e';
		if(gview.keypress(INTERFACEKEY_STRING_A102))entry='f';
		if(gview.keypress(INTERFACEKEY_STRING_A103))entry='g';
		if(gview.keypress(INTERFACEKEY_STRING_A104))entry='h';
		if(gview.keypress(INTERFACEKEY_STRING_A105))entry='i';
		if(gview.keypress(INTERFACEKEY_STRING_A106))entry='j';
		if(gview.keypress(INTERFACEKEY_STRING_A107))entry='k';
		if(gview.keypress(INTERFACEKEY_STRING_A108))entry='l';
		if(gview.keypress(INTERFACEKEY_STRING_A109))entry='m';
		if(gview.keypress(INTERFACEKEY_STRING_A110))entry='n';
		if(gview.keypress(INTERFACEKEY_STRING_A111))entry='o';
		if(gview.keypress(INTERFACEKEY_STRING_A112))entry='p';
		if(gview.keypress(INTERFACEKEY_STRING_A113))entry='q';
		if(gview.keypress(INTERFACEKEY_STRING_A114))entry='r';
		if(gview.keypress(INTERFACEKEY_STRING_A115))entry='s';
		if(gview.keypress(INTERFACEKEY_STRING_A116))entry='t';
		if(gview.keypress(INTERFACEKEY_STRING_A117))entry='u';
		if(gview.keypress(INTERFACEKEY_STRING_A118))entry='v';
		if(gview.keypress(INTERFACEKEY_STRING_A119))entry='w';
		if(gview.keypress(INTERFACEKEY_STRING_A120))entry='x';
		if(gview.keypress(INTERFACEKEY_STRING_A121))entry='y';
		if(gview.keypress(INTERFACEKEY_STRING_A122))entry='z';
		if(gview.keypress(INTERFACEKEY_STRING_A065))entry='A';
		if(gview.keypress(INTERFACEKEY_STRING_A066))entry='B';
		if(gview.keypress(INTERFACEKEY_STRING_A067))entry='C';
		if(gview.keypress(INTERFACEKEY_STRING_A068))entry='D';
		if(gview.keypress(INTERFACEKEY_STRING_A069))entry='E';
		if(gview.keypress(INTERFACEKEY_STRING_A070))entry='F';
		if(gview.keypress(INTERFACEKEY_STRING_A071))entry='G';
		if(gview.keypress(INTERFACEKEY_STRING_A072))entry='H';
		if(gview.keypress(INTERFACEKEY_STRING_A073))entry='I';
		if(gview.keypress(INTERFACEKEY_STRING_A074))entry='J';
		if(gview.keypress(INTERFACEKEY_STRING_A075))entry='K';
		if(gview.keypress(INTERFACEKEY_STRING_A076))entry='L';
		if(gview.keypress(INTERFACEKEY_STRING_A077))entry='M';
		if(gview.keypress(INTERFACEKEY_STRING_A078))entry='N';
		if(gview.keypress(INTERFACEKEY_STRING_A079))entry='O';
		if(gview.keypress(INTERFACEKEY_STRING_A080))entry='P';
		if(gview.keypress(INTERFACEKEY_STRING_A081))entry='Q';
		if(gview.keypress(INTERFACEKEY_STRING_A082))entry='R';
		if(gview.keypress(INTERFACEKEY_STRING_A083))entry='S';
		if(gview.keypress(INTERFACEKEY_STRING_A084))entry='T';
		if(gview.keypress(INTERFACEKEY_STRING_A085))entry='U';
		if(gview.keypress(INTERFACEKEY_STRING_A086))entry='V';
		if(gview.keypress(INTERFACEKEY_STRING_A087))entry='W';
		if(gview.keypress(INTERFACEKEY_STRING_A088))entry='X';
		if(gview.keypress(INTERFACEKEY_STRING_A089))entry='Y';
		if(gview.keypress(INTERFACEKEY_STRING_A090))entry='Z';
		}
	if(flag & STRINGENTRY_SPACE)
		{
		if(gview.keypress(INTERFACEKEY_STRING_A032))entry=' ';
		}
	if(gview.keypress(INTERFACEKEY_STRING_A000))entry='\x0';
	if(flag & STRINGENTRY_NUMBERS)
		{
		if(gview.keypress(INTERFACEKEY_STRING_A048))entry='0';
		if(gview.keypress(INTERFACEKEY_STRING_A049))entry='1';
		if(gview.keypress(INTERFACEKEY_STRING_A050))entry='2';
		if(gview.keypress(INTERFACEKEY_STRING_A051))entry='3';
		if(gview.keypress(INTERFACEKEY_STRING_A052))entry='4';
		if(gview.keypress(INTERFACEKEY_STRING_A053))entry='5';
		if(gview.keypress(INTERFACEKEY_STRING_A054))entry='6';
		if(gview.keypress(INTERFACEKEY_STRING_A055))entry='7';
		if(gview.keypress(INTERFACEKEY_STRING_A056))entry='8';
		if(gview.keypress(INTERFACEKEY_STRING_A057))entry='9';
		}
	if(flag & STRINGENTRY_SYMBOLS)
		{
		if(gview.keypress(INTERFACEKEY_STRING_A000))entry=0;
		if(gview.keypress(INTERFACEKEY_STRING_A001))entry=1;
		if(gview.keypress(INTERFACEKEY_STRING_A002))entry=2;
		if(gview.keypress(INTERFACEKEY_STRING_A003))entry=3;
		if(gview.keypress(INTERFACEKEY_STRING_A004))entry=4;
		if(gview.keypress(INTERFACEKEY_STRING_A005))entry=5;
		if(gview.keypress(INTERFACEKEY_STRING_A006))entry=6;
		if(gview.keypress(INTERFACEKEY_STRING_A007))entry=7;
		if(gview.keypress(INTERFACEKEY_STRING_A008))entry=8;
		if(gview.keypress(INTERFACEKEY_STRING_A009))entry=9;
		if(gview.keypress(INTERFACEKEY_STRING_A010))entry=10;
		if(gview.keypress(INTERFACEKEY_STRING_A011))entry=11;
		if(gview.keypress(INTERFACEKEY_STRING_A012))entry=12;
		if(gview.keypress(INTERFACEKEY_STRING_A013))entry=13;
		if(gview.keypress(INTERFACEKEY_STRING_A014))entry=14;
		if(gview.keypress(INTERFACEKEY_STRING_A015))entry=15;
		if(gview.keypress(INTERFACEKEY_STRING_A016))entry=16;
		if(gview.keypress(INTERFACEKEY_STRING_A017))entry=17;
		if(gview.keypress(INTERFACEKEY_STRING_A018))entry=18;
		if(gview.keypress(INTERFACEKEY_STRING_A019))entry=19;
		if(gview.keypress(INTERFACEKEY_STRING_A020))entry=20;
		if(gview.keypress(INTERFACEKEY_STRING_A021))entry=21;
		if(gview.keypress(INTERFACEKEY_STRING_A022))entry=22;
		if(gview.keypress(INTERFACEKEY_STRING_A023))entry=23;
		if(gview.keypress(INTERFACEKEY_STRING_A024))entry=24;
		if(gview.keypress(INTERFACEKEY_STRING_A025))entry=25;
		if(gview.keypress(INTERFACEKEY_STRING_A026))entry=26;
		if(gview.keypress(INTERFACEKEY_STRING_A027))entry=27;
		if(gview.keypress(INTERFACEKEY_STRING_A028))entry=28;
		if(gview.keypress(INTERFACEKEY_STRING_A029))entry=29;
		if(gview.keypress(INTERFACEKEY_STRING_A030))entry=30;
		if(gview.keypress(INTERFACEKEY_STRING_A031))entry=31;
		if(gview.keypress(INTERFACEKEY_STRING_A032))entry=32;
		if(gview.keypress(INTERFACEKEY_STRING_A033))entry=33;
		if(gview.keypress(INTERFACEKEY_STRING_A034))entry=34;
		if(gview.keypress(INTERFACEKEY_STRING_A035))entry=35;
		if(gview.keypress(INTERFACEKEY_STRING_A036))entry=36;
		if(gview.keypress(INTERFACEKEY_STRING_A037))entry=37;
		if(gview.keypress(INTERFACEKEY_STRING_A038))entry=38;
		if(gview.keypress(INTERFACEKEY_STRING_A039))entry=39;
		if(gview.keypress(INTERFACEKEY_STRING_A040))entry=40;
		if(gview.keypress(INTERFACEKEY_STRING_A041))entry=41;
		if(gview.keypress(INTERFACEKEY_STRING_A042))entry=42;
		if(gview.keypress(INTERFACEKEY_STRING_A043))entry=43;
		if(gview.keypress(INTERFACEKEY_STRING_A044))entry=44;
		if(gview.keypress(INTERFACEKEY_STRING_A045))entry=45;
		if(gview.keypress(INTERFACEKEY_STRING_A046))entry=46;
		if(gview.keypress(INTERFACEKEY_STRING_A047))entry=47;
		if(gview.keypress(INTERFACEKEY_STRING_A048))entry=48;
		if(gview.keypress(INTERFACEKEY_STRING_A049))entry=49;
		if(gview.keypress(INTERFACEKEY_STRING_A050))entry=50;
		if(gview.keypress(INTERFACEKEY_STRING_A051))entry=51;
		if(gview.keypress(INTERFACEKEY_STRING_A052))entry=52;
		if(gview.keypress(INTERFACEKEY_STRING_A053))entry=53;
		if(gview.keypress(INTERFACEKEY_STRING_A054))entry=54;
		if(gview.keypress(INTERFACEKEY_STRING_A055))entry=55;
		if(gview.keypress(INTERFACEKEY_STRING_A056))entry=56;
		if(gview.keypress(INTERFACEKEY_STRING_A057))entry=57;
		if(gview.keypress(INTERFACEKEY_STRING_A058))entry=58;
		if(gview.keypress(INTERFACEKEY_STRING_A059))entry=59;
		if(gview.keypress(INTERFACEKEY_STRING_A060))entry=60;
		if(gview.keypress(INTERFACEKEY_STRING_A061))entry=61;
		if(gview.keypress(INTERFACEKEY_STRING_A062))entry=62;
		if(gview.keypress(INTERFACEKEY_STRING_A063))entry=63;
		if(gview.keypress(INTERFACEKEY_STRING_A064))entry=64;
		if(gview.keypress(INTERFACEKEY_STRING_A065))entry=65;
		if(gview.keypress(INTERFACEKEY_STRING_A066))entry=66;
		if(gview.keypress(INTERFACEKEY_STRING_A067))entry=67;
		if(gview.keypress(INTERFACEKEY_STRING_A068))entry=68;
		if(gview.keypress(INTERFACEKEY_STRING_A069))entry=69;
		if(gview.keypress(INTERFACEKEY_STRING_A070))entry=70;
		if(gview.keypress(INTERFACEKEY_STRING_A071))entry=71;
		if(gview.keypress(INTERFACEKEY_STRING_A072))entry=72;
		if(gview.keypress(INTERFACEKEY_STRING_A073))entry=73;
		if(gview.keypress(INTERFACEKEY_STRING_A074))entry=74;
		if(gview.keypress(INTERFACEKEY_STRING_A075))entry=75;
		if(gview.keypress(INTERFACEKEY_STRING_A076))entry=76;
		if(gview.keypress(INTERFACEKEY_STRING_A077))entry=77;
		if(gview.keypress(INTERFACEKEY_STRING_A078))entry=78;
		if(gview.keypress(INTERFACEKEY_STRING_A079))entry=79;
		if(gview.keypress(INTERFACEKEY_STRING_A080))entry=80;
		if(gview.keypress(INTERFACEKEY_STRING_A081))entry=81;
		if(gview.keypress(INTERFACEKEY_STRING_A082))entry=82;
		if(gview.keypress(INTERFACEKEY_STRING_A083))entry=83;
		if(gview.keypress(INTERFACEKEY_STRING_A084))entry=84;
		if(gview.keypress(INTERFACEKEY_STRING_A085))entry=85;
		if(gview.keypress(INTERFACEKEY_STRING_A086))entry=86;
		if(gview.keypress(INTERFACEKEY_STRING_A087))entry=87;
		if(gview.keypress(INTERFACEKEY_STRING_A088))entry=88;
		if(gview.keypress(INTERFACEKEY_STRING_A089))entry=89;
		if(gview.keypress(INTERFACEKEY_STRING_A090))entry=90;
		if(gview.keypress(INTERFACEKEY_STRING_A091))entry=91;
		if(gview.keypress(INTERFACEKEY_STRING_A092))entry=92;
		if(gview.keypress(INTERFACEKEY_STRING_A093))entry=93;
		if(gview.keypress(INTERFACEKEY_STRING_A094))entry=94;
		if(gview.keypress(INTERFACEKEY_STRING_A095))entry=95;
		if(gview.keypress(INTERFACEKEY_STRING_A096))entry=96;
		if(gview.keypress(INTERFACEKEY_STRING_A097))entry=97;
		if(gview.keypress(INTERFACEKEY_STRING_A098))entry=98;
		if(gview.keypress(INTERFACEKEY_STRING_A099))entry=99;
		if(gview.keypress(INTERFACEKEY_STRING_A100))entry=100;
		if(gview.keypress(INTERFACEKEY_STRING_A101))entry=101;
		if(gview.keypress(INTERFACEKEY_STRING_A102))entry=102;
		if(gview.keypress(INTERFACEKEY_STRING_A103))entry=103;
		if(gview.keypress(INTERFACEKEY_STRING_A104))entry=104;
		if(gview.keypress(INTERFACEKEY_STRING_A105))entry=105;
		if(gview.keypress(INTERFACEKEY_STRING_A106))entry=106;
		if(gview.keypress(INTERFACEKEY_STRING_A107))entry=107;
		if(gview.keypress(INTERFACEKEY_STRING_A108))entry=108;
		if(gview.keypress(INTERFACEKEY_STRING_A109))entry=109;
		if(gview.keypress(INTERFACEKEY_STRING_A110))entry=110;
		if(gview.keypress(INTERFACEKEY_STRING_A111))entry=111;
		if(gview.keypress(INTERFACEKEY_STRING_A112))entry=112;
		if(gview.keypress(INTERFACEKEY_STRING_A113))entry=113;
		if(gview.keypress(INTERFACEKEY_STRING_A114))entry=114;
		if(gview.keypress(INTERFACEKEY_STRING_A115))entry=115;
		if(gview.keypress(INTERFACEKEY_STRING_A116))entry=116;
		if(gview.keypress(INTERFACEKEY_STRING_A117))entry=117;
		if(gview.keypress(INTERFACEKEY_STRING_A118))entry=118;
		if(gview.keypress(INTERFACEKEY_STRING_A119))entry=119;
		if(gview.keypress(INTERFACEKEY_STRING_A120))entry=120;
		if(gview.keypress(INTERFACEKEY_STRING_A121))entry=121;
		if(gview.keypress(INTERFACEKEY_STRING_A122))entry=122;
		if(gview.keypress(INTERFACEKEY_STRING_A123))entry=123;
		if(gview.keypress(INTERFACEKEY_STRING_A124))entry=124;
		if(gview.keypress(INTERFACEKEY_STRING_A125))entry=125;
		if(gview.keypress(INTERFACEKEY_STRING_A126))entry=126;
		if(gview.keypress(INTERFACEKEY_STRING_A127))entry=127;
		if(gview.keypress(INTERFACEKEY_STRING_A128))entry=128;
		if(gview.keypress(INTERFACEKEY_STRING_A129))entry=129;
		if(gview.keypress(INTERFACEKEY_STRING_A130))entry=130;
		if(gview.keypress(INTERFACEKEY_STRING_A131))entry=131;
		if(gview.keypress(INTERFACEKEY_STRING_A132))entry=132;
		if(gview.keypress(INTERFACEKEY_STRING_A133))entry=133;
		if(gview.keypress(INTERFACEKEY_STRING_A134))entry=134;
		if(gview.keypress(INTERFACEKEY_STRING_A135))entry=135;
		if(gview.keypress(INTERFACEKEY_STRING_A136))entry=136;
		if(gview.keypress(INTERFACEKEY_STRING_A137))entry=137;
		if(gview.keypress(INTERFACEKEY_STRING_A138))entry=138;
		if(gview.keypress(INTERFACEKEY_STRING_A139))entry=139;
		if(gview.keypress(INTERFACEKEY_STRING_A140))entry=140;
		if(gview.keypress(INTERFACEKEY_STRING_A141))entry=141;
		if(gview.keypress(INTERFACEKEY_STRING_A142))entry=142;
		if(gview.keypress(INTERFACEKEY_STRING_A143))entry=143;
		if(gview.keypress(INTERFACEKEY_STRING_A144))entry=144;
		if(gview.keypress(INTERFACEKEY_STRING_A145))entry=145;
		if(gview.keypress(INTERFACEKEY_STRING_A146))entry=146;
		if(gview.keypress(INTERFACEKEY_STRING_A147))entry=147;
		if(gview.keypress(INTERFACEKEY_STRING_A148))entry=148;
		if(gview.keypress(INTERFACEKEY_STRING_A149))entry=149;
		if(gview.keypress(INTERFACEKEY_STRING_A150))entry=150;
		if(gview.keypress(INTERFACEKEY_STRING_A151))entry=151;
		if(gview.keypress(INTERFACEKEY_STRING_A152))entry=152;
		if(gview.keypress(INTERFACEKEY_STRING_A153))entry=153;
		if(gview.keypress(INTERFACEKEY_STRING_A154))entry=154;
		if(gview.keypress(INTERFACEKEY_STRING_A155))entry=155;
		if(gview.keypress(INTERFACEKEY_STRING_A156))entry=156;
		if(gview.keypress(INTERFACEKEY_STRING_A157))entry=157;
		if(gview.keypress(INTERFACEKEY_STRING_A158))entry=158;
		if(gview.keypress(INTERFACEKEY_STRING_A159))entry=159;
		if(gview.keypress(INTERFACEKEY_STRING_A160))entry=160;
		if(gview.keypress(INTERFACEKEY_STRING_A161))entry=161;
		if(gview.keypress(INTERFACEKEY_STRING_A162))entry=162;
		if(gview.keypress(INTERFACEKEY_STRING_A163))entry=163;
		if(gview.keypress(INTERFACEKEY_STRING_A164))entry=164;
		if(gview.keypress(INTERFACEKEY_STRING_A165))entry=165;
		if(gview.keypress(INTERFACEKEY_STRING_A166))entry=166;
		if(gview.keypress(INTERFACEKEY_STRING_A167))entry=167;
		if(gview.keypress(INTERFACEKEY_STRING_A168))entry=168;
		if(gview.keypress(INTERFACEKEY_STRING_A169))entry=169;
		if(gview.keypress(INTERFACEKEY_STRING_A170))entry=170;
		if(gview.keypress(INTERFACEKEY_STRING_A171))entry=171;
		if(gview.keypress(INTERFACEKEY_STRING_A172))entry=172;
		if(gview.keypress(INTERFACEKEY_STRING_A173))entry=173;
		if(gview.keypress(INTERFACEKEY_STRING_A174))entry=174;
		if(gview.keypress(INTERFACEKEY_STRING_A175))entry=175;
		if(gview.keypress(INTERFACEKEY_STRING_A176))entry=176;
		if(gview.keypress(INTERFACEKEY_STRING_A177))entry=177;
		if(gview.keypress(INTERFACEKEY_STRING_A178))entry=178;
		if(gview.keypress(INTERFACEKEY_STRING_A179))entry=179;
		if(gview.keypress(INTERFACEKEY_STRING_A180))entry=180;
		if(gview.keypress(INTERFACEKEY_STRING_A181))entry=181;
		if(gview.keypress(INTERFACEKEY_STRING_A182))entry=182;
		if(gview.keypress(INTERFACEKEY_STRING_A183))entry=183;
		if(gview.keypress(INTERFACEKEY_STRING_A184))entry=184;
		if(gview.keypress(INTERFACEKEY_STRING_A185))entry=185;
		if(gview.keypress(INTERFACEKEY_STRING_A186))entry=186;
		if(gview.keypress(INTERFACEKEY_STRING_A187))entry=187;
		if(gview.keypress(INTERFACEKEY_STRING_A188))entry=188;
		if(gview.keypress(INTERFACEKEY_STRING_A189))entry=189;
		if(gview.keypress(INTERFACEKEY_STRING_A190))entry=190;
		if(gview.keypress(INTERFACEKEY_STRING_A191))entry=191;
		if(gview.keypress(INTERFACEKEY_STRING_A192))entry=192;
		if(gview.keypress(INTERFACEKEY_STRING_A193))entry=193;
		if(gview.keypress(INTERFACEKEY_STRING_A194))entry=194;
		if(gview.keypress(INTERFACEKEY_STRING_A195))entry=195;
		if(gview.keypress(INTERFACEKEY_STRING_A196))entry=196;
		if(gview.keypress(INTERFACEKEY_STRING_A197))entry=197;
		if(gview.keypress(INTERFACEKEY_STRING_A198))entry=198;
		if(gview.keypress(INTERFACEKEY_STRING_A199))entry=199;
		if(gview.keypress(INTERFACEKEY_STRING_A200))entry=200;
		if(gview.keypress(INTERFACEKEY_STRING_A201))entry=201;
		if(gview.keypress(INTERFACEKEY_STRING_A202))entry=202;
		if(gview.keypress(INTERFACEKEY_STRING_A203))entry=203;
		if(gview.keypress(INTERFACEKEY_STRING_A204))entry=204;
		if(gview.keypress(INTERFACEKEY_STRING_A205))entry=205;
		if(gview.keypress(INTERFACEKEY_STRING_A206))entry=206;
		if(gview.keypress(INTERFACEKEY_STRING_A207))entry=207;
		if(gview.keypress(INTERFACEKEY_STRING_A208))entry=208;
		if(gview.keypress(INTERFACEKEY_STRING_A209))entry=209;
		if(gview.keypress(INTERFACEKEY_STRING_A210))entry=210;
		if(gview.keypress(INTERFACEKEY_STRING_A211))entry=211;
		if(gview.keypress(INTERFACEKEY_STRING_A212))entry=212;
		if(gview.keypress(INTERFACEKEY_STRING_A213))entry=213;
		if(gview.keypress(INTERFACEKEY_STRING_A214))entry=214;
		if(gview.keypress(INTERFACEKEY_STRING_A215))entry=215;
		if(gview.keypress(INTERFACEKEY_STRING_A216))entry=216;
		if(gview.keypress(INTERFACEKEY_STRING_A217))entry=217;
		if(gview.keypress(INTERFACEKEY_STRING_A218))entry=218;
		if(gview.keypress(INTERFACEKEY_STRING_A219))entry=219;
		if(gview.keypress(INTERFACEKEY_STRING_A220))entry=220;
		if(gview.keypress(INTERFACEKEY_STRING_A221))entry=221;
		if(gview.keypress(INTERFACEKEY_STRING_A222))entry=222;
		if(gview.keypress(INTERFACEKEY_STRING_A223))entry=223;
		if(gview.keypress(INTERFACEKEY_STRING_A224))entry=224;
		if(gview.keypress(INTERFACEKEY_STRING_A225))entry=225;
		if(gview.keypress(INTERFACEKEY_STRING_A226))entry=226;
		if(gview.keypress(INTERFACEKEY_STRING_A227))entry=227;
		if(gview.keypress(INTERFACEKEY_STRING_A228))entry=228;
		if(gview.keypress(INTERFACEKEY_STRING_A229))entry=229;
		if(gview.keypress(INTERFACEKEY_STRING_A230))entry=230;
		if(gview.keypress(INTERFACEKEY_STRING_A231))entry=231;
		if(gview.keypress(INTERFACEKEY_STRING_A232))entry=232;
		if(gview.keypress(INTERFACEKEY_STRING_A233))entry=233;
		if(gview.keypress(INTERFACEKEY_STRING_A234))entry=234;
		if(gview.keypress(INTERFACEKEY_STRING_A235))entry=235;
		if(gview.keypress(INTERFACEKEY_STRING_A236))entry=236;
		if(gview.keypress(INTERFACEKEY_STRING_A237))entry=237;
		if(gview.keypress(INTERFACEKEY_STRING_A238))entry=238;
		if(gview.keypress(INTERFACEKEY_STRING_A239))entry=239;
		if(gview.keypress(INTERFACEKEY_STRING_A240))entry=240;
		if(gview.keypress(INTERFACEKEY_STRING_A241))entry=241;
		if(gview.keypress(INTERFACEKEY_STRING_A242))entry=242;
		if(gview.keypress(INTERFACEKEY_STRING_A243))entry=243;
		if(gview.keypress(INTERFACEKEY_STRING_A244))entry=244;
		if(gview.keypress(INTERFACEKEY_STRING_A245))entry=245;
		if(gview.keypress(INTERFACEKEY_STRING_A246))entry=246;
		if(gview.keypress(INTERFACEKEY_STRING_A247))entry=247;
		if(gview.keypress(INTERFACEKEY_STRING_A248))entry=248;
		if(gview.keypress(INTERFACEKEY_STRING_A249))entry=249;
		if(gview.keypress(INTERFACEKEY_STRING_A250))entry=250;
		if(gview.keypress(INTERFACEKEY_STRING_A251))entry=251;
		if(gview.keypress(INTERFACEKEY_STRING_A252))entry=252;
		if(gview.keypress(INTERFACEKEY_STRING_A253))entry=253;
		if(gview.keypress(INTERFACEKEY_STRING_A254))entry=254;
		if(gview.keypress(INTERFACEKEY_STRING_A255))entry=255;
		}

	if(entry!=255)
		{
		if(entry=='\x0')
			{
			if(str.length()>0)str.resize(str.length()-1);
			}
		else
			{
			int cursor=str.length();
			if(cursor>=maxlen)cursor=maxlen-1;
			if(cursor<0)cursor=0;

			if(str.length()<cursor+1)str.resize(cursor+1);

			if(entry>='a'&&entry<='z'&&(flag & STRINGENTRY_CAPS))str[cursor]=entry+'A'-'a';
			else str[cursor]=entry;
			}

		gview.c=0;

		return 1;
		}

	return 0;
}

char standardscrolling(short &selection,long min,long max,long jump,unsigned long flag)
{
	short osel=selection;

	long seltemp=selection;
	standardscrolling(seltemp,min,max,jump,flag);
	selection=seltemp;

	if(osel!=selection)return 1;
	else return 0;
}

void finishscrolling(long &selection,long min,long max,long jump,unsigned long flag,char littlekey)
{
	if(flag & SCROLLING_NOSELECT)
		{
		if(selection>max-jump+1)selection=max-jump+1;
		if(selection<min)selection=min;
		}
	else
		{
		//WRAP ONLY IF YOU USED LITTLE KEYS OR IF YOU WERE AT THE END
		if((littlekey||selection==min-jump||selection==max+jump)&&!(flag & SCROLLING_NO_WRAP))
			{
			if(selection>max)selection=min;
			if(selection<min)selection=max;
			}
		//OTHERWISE JUST CLUNK AT THE BOTTOM
		else
			{
			if(selection>max)selection=max;
			if(selection<min)selection=min;
			}
		}
}

char standardscrolling(long &selection,long min,long max,long jump,unsigned long flag)
{
	short osel=selection;

	//NOTE: THIS CAN HANDLE max = -1, min = 0
	char littlekey=0;
	if(flag & SCROLLING_REVERSE)
		{
		if(gview.keypress(INTERFACEKEY_STANDARDSCROLL_DOWN)){selection--;littlekey=1;}
		if(gview.keypress(INTERFACEKEY_STANDARDSCROLL_UP)){selection++;littlekey=1;}
		if(gview.keypress(INTERFACEKEY_STANDARDSCROLL_PAGEDOWN))selection-=jump;
		if(gview.keypress(INTERFACEKEY_STANDARDSCROLL_PAGEUP))selection+=jump;
		}
	else
		{
		if(gview.keypress(INTERFACEKEY_STANDARDSCROLL_UP)){selection--;littlekey=1;}
		if(gview.keypress(INTERFACEKEY_STANDARDSCROLL_DOWN)){selection++;littlekey=1;}
		if(gview.keypress(INTERFACEKEY_STANDARDSCROLL_PAGEUP))selection-=jump;
		if(gview.keypress(INTERFACEKEY_STANDARDSCROLL_PAGEDOWN))selection+=jump;
		}

	finishscrolling(selection,min,max,jump,flag,littlekey);

	if(osel!=selection)return 1;
	else return 0;
}

char secondaryscrolling(short &selection,long min,long max,long jump,unsigned long flag)
{
	long temp=selection;
	char ret=secondaryscrolling(temp,min,max,jump,flag);
	selection=temp;
	return ret;
}

char secondaryscrolling(long &selection,long min,long max,long jump,unsigned long flag)
{
	short osel=selection;

	//NOTE: THIS CAN HANDLE max = -1, min = 0
	char littlekey=0;
	if(flag & SCROLLING_REVERSE)
		{
		if(gview.keypress(INTERFACEKEY_SECONDSCROLL_DOWN)){selection--;littlekey=1;}
		if(gview.keypress(INTERFACEKEY_SECONDSCROLL_UP)){selection++;littlekey=1;}
		if(gview.keypress(INTERFACEKEY_SECONDSCROLL_PAGEDOWN))selection-=jump;
		if(gview.keypress(INTERFACEKEY_SECONDSCROLL_PAGEUP))selection+=jump;
		}
	else
		{
		if(gview.keypress(INTERFACEKEY_SECONDSCROLL_UP)){selection--;littlekey=1;}
		if(gview.keypress(INTERFACEKEY_SECONDSCROLL_DOWN)){selection++;littlekey=1;}
		if(gview.keypress(INTERFACEKEY_SECONDSCROLL_PAGEUP))selection-=jump;
		if(gview.keypress(INTERFACEKEY_SECONDSCROLL_PAGEDOWN))selection+=jump;
		}

	finishscrolling(selection,min,max,jump,flag,littlekey);

	if(osel!=selection)return 1;
	else return 0;
}