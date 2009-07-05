#include "platform.h"
#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <map>
#include <set>

extern "C" {
#include <zlib.h>
#include <assert.h>
#ifndef WIN32
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/time.h>
# include <signal.h>
#endif
}
#include "svector.h"
#include "random.h"

using std::string;

#include "basics.h"

#include "keybindings.h"

#include "enabler.h"
extern enablerst enabler;
#include "g_basics.h"
#include "graphics.h"
extern graphicst gps;

#include "music_and_sound_g.h"
#include "interface.h"
extern interfacest gview;
#include "init.h"
extern initst init;

extern GameMode gamemode;
extern musicsoundst musicsound;

//some libraries do not define this function in string.h
char* stpcpy(char* to, const char* from) {
 while (*to=*from) {
  to++;
  from++;
 }
 return to;
}

typedef struct {
 int ofset;
 int count;
 char* name;
} BindingCategories;
#define BINDCATSNUM 12
const BindingCategories BindCats[BINDCATSNUM]={
 {1,WORLDKEY_START-1,"System"},
 {INTERFACEKEY_STRING_A000,INTERFACEKEYNUM-INTERFACEKEY_STRING_A000,"Text Entry"},
 {WORLDKEY_START,ADVENTURERKEY_START-WORLDKEY_START,"World"},
 {ADVENTURERKEY_START,EMBARKKEY_START-ADVENTURERKEY_START,"Adventurer"},
 {EMBARKKEY_START,BUILDINGKEY_START-EMBARKKEY_START,"Embark"},
 {DWARFMAINKEY_START,MILITIAKEY_START-DWARFMAINKEY_START,"Dwarf Main"},
 {BUILDINGKEY_START,WORKSHOPKEY_START-BUILDINGKEY_START,"Building"},
 {WORKSHOPKEY_START,PILEZONEKEY_START-WORKSHOPKEY_START,"Workshop"},
 {PILEZONEKEY_START,STOCKORDERKEY_START-PILEZONEKEY_START,"Pile/Zone"},
 {STOCKORDERKEY_START,DWARFMAINKEY_START-STOCKORDERKEY_START,"Stock/Order"},
 {MILITIAKEY_START,INTERFACEKEY_STRING_A000-MILITIAKEY_START,"Militia"},
 {INTERFACEKEYNUM,0,"Macros"}
};

typedef struct {
 char* token;
 char* name;
} BindingNames;
const BindingNames InterfaceNames[INTERFACEKEYNUM]={
{"",""},
{"SELECT","Select"},
{"DESELECT","Deselect"},
{"SELECT_ALL","Select all"},
{"DESELECT_ALL","Deselect All"},
{"LEAVESCREEN","Leave screen"},
{"LEAVESCREEN_TEXT","Leave text entry screen"},
{"LEAVESCREEN_ALL","Leave all screens"},
{"CLOSE_MEGA_ANNOUNCEMENT","Close mega announcement"},
{"OPTIONS","Main menu"},
{"OPTION_EXPORT","Options, Export Local Image"},
{"HELP","Help"},
{"TOGGLE_FULLSCREEN","Toggle Fullscreen"},
{"MOVIES","Movies"},
{"MOVIE_RECORD","Movie: Record"},
{"MOVIE_PLAY","Movie: Play"},
{"MOVIE_SAVE","Movie: Save"},
{"MOVIE_LOAD","Movie: Load"},
{"CHANGETAB","Change tab or highlight selection"},
{"REVERSETAB","Back tab or highlight selection"},
{"STANDARDSCROLL_UP","Move selector up"},
{"STANDARDSCROLL_DOWN","Move selector down"},
{"STANDARDSCROLL_LEFT","Move selector left"},
{"STANDARDSCROLL_RIGHT","Move selector right"},
{"STANDARDSCROLL_PAGEUP","Page selector up"},
{"STANDARDSCROLL_PAGEDOWN","Page selector down"},
{"SECONDSCROLL_UP","Move secondary selector up"},
{"SECONDSCROLL_DOWN","Move secondary selector down"},
{"SECONDSCROLL_PAGEUP","Page secondary selector up"},
{"SECONDSCROLL_PAGEDOWN","Page secondary selector down"},
{"CURSOR_UP","Move view/cursor up"},
{"CURSOR_DOWN","Move view/cursor down"},
{"CURSOR_LEFT","Move view/cursor left"},
{"CURSOR_RIGHT","Move view/cursor right"},
{"CURSOR_UPLEFT","Move view/cursor up and left"},
{"CURSOR_UPRIGHT","Move view/cursor up and right"},
{"CURSOR_DOWNLEFT","Move view/cursor down and left"},
{"CURSOR_DOWNRIGHT","Move view/cursor down and right"},
{"CURSOR_UP_FAST","Move view/cursor up, fast"},
{"CURSOR_DOWN_FAST","Move view/cursor down, fast"},
{"CURSOR_LEFT_FAST","Move view/cursor left, fast"},
{"CURSOR_RIGHT_FAST","Move view/cursor right, fast"},
{"CURSOR_UPLEFT_FAST","Move view/cursor up and left, fast"},
{"CURSOR_UPRIGHT_FAST","Move view/cursor up and right, fast"},
{"CURSOR_DOWNLEFT_FAST","Move view/cursor down and left, fast"},
{"CURSOR_DOWNRIGHT_FAST","Move view/cursor down and right, fast"},
{"CURSOR_UP_Z","Move view/cursor up (z)"},
{"CURSOR_DOWN_Z","Move view/cursor down (z)"},
{"CURSOR_UP_Z_AUX","Move view/cursor up (z), aux"},
{"CURSOR_DOWN_Z_AUX","Move view/cursor down (z), aux"},
{"MENU_CONFIRM","Menu Confirm"},
{"SAVE_BINDINGS","Save Bindings"},
{"LOAD_BINDINGS","Load Bindings"},
{"MACRO_BREAK","Break Macro Loop"},

//WORLDKEY_START
{"WORLD_PARAM_TITLE","World Param: Title"},
{"WORLD_PARAM_ADD","World Param: Add"},
{"WORLD_PARAM_COPY","World Param: Copy"},
{"WORLD_PARAM_DELETE","World Param: Delete"},
{"WORLD_PARAM_NAME_RANDOM","World Param: Name, Random"},
{"WORLD_PARAM_NAME_ENTER","World Param: Name, Enter"},
{"WORLD_PARAM_SEED_RANDOM","World Param: Seed, Random"},
{"WORLD_PARAM_SEED_ENTER","World Param: Seed, Enter"},
{"WORLD_PARAM_LOAD","World Param: Load"},
{"WORLD_PARAM_SAVE","World Param: Save"},
{"WORLD_PARAM_DIM_X_UP","World Param: Dim X, Up"},
{"WORLD_PARAM_DIM_X_DOWN","World Param: Dim X, Down"},
{"WORLD_PARAM_DIM_Y_UP","World Param: Dim Y, Up"},
{"WORLD_PARAM_DIM_Y_DOWN","World Param: Dim Y, Down"},
{"WORLD_PARAM_SET","World Param: Enter Advanced Parameters"},
{"WORLD_PARAM_INCREASE","World Param: Increase Parameter"},
{"WORLD_PARAM_DECREASE","World Param: Decrease Parameter"},
{"WORLD_PARAM_ENTER_VALUE","World Param: Enter Value"},
{"WORLD_PARAM_NULLIFY","World Param: Nullify Parameter"},
{"WORLD_PARAM_PRESET","World Param: Set Presets"},
{"WORLD_PARAM_REJECT_CONTINUE","World Param: Reject, Continue"},
{"WORLD_PARAM_REJECT_ABORT","World Param: Reject, Abort"},
{"WORLD_PARAM_REJECT_ALLOW_THIS","World Param: Reject, Allow This"},
{"WORLD_PARAM_REJECT_ALLOW_ALL","World Param: Reject, Allow All"},
{"WORLD_GEN_CONTINUE","World Generation: Continue"},
{"WORLD_GEN_USE","World Generation: Use"},
{"WORLD_GEN_ABORT","World Generation: Abort"},
{"WORLDGEN_EXPORT_MAP","World Gen: Export Map"},
{"LEGENDS_EXPORT_MAP","Legends: Export Map"},
{"LEGENDS_EXPORT_DETAILED_MAP","Legends: Export Detailed Map"},
{"LEGENDS_TOGGLE_CIVSITE","Legends: Civ/Site"},
{"LEGENDS_STRING_FILTER","Legends: String filter"},

//ADVENTURERKEY_START
{"A_MOVE_N","Adventure: move north"},
{"A_MOVE_S","Adventure: move south"},
{"A_MOVE_E","Adventure: move east"},
{"A_MOVE_W","Adventure: move west"},
{"A_MOVE_NW","Adventure: move northwest"},
{"A_MOVE_NE","Adventure: move northeast"},
{"A_MOVE_SW","Adventure: move southwest"},
{"A_MOVE_SE","Adventure: move southeast"},
{"A_MOVE_WAIT","Adventure: move wait"},
{"A_CARE_MOVE_N","Adventure: careful move north up/down"},
{"A_CARE_MOVE_S","Adventure: careful move south up/down"},
{"A_CARE_MOVE_E","Adventure: careful move east up/down"},
{"A_CARE_MOVE_W","Adventure: careful move west up/down"},
{"A_CARE_MOVE_NW","Adventure: careful move northwest up/down"},
{"A_CARE_MOVE_NE","Adventure: careful move northeast up/down"},
{"A_CARE_MOVE_SW","Adventure: careful move southwest up/down"},
{"A_CARE_MOVE_SE","Adventure: careful move southeast up/down"},
{"A_CARE_MOVE_UPDOWN","Adventure: careful move up/down"},
{"A_MOVE_N_UP","Adventure: move north/up"},
{"A_MOVE_S_UP","Adventure: move south/up"},
{"A_MOVE_E_UP","Adventure: move east/up"},
{"A_MOVE_W_UP","Adventure: move west/up"},
{"A_MOVE_NW_UP","Adventure: move northwest/up"},
{"A_MOVE_NE_UP","Adventure: move northeast/up"},
{"A_MOVE_SW_UP","Adventure: move southwest/up"},
{"A_MOVE_SE_UP","Adventure: move southeast/up"},
{"A_MOVE_UP","Adventure: move up"},
{"A_MOVE_N_DOWN","Adventure: move north/down"},
{"A_MOVE_S_DOWN","Adventure: move south/down"},
{"A_MOVE_E_DOWN","Adventure: move east/down"},
{"A_MOVE_W_DOWN","Adventure: move west/down"},
{"A_MOVE_NW_DOWN","Adventure: move northwest/down"},
{"A_MOVE_NE_DOWN","Adventure: move northeast/down"},
{"A_MOVE_SW_DOWN","Adventure: move southwest/down"},
{"A_MOVE_SE_DOWN","Adventure: move southeast/down"},
{"A_MOVE_DOWN","Adventure: move down"},
{"A_MOVE_UP_AUX","Adventure: move up, aux"},
{"A_MOVE_DOWN_AUX","Adventure: move down, aux"},
{"A_COMBAT_ATTACK","Adventure: Combat, Attack Mode"},
{"A_COMBAT_DODGE","Adventure: Combat, Dodge Mode"},
{"A_COMBAT_CHARGEDEF","Adventure: Combat, Charge Defend Mode"},
{"A_STATUS","Adventure: Status"},
{"A_STATUS_WRESTLE","Adventure: Status, Close Combat"},
{"A_STATUS_CUSTOMIZE","Adventure: Status, Customize"},
{"A_STATUS_KILLS","Adventure: Status, Kills"},
{"A_SLEEP","Adventure: Sleep"},
{"A_WAIT","Adventure: Wait"},
{"A_ATTACK","Adventure: Attack"},
{"A_LOOK","Adventure: Look"},
{"A_SEARCH","Adventure: Search"},
{"A_TALK","Adventure: Talk"},
{"A_INTERACT","Adventure: Inv. Interact"},
{"A_INV_LOOK","Adventure: Inv. Look"},
{"A_INV_REMOVE","Adventure: Inv. Remove"},
{"A_INV_WEAR","Adventure: Inv. Wear"},
{"A_INV_EATDRINK","Adventure: Inv. Eat/Drink"},
{"A_INV_PUTIN","Adventure: Inv. Put In"},
{"A_INV_DROP","Adventure: Inv. Drop"},
{"A_GROUND","Adventure: Get/Ground"},
{"A_THROW","Adventure: Throw"},
{"A_SHOOT","Adventure: Fire"},
{"A_ANNOUNCEMENTS","Adventure: Announcements"},
{"A_COMBAT","Adventure: Combat Options"},
{"A_MOVEMENT","Adventure: Movement Options"},
{"A_MOVEMENT_SWIM","Adventure: Movement Options, Swim"},
{"A_SNEAK","Adventure: Sneak"},
{"A_CENTER","Adventure: Center"},
{"A_BUILDING","Adventure: Building Interact"},
{"A_TRAVEL","Adventure: Travel"},
{"A_DATE","Adventure: Get Date"},
{"A_WEATHER","Adventure: Get Weather"},
{"A_TEMPERATURE","Adventure: Get Temperature"},
{"A_STANCE","Adventure: Change Stance"},
{"A_OPTION1","Adventure: Option 1"},
{"A_OPTION2","Adventure: Option 2"},
{"A_OPTION3","Adventure: Option 3"},
{"A_OPTION4","Adventure: Option 4"},
{"A_OPTION5","Adventure: Option 5"},
{"A_OPTION6","Adventure: Option 6"},
{"A_OPTION7","Adventure: Option 7"},
{"A_OPTION8","Adventure: Option 8"},
{"A_OPTION9","Adventure: Option 9"},
{"A_OPTION10","Adventure: Option 10"},
{"A_OPTION11","Adventure: Option 11"},
{"A_OPTION12","Adventure: Option 12"},
{"A_OPTION13","Adventure: Option 13"},
{"A_OPTION14","Adventure: Option 14"},
{"A_OPTION15","Adventure: Option 15"},
{"A_OPTION16","Adventure: Option 16"},
{"A_OPTION17","Adventure: Option 17"},
{"A_OPTION18","Adventure: Option 18"},
{"A_OPTION19","Adventure: Option 19"},
{"A_OPTION20","Adventure: Option 20"},
{"A_ENTER_NAME","Adventure: Creation, Name Entry"},
{"A_CUST_NAME","Adventure: Creation, Customize Name"},
{"A_RANDOM_NAME","Adventure: Creation, Random Name"},
{"A_CHANGE_GENDER","Adventure: Creation, Change Gender"},
{"A_END_TRAVEL","Adventure: Travel, Visit Site"},
{"A_TRAVEL_CLOUDS","Adventure: Travel, Clouds"},
{"A_LOG","Adventure: Log"},
{"A_TRAVEL_LOG","Adventure: Travel, Log"},
{"A_LOG_TASKS","Adventure: Log, Tasks"},
{"A_LOG_ENTITIES","Adventure: Log, Entities"},
{"A_LOG_SITES","Adventure: Log, Sites"},
{"A_LOG_SUBREGIONS","Adventure: Log, Regions"},
{"A_LOG_ZOOM_CURRENT_LOCATION","Adventure: Log, Zoom to Current Location"},
{"A_LOG_ZOOM_SELECTED","Adventure: Log, Zoom to Selected"},
{"A_LOG_LINE","Adventure: Log, Toggle Line"},
{"A_LOG_MAP","Adventure: Log, Toggle Map/Info"},
{"A_BARTER_VIEW","Adventure: Barter, View"},
{"A_BARTER_CURRENCY_1","Adventure: Barter, Currency 1"},
{"A_BARTER_CURRENCY_2","Adventure: Barter, Currency 2"},
{"A_BARTER_CURRENCY_3","Adventure: Barter, Currency 3"},
{"A_BARTER_CURRENCY_4","Adventure: Barter, Currency 4"},
{"A_BARTER_CURRENCY_5","Adventure: Barter, Currency 5"},
{"A_BARTER_CURRENCY_6","Adventure: Barter, Currency 6"},
{"A_BARTER_CURRENCY_7","Adventure: Barter, Currency 7"},
{"A_BARTER_CURRENCY_8","Adventure: Barter, Currency 8"},
{"A_BARTER_CURRENCY_9","Adventure: Barter, Currency 9"},
{"A_BARTER_TRADE","Adventure: Barter, Trade"},

//EMBARKKEY_START
{"SETUP_EMBARK","Setup game: Embark"},
{"SETUP_NAME_FORT","Setup game: Name Fort"},
{"SETUP_NAME_GROUP","Setup game: Name Group"},
{"SETUP_RECLAIM","Setup game: Reclaim"},
{"SETUP_FIND","Setup game: Find"},
{"SETUP_NOTES","Setup game: Notes"},
{"SETUP_NOTES_TAKE_NOTES","Setup game: Notes, New Note"},
{"SETUP_NOTES_DELETE_NOTE","Setup game: Notes, Delete Note"},
{"SETUP_NOTES_CHANGE_SYMBOL_SELECTION","Setup game: Notes, Change Symbol Selection"},
{"SETUP_NOTES_ADOPT_SYMBOL","Setup game: Notes, Adopt Symbol"},
{"SETUP_LOCAL_Y_UP","Setup game: Resize Local Y Up"},
{"SETUP_LOCAL_Y_DOWN","Setup game: Resize Local Y Down"},
{"SETUP_LOCAL_X_UP","Setup game: Resize Local X Up"},
{"SETUP_LOCAL_X_DOWN","Setup game: Resize Local X Down"},
{"SETUP_LOCAL_Y_MUP","Setup game: Move Local Y Up"},
{"SETUP_LOCAL_Y_MDOWN","Setup game: Move Local Y Down"},
{"SETUP_LOCAL_X_MUP","Setup game: Move Local X Up"},
{"SETUP_LOCAL_X_MDOWN","Setup game: Move Local X Down"},
{"SETUP_BIOME_1","Setup game: View Biome 1"},
{"SETUP_BIOME_2","Setup game: View Biome 2"},
{"SETUP_BIOME_3","Setup game: View Biome 3"},
{"SETUP_BIOME_4","Setup game: View Biome 4"},
{"SETUP_BIOME_5","Setup game: View Biome 5"},
{"SETUP_BIOME_6","Setup game: View Biome 6"},
{"SETUP_BIOME_7","Setup game: View Biome 7"},
{"SETUP_BIOME_8","Setup game: View Biome 8"},
{"SETUP_BIOME_9","Setup game: View Biome 9"},
{"CHOOSE_NAME_RANDOM","Choose name: Random"},
{"CHOOSE_NAME_CLEAR","Choose name: Clear"},
{"CHOOSE_NAME_TYPE","Choose name: Type"},
{"SETUPGAME_NEW","Setup, New"},
{"SETUPGAME_VIEW","Setup, View"},
{"SETUPGAME_CUSTOMIZE_UNIT","Setup, Customize Unit"},
{"SETUPGAME_SAVE_PROFILE","Setup, Save Profile"},
{"SETUPGAME_SAVE_PROFILE_ABORT","Setup, Save Profile, Abort"},
{"SETUPGAME_SAVE_PROFILE_GO","Setup, Save Profile, Go"},
{"SETUPGAME_VIEW_PROFILE_PROBLEMS","Setup, View Profile Problems"},

//BUILDINGKEY_START
{"BUILDJOB_ADD","Buildjob: Add"},
{"BUILDJOB_CANCEL","Buildjob: Cancel"},
{"BUILDJOB_PROMOTE","Buildjob: Promote"},
{"BUILDJOB_REPEAT","Buildjob: Repeat"},
{"BUILDJOB_SUSPEND","Buildjob: Suspend"},
{"BUILDJOB_WORKSHOP_PROFILE","Buildjob: Workshop Profile"},
{"BUILDJOB_WELL_FREE","Buildjob: Well, Free"},
{"BUILDJOB_WELL_SIZE","Buildjob: Well, Size"},
{"BUILDJOB_TARGET_FREE","Buildjob: Target, Free"},
{"BUILDJOB_TARGET_SIZE","Buildjob: Target, Size"},
{"BUILDJOB_TARGET_DOWN","Buildjob: Target, Down"},
{"BUILDJOB_TARGET_UP","Buildjob: Target, Up"},
{"BUILDJOB_TARGET_RIGHT","Buildjob: Target, Right"},
{"BUILDJOB_TARGET_LEFT","Buildjob: Target, Left"},
{"BUILDJOB_STATUE_ASSIGN","Buildjob: Statue, Assign"},
{"BUILDJOB_STATUE_FREE","Buildjob: Statue, Free"},
{"BUILDJOB_STATUE_SIZE","Buildjob: Statue, Size"},
{"BUILDJOB_CAGE_JUSTICE","Buildjob: Cage, Justice"},
{"BUILDJOB_CAGE_FREE","Buildjob: Cage, Free"},
{"BUILDJOB_CAGE_SIZE","Buildjob: Cage, Size"},
{"BUILDJOB_CAGE_ASSIGN_OCC","Buildjob: Cage, Assign Occ"},
{"BUILDJOB_CAGE_WATER","Buildjob: Cage, Water"},
{"BUILDJOB_CAGE_ASSIGN","Buildjob: Cage, Assign Owner"},
{"BUILDJOB_CHAIN_ASSIGN_OCC","Buildjob: Chain, Assign Occ"},
{"BUILDJOB_CHAIN_JUSTICE","Buildjob: Chain, Justice"},
{"BUILDJOB_CHAIN_ASSIGN","Buildjob: Chain, Assign Owner"},
{"BUILDJOB_CHAIN_FREE","Buildjob: Chain, Free"},
{"BUILDJOB_CHAIN_SIZE","Buildjob: Chain, Size"},
{"BUILDJOB_SIEGE_FIRING","Buildjob: Siege, Fire"},
{"BUILDJOB_SIEGE_ORIENT","Buildjob: Siege, Orient"},
{"BUILDJOB_DOOR_INTERNAL","Buildjob: Door, Internal"},
{"BUILDJOB_DOOR_LOCK","Buildjob: Door, Forbid"},
{"BUILDJOB_DOOR_AJAR","Buildjob: Door, Pet-passable"},
{"BUILDJOB_COFFIN_ASSIGN","Buildjob: Coffin, Assign"},
{"BUILDJOB_COFFIN_FREE","Buildjob: Coffin, Free"},
{"BUILDJOB_COFFIN_SIZE","Buildjob: Coffin, Size"},
{"BUILDJOB_COFFIN_BURIAL","Buildjob: Coffin, Gen. Burial"},
{"BUILDJOB_COFFIN_CIV","Buildjob: Coffing, Allow Citizens"},
{"BUILDJOB_COFFIN_PET","Buildjob: Coffing, Allow Pets"},
{"BUILDJOB_CHAIR_ASSIGN","Buildjob: Chair, Assign"},
{"BUILDJOB_CHAIR_FREE","Buildjob: Chair, Free"},
{"BUILDJOB_CHAIR_SIZE","Buildjob: Chair, Size"},
{"BUILDJOB_TABLE_ASSIGN","Buildjob: Table, Assign"},
{"BUILDJOB_TABLE_HALL","Buildjob: Table, Hall"},
{"BUILDJOB_TABLE_FREE","Buildjob: Table, Free"},
{"BUILDJOB_TABLE_SIZE","Buildjob: Table, Size"},
{"BUILDJOB_BED_ASSIGN","Buildjob: Bed, Assign"},
{"BUILDJOB_BED_FREE","Buildjob: Bed, Free"},
{"BUILDJOB_BED_BARRACKS","Buildjob: Bed, Barracks"},
{"BUILDJOB_BED_RENT","Buildjob: Bed, Rent"},
{"BUILDJOB_BED_SIZE","Buildjob: Bed, Size"},
{"BUILDJOB_DEPOT_BRING","Buildjob: Depot, Bring"},
{"BUILDJOB_DEPOT_TRADE","Buildjob: Depot, Trade"},
{"BUILDJOB_DEPOT_REQUEST_TRADER","Buildjob: Depot, Request Trader"},
{"BUILDJOB_DEPOT_BROKER_ONLY","Buildjob: Depot, Broker Only"},
{"BUILDJOB_ANIMALTRAP_BAIT_NONE","Buildjob: Animal Trap, No Bait"},
{"BUILDJOB_ANIMALTRAP_BAIT_MEAT","Buildjob: Animal Trap, Meat Bait"},
{"BUILDJOB_ANIMALTRAP_BAIT_FISH","Buildjob: Animal Trap, Fish Bait"},
{"BUILDJOB_ANIMALTRAP_BAIT_GEM","Buildjob: Animal Trap, Gem Bait"},
{"BUILDJOB_FARM_FALLOW","Buildjob: Farm, Fallow"},
{"BUILDJOB_FARM_FERTILIZE","Buildjob: Farm, Fertilize"},
{"BUILDJOB_FARM_SEASFERT","Buildjob: Farm, Seas Fert"},
{"BUILDJOB_FARM_SPRING","Buildjob: Farm, Spring"},
{"BUILDJOB_FARM_SUMMER","Buildjob: Farm, Summer"},
{"BUILDJOB_FARM_AUTUMN","Buildjob: Farm, Autumn"},
{"BUILDJOB_FARM_WINTER","Buildjob: Farm, Winter"},
{"BUILDJOB_RACK_MAT_BONE","Buildjob: Rack, Mat, Bone"},
{"BUILDJOB_RACK_MAT_BRONZE","Buildjob: Rack, Mat, Bronze"},
{"BUILDJOB_RACK_MAT_COPPER","Buildjob: Rack, Mat, Copper"},
{"BUILDJOB_RACK_MAT_IRON","Buildjob: Rack, Mat, Iron"},
{"BUILDJOB_RACK_MAT_STEEL","Buildjob: Rack, Mat, Steel"},
{"BUILDJOB_RACK_MAT_WOOD","Buildjob: Rack, Mat, Wood"},
{"BUILDJOB_RACK_MAT_METAL","Buildjob: Rack, Mat, Special Metal"},
{"BUILDJOB_STAND_MAT_BONE","Buildjob: Stand, Mat, Bone"},
{"BUILDJOB_STAND_MAT_LEATHER","Buildjob: Stand, Mat, Leather"},
{"BUILDJOB_STAND_MAT_COPPER","Buildjob: Stand, Mat, Copper"},
{"BUILDJOB_STAND_MAT_BRONZE","Buildjob: Stand, Mat, Bronze"},
{"BUILDJOB_STAND_MAT_STEEL","Buildjob: Stand, Mat, Steel"},
{"BUILDJOB_STAND_MAT_IRON","Buildjob: Stand, Mat, Iron"},
{"BUILDJOB_STAND_MAT_WOOD","Buildjob: Stand, Mat, Wood"},
{"BUILDJOB_STAND_MAT_METAL","Buildjob: Stand, Mat, Special Metal"},
{"BUILDJOB_RACKSTAND_ASSIGN","Buildjob: Rackstand, Assign"},
{"BUILDJOB_RACKSTAND_FREE","Buildjob: Rackstand, Free"},
{"BUILDJOB_RACKSTAND_SIZE","Buildjob: Rackstand, Size"},
{"BUILDJOB_RACKSTAND_ITEM","Buildjob: Rackstand, Item"},
{"BUILDJOB_RACKSTAND_MAT","Buildjob: Rackstand, Mat"},
{"BUILDJOB_RACKSTAND_DEFAULTS1","Buildjob: Rackstand, All Items"},
{"BUILDJOB_RACKSTAND_KILL1","Buildjob: Rackstand, No Items"},
{"BUILDJOB_RACKSTAND_DEFAULTS2","Buildjob: Rackstand, All Mats"},
{"BUILDJOB_RACKSTAND_KILL2","Buildjob: Rackstand, No Mats"},
{"HOTKEY_BUILDING_ARMORSTAND","Hotkey: Building, Armorstand"},
{"HOTKEY_BUILDING_BED","Hotkey: Building, Bed"},
{"HOTKEY_BUILDING_CHAIR","Hotkey: Building, Chair"},
{"HOTKEY_BUILDING_COFFIN","Hotkey: Building, Coffin"},
{"HOTKEY_BUILDING_DOOR","Hotkey: Building, Door"},
{"HOTKEY_BUILDING_FLOODGATE","Hotkey: Building, Floodgate"},
{"HOTKEY_BUILDING_HATCH","Hotkey: Building, Hatch"},
{"HOTKEY_BUILDING_GRATE_WALL","Hotkey: Building, Wall Grate"},
{"HOTKEY_BUILDING_GRATE_FLOOR","Hotkey: Building, Floor Grate"},
{"HOTKEY_BUILDING_BARS_VERTICAL","Hotkey: Building, Vertical Bars"},
{"HOTKEY_BUILDING_BARS_FLOOR","Hotkey: Building, Floor Bars"},
{"HOTKEY_BUILDING_CABINET","Hotkey: Building, Cabinet"},
{"HOTKEY_BUILDING_BOX","Hotkey: Building, Chest"},
{"HOTKEY_BUILDING_KENNEL","Hotkey: Building, Kennel"},
{"HOTKEY_BUILDING_FARMPLOT","Hotkey: Building, Farm Plot"},
{"HOTKEY_BUILDING_WEAPONRACK","Hotkey: Building, Weaponrack"},
{"HOTKEY_BUILDING_STATUE","Hotkey: Building, Statue"},
{"HOTKEY_BUILDING_TABLE","Hotkey: Building, Table"},
{"HOTKEY_BUILDING_ROAD_DIRT","Hotkey: Building, Dirt Road"},
{"HOTKEY_BUILDING_ROAD_PAVED","Hotkey: Building, Paved Road"},
{"HOTKEY_BUILDING_BRIDGE","Hotkey: Building, Bridge"},
{"HOTKEY_BUILDING_WELL","Hotkey: Building, Well"},
{"HOTKEY_BUILDING_SIEGEENGINE","Hotkey: Building, Siege"},
{"HOTKEY_BUILDING_WORKSHOP","Hotkey: Building, Workshop"},
{"HOTKEY_BUILDING_FURNACE","Hotkey: Building, Furnace"},
{"HOTKEY_BUILDING_WINDOW_GLASS","Hotkey: Building, Glass Window"},
{"HOTKEY_BUILDING_WINDOW_GEM","Hotkey: Building, Gem Window"},
{"HOTKEY_BUILDING_SHOP","Hotkey: Building, Shop"},
{"HOTKEY_BUILDING_ANIMALTRAP","Hotkey: Building, Animal Trap"},
{"HOTKEY_BUILDING_CHAIN","Hotkey: Building, Chain"},
{"HOTKEY_BUILDING_CAGE","Hotkey: Building, Cage"},
{"HOTKEY_BUILDING_TRADEDEPOT","Hotkey: Building, Trade Depot"},
{"HOTKEY_BUILDING_TRAP","Hotkey: Building, Trap"},
{"HOTKEY_BUILDING_MACHINE","Hotkey: Building, Machine Component"},
{"HOTKEY_BUILDING_SUPPORT","Hotkey: Building, Support"},
{"HOTKEY_BUILDING_ARCHERYTARGET","Hotkey: Building, Archery Target"},
{"HOTKEY_BUILDING_MACHINE_SCREW_PUMP","Hotkey: Building, Machine Component, Screw Pump"},
{"HOTKEY_BUILDING_MACHINE_WATER_WHEEL","Hotkey: Building, Machine Component, Water Wheel"},
{"HOTKEY_BUILDING_MACHINE_WINDMILL","Hotkey: Building, Machine Component, Windmill"},
{"HOTKEY_BUILDING_MACHINE_GEAR_ASSEMBLY","Hotkey: Building, Machine Component, Gear Assembly"},
{"HOTKEY_BUILDING_MACHINE_AXLE_VERTICAL","Hotkey: Building, Machine Component, Vertical Axle"},
{"HOTKEY_BUILDING_MACHINE_AXLE_HORIZONTAL","Hotkey: Building, Machine Component, Horizontal Axle"},
{"HOTKEY_BUILDING_SIEGEENGINE_BALLISTA","Hotkey: Building, Siege, Ballista"},
{"HOTKEY_BUILDING_SIEGEENGINE_CATAPULT","Hotkey: Building, Siege, Catapult"},
{"HOTKEY_BUILDING_TRAP_STONE","Hotkey: Building, Trap, Stone"},
{"HOTKEY_BUILDING_TRAP_WEAPON","Hotkey: Building, Trap, Weapon"},
{"HOTKEY_BUILDING_TRAP_LEVER","Hotkey: Building, Trap, Lever"},
{"HOTKEY_BUILDING_TRAP_TRIGGER","Hotkey: Building, Trap, Trigger"},
{"HOTKEY_BUILDING_TRAP_CAGE","Hotkey: Building, Trap, Cage"},
{"HOTKEY_BUILDING_TRAP_SPIKE","Hotkey: Building, Trap, Upright Spear/Spike"},
{"HOTKEY_BUILDING_CONSTRUCTION","Hotkey: Building, Wall/Floor/Stairs"},
{"HOTKEY_BUILDING_CONSTRUCTION_WALL","Hotkey: Building, Construction, Wall"},
{"HOTKEY_BUILDING_CONSTRUCTION_FLOOR","Hotkey: Building, Construction, Floor"},
{"HOTKEY_BUILDING_CONSTRUCTION_RAMP","Hotkey: Building, Construction, Ramp"},
{"HOTKEY_BUILDING_CONSTRUCTION_STAIR_UP","Hotkey: Building, Construction, Upward Stair"},
{"HOTKEY_BUILDING_CONSTRUCTION_STAIR_DOWN","Hotkey: Building, Construction, Downward Stair"},
{"HOTKEY_BUILDING_CONSTRUCTION_STAIR_UPDOWN","Hotkey: Building, Construction, Up/Down Stair"},
{"HOTKEY_BUILDING_CONSTRUCTION_FORTIFICATION","Hotkey: Building, Construction, Fortification"},
{"HOTKEY_BUILDING_WORKSHOP_LEATHER","Hotkey: Building, Wksp, Leather"},
{"HOTKEY_BUILDING_WORKSHOP_QUERN","Hotkey: Building, Wksp, Quern"},
{"HOTKEY_BUILDING_WORKSHOP_MILLSTONE","Hotkey: Building, Wksp, Millstone"},
{"HOTKEY_BUILDING_WORKSHOP_LOOM","Hotkey: Building, Wksp, Loom"},
{"HOTKEY_BUILDING_WORKSHOP_CLOTHES","Hotkey: Building, Wksp, Clothes"},
{"HOTKEY_BUILDING_WORKSHOP_BOWYER","Hotkey: Building, Wksp, Bowyer"},
{"HOTKEY_BUILDING_WORKSHOP_CARPENTER","Hotkey: Building, Wksp, Carpenter"},
{"HOTKEY_BUILDING_WORKSHOP_METALSMITH","Hotkey: Building, Wksp, Metalsmith"},
{"HOTKEY_BUILDING_WORKSHOP_LAVAMILL","Hotkey: Building, Wksp, Lavamill"},
{"HOTKEY_BUILDING_WORKSHOP_JEWELER","Hotkey: Building, Wksp, Jeweler"},
{"HOTKEY_BUILDING_WORKSHOP_MASON","Hotkey: Building, Wksp, Mason"},
{"HOTKEY_BUILDING_WORKSHOP_BUTCHER","Hotkey: Building, Wksp, Butcher"},
{"HOTKEY_BUILDING_WORKSHOP_TANNER","Hotkey: Building, Wksp, Tanner"},
{"HOTKEY_BUILDING_WORKSHOP_DYER","Hotkey: Building, Wksp, Dyer"},
{"HOTKEY_BUILDING_WORKSHOP_CRAFTSMAN","Hotkey: Building, Wksp, Craftsman"},
{"HOTKEY_BUILDING_WORKSHOP_SIEGE","Hotkey: Building, Wksp, Siege"},
{"HOTKEY_BUILDING_WORKSHOP_MECHANIC","Hotkey: Building, Wksp, Mechanic"},
{"HOTKEY_BUILDING_WORKSHOP_STILL","Hotkey: Building, Wksp, Still"},
{"HOTKEY_BUILDING_WORKSHOP_FARMER","Hotkey: Building, Wksp, Farmer"},
{"HOTKEY_BUILDING_WORKSHOP_KITCHEN","Hotkey: Building, Wksp, Kitchen"},
{"HOTKEY_BUILDING_WORKSHOP_FISHERY","Hotkey: Building, Wksp, Fishery"},
{"HOTKEY_BUILDING_WORKSHOP_LAB","Hotkey: Building, Wksp, Lab"},
{"HOTKEY_BUILDING_WORKSHOP_ASHERY","Hotkey: Building, Wksp, Ashery"},
{"HOTKEY_BUILDING_FURNACE_WOOD","Hotkey: Building, Furn, Wood"},
{"HOTKEY_BUILDING_FURNACE_SMELTER","Hotkey: Building, Furn, Smelter"},
{"HOTKEY_BUILDING_FURNACE_GLASS","Hotkey: Building, Furn, Glass"},
{"HOTKEY_BUILDING_FURNACE_KILN","Hotkey: Building, Furn, Kiln"},
{"HOTKEY_BUILDING_FURNACE_SMELTER_LAVA","Hotkey: Building, Furn, Smelter"},
{"HOTKEY_BUILDING_FURNACE_GLASS_LAVA","Hotkey: Building, Furn, Glass"},
{"HOTKEY_BUILDING_FURNACE_KILN_LAVA","Hotkey: Building, Furn, Kiln"},
{"BUILDING_DIM_Y_UP","Building, Change Height +"},
{"BUILDING_DIM_Y_DOWN","Building, Change Height -"},
{"BUILDING_DIM_X_UP","Building, Change Width +"},
{"BUILDING_DIM_X_DOWN","Building, Change Width -"},
{"BUILDING_ORIENT_UP","Building, Orient Up"},
{"BUILDING_ORIENT_LEFT","Building, Orient Left"},
{"BUILDING_ORIENT_RIGHT","Building, Orient Right"},
{"BUILDING_ORIENT_DOWN","Building, Orient Down"},
{"BUILDING_ORIENT_NONE","Building, Orient None"},
{"BUILDING_VIEW_ITEM","Building, View Item"},
{"BUILDING_ADVANCE_STAGE","Building, Done Selecting"},
{"BUILDING_EXPAND_CONTRACT","Building, Expand/Contract"},
{"BUILDING_TRIGGER_ENABLE_WATER","Building, Trigger, Enable Water"},
{"BUILDING_TRIGGER_MIN_WATER_UP","Building, Trigger, Min Water Up"},
{"BUILDING_TRIGGER_MIN_WATER_DOWN","Building, Trigger, Min Water Down"},
{"BUILDING_TRIGGER_MAX_WATER_UP","Building, Trigger, Max Water Up"},
{"BUILDING_TRIGGER_MAX_WATER_DOWN","Building, Trigger, Max Water Down"},
{"BUILDING_TRIGGER_ENABLE_MAGMA","Building, Trigger, Enabler Magma"},
{"BUILDING_TRIGGER_MIN_MAGMA_UP","Building, Trigger, Min Magma Up"},
{"BUILDING_TRIGGER_MIN_MAGMA_DOWN","Building, Trigger, Min Magma Down"},
{"BUILDING_TRIGGER_MAX_MAGMA_UP","Building, Trigger, Max Magma Up"},
{"BUILDING_TRIGGER_MAX_MAGMA_DOWN","Building, Trigger, Max Magma Down"},
{"BUILDING_TRIGGER_ENABLE_CREATURE","Building, Trigger, Enable Creature"},
{"BUILDING_TRIGGER_ENABLE_LOCALS","Building, Trigger, Enable Locals"},
{"BUILDING_TRIGGER_RESETS","Building, Trigger, Resets"},
{"BUILDING_TRIGGER_MIN_SIZE_UP","Building, Trigger, Min Size Up"},
{"BUILDING_TRIGGER_MIN_SIZE_DOWN","Building, Trigger, Min Size Down"},
{"BUILDING_TRIGGER_MAX_SIZE_UP","Building, Trigger, Max Size Up"},
{"BUILDING_TRIGGER_MAX_SIZE_DOWN","Building, Trigger, Max Size Down"},
{"DESTROYBUILDING","Destroy Building"},
{"SUSPENDBUILDING","Suspend Building"},
{"BUILDINGLIST_ZOOM_T","Building List: Zoom T"},
{"BUILDINGLIST_ZOOM_Q","Building List: Zoom Q"},

//WORKSHOPKEY_START
{"HOTKEY_MAKE_ASH","Hotkey: Make Ash"},
{"HOTKEY_MAKE_CHARCOAL","Hotkey: Make Charcoal"},
{"HOTKEY_MELT_OBJECT","Hotkey: Melt Object"},
{"HOTKEY_GLASS_GREEN","Hotkey: Green Glass"},
{"HOTKEY_GLASS_CLEAR","Hotkey: Clear Glass"},
{"HOTKEY_GLASS_CRYSTAL","Hotkey: Crystal Glass"},
{"HOTKEY_COLLECT_SAND","Hotkey: Collect Sand"},
{"HOTKEY_GLASS_ROUGH","Hotkey: Raw Glass"},
{"HOTKEY_GLASS_ARMORSTAND","Hotkey: Glass Armorstand"},
{"HOTKEY_GLASS_BOX","Hotkey: Glass Box"},
{"HOTKEY_GLASS_CABINET","Hotkey: Glass Cabinet"},
{"HOTKEY_GLASS_COFFIN","Hotkey: Glass Coffin"},
{"HOTKEY_GLASS_FLOODGATE","Hotkey: Glass Floodgate"},
{"HOTKEY_GLASS_HATCH_COVER","Hotkey: Glass Hatch Cover"},
{"HOTKEY_GLASS_GRATE","Hotkey: Glass Grate"},
{"HOTKEY_GLASS_GOBLET","Hotkey: Glass Goblet"},
{"HOTKEY_GLASS_TOY","Hotkey: Glass Toy"},
{"HOTKEY_GLASS_INSTRUMENT","Hotkey: Glass Instrument"},
{"HOTKEY_GLASS_DOOR","Hotkey: Glass Portal"},
{"HOTKEY_GLASS_STATUE","Hotkey: Glass Statue"},
{"HOTKEY_GLASS_TABLE","Hotkey: Glass Table"},
{"HOTKEY_GLASS_CAGE","Hotkey: Glass Cage"},
{"HOTKEY_GLASS_CHAIR","Hotkey: Glass Chair"},
{"HOTKEY_GLASS_BLOCKS","Hotkey: Glass Blocks"},
{"HOTKEY_GLASS_FLASK","Hotkey: Glass Vial"},
{"HOTKEY_GLASS_WEAPONRACK","Hotkey: Glass Weaponrack"},
{"HOTKEY_GLASS_WINDOW","Hotkey: Glass Window"},
{"HOTKEY_MAKE_PEARLASH","Hotkey: Make Pearlash"},
{"HOTKEY_ASHERY_LYE","Hotkey: Ashery, Lye"},
{"HOTKEY_ASHERY_POTASH","Hotkey: Ashery, Potash (Lye)"},
{"HOTKEY_ASHERY_POTASH_DIRECT","Hotkey: Ashery, Potash (Ash)"},
{"HOTKEY_CARPENTER_BARREL","Hotkey: Carpenter, Barrel"},
{"HOTKEY_CARPENTER_BLOCKS","Hotkey: Carpenter, Blocks"},
{"HOTKEY_CARPENTER_BUCKET","Hotkey: Carpenter, Bucket"},
{"HOTKEY_CARPENTER_TRAP_ANIMAL","Hotkey: Carpenter, Animal Trap"},
{"HOTKEY_CARPENTER_CAGE","Hotkey: Carpenter, Cage"},
{"HOTKEY_CARPENTER_ARMORSTAND","Hotkey: Carpenter, Armorstand"},
{"HOTKEY_CARPENTER_BED","Hotkey: Carpenter, Bed"},
{"HOTKEY_CARPENTER_CHAIR","Hotkey: Carpenter, Chair"},
{"HOTKEY_CARPENTER_COFFIN","Hotkey: Carpenter, Coffin"},
{"HOTKEY_CARPENTER_DOOR","Hotkey: Carpenter, Door"},
{"HOTKEY_CARPENTER_FLOODGATE","Hotkey: Carpenter, Floodgate"},
{"HOTKEY_CARPENTER_HATCH_COVER","Hotkey: Carpenter, Hatch Cover"},
{"HOTKEY_CARPENTER_GRATE","Hotkey: Carpenter, Grate"},
{"HOTKEY_CARPENTER_CABINET","Hotkey: Carpenter, Cabinet"},
{"HOTKEY_CARPENTER_BIN","Hotkey: Carpenter, Bin"},
{"HOTKEY_CARPENTER_BOX","Hotkey: Carpenter, Box"},
{"HOTKEY_CARPENTER_WEAPONRACK","Hotkey: Carpenter, Weaponrack"},
{"HOTKEY_CARPENTER_TABLE","Hotkey: Carpenter, Table"},
{"HOTKEY_SIEGE_BALLISTA","Hotkey: Siege Shop, Ballista"},
{"HOTKEY_SIEGE_CATAPULT","Hotkey: Siege Shop, Catapult"},
{"HOTKEY_LEATHER_BOX","Hotkey: Leather, Bag"},
{"HOTKEY_LEATHER_FLASK","Hotkey: Leather, Flask"},
{"HOTKEY_LEATHER_SHIRT","Hotkey: Leather, Shirt"},
{"HOTKEY_LEATHER_CLOAK","Hotkey: Leather, Cloak"},
{"HOTKEY_LEATHER_BACKPACK","Hotkey: Leather, Backpack"},
{"HOTKEY_LEATHER_QUIVER","Hotkey: Leather, Quiver"},
{"HOTKEY_LEATHER_IMAGE","Hotkey: Leather, Image"},
{"HOTKEY_CLOTHES_MAT_PLANT","Hotkey: Clothes, Cloth"},
{"HOTKEY_CLOTHES_MAT_SILK","Hotkey: Clothes, Silk"},
{"HOTKEY_CLOTHES_SHIRT","Hotkey: Clothes, Shirt"},
{"HOTKEY_CLOTHES_CLOAK","Hotkey: Clothes, Cloak"},
{"HOTKEY_CLOTHES_BOX","Hotkey: Clothes, Box"},
{"HOTKEY_CLOTHES_CHAIN","Hotkey: Clothes, Rope"},
{"HOTKEY_CLOTHES_IMAGE","Hotkey: Clothes, Image"},
{"HOTKEY_CRAFTS_MAT_STONE","Hotkey: Crafts, Mat Stone"},
{"HOTKEY_CRAFTS_MAT_WOOD","Hotkey: Crafts, Mat Wood"},
{"HOTKEY_CRAFTS_DEC_BONE","Hotkey: Crafts, Dec Bone"},
{"HOTKEY_CRAFTS_DEC_SHELL","Hotkey: Crafts, Dec Shell"},
{"HOTKEY_CRAFTS_TOTEM","Hotkey: Crafts, Totem"},
{"HOTKEY_CRAFTS_CLOTH","Hotkey: Crafts, Cloth"},
{"HOTKEY_CRAFTS_SILK","Hotkey: Crafts, Silk"},
{"HOTKEY_CRAFTS_SHELL","Hotkey: Crafts, Shell"},
{"HOTKEY_CRAFTS_BONE","Hotkey: Crafts, Bone"},
{"HOTKEY_CRAFTS_LEATHER","Hotkey: Crafts, Leather"},
{"HOTKEY_CRAFTS_MAT_CRAFTS","Hotkey: Crafts, Mat Crafts"},
{"HOTKEY_CRAFTS_MAT_GOBLET","Hotkey: Crafts, Mat Goblet"},
{"HOTKEY_CRAFTS_MAT_INSTRUMENT","Hotkey: Crafts, Mat Instrument"},
{"HOTKEY_CRAFTS_MAT_TOY","Hotkey: Crafts, Mat Toy"},
{"HOTKEY_SMITH_WEAPON","Hotkey: Forge, Weapon"},
{"HOTKEY_SMITH_ARMOR","Hotkey: Forge, Armor"},
{"HOTKEY_SMITH_FURNITURE","Hotkey: Forge, Furniture"},
{"HOTKEY_SMITH_SIEGE","Hotkey: Forge, Siege"},
{"HOTKEY_SMITH_TRAP","Hotkey: Forge, Trap"},
{"HOTKEY_SMITH_OTHER","Hotkey: Forge, Other"},
{"HOTKEY_SMITH_METAL","Hotkey: Forge, Metal Clothing"},
{"HOTKEY_ALCHEMIST_SOAP","Hotkey: Alchemist, Soap"},
{"HOTKEY_STILL_BREW","Hotkey: Still, Brew"},
{"HOTKEY_STILL_EXTRACT","Hotkey: Still, Extract"},
{"HOTKEY_LOOM_COLLECT_SILK","Hotkey: Loom, Collect Silk"},
{"HOTKEY_LOOM_WEAVE_CLOTH","Hotkey: Loom, Weave Cloth"},
{"HOTKEY_LOOM_WEAVE_SILK","Hotkey: Loom, Weave Silk"},
{"HOTKEY_LOOM_WEAVE_METAL","Hotkey: Loom, Metal"},
{"HOTKEY_KITCHEN_COOK_2","Hotkey: Kitchen, Meal 1"},
{"HOTKEY_KITCHEN_COOK_3","Hotkey: Kitchen, Meal 2"},
{"HOTKEY_KITCHEN_COOK_4","Hotkey: Kitchen, Meal 3"},
{"HOTKEY_KITCHEN_RENDER_FAT","Hotkey: Kitchen, Render Fat"},
{"HOTKEY_FARMER_PROCESS","Hotkey: Farmer, Process"},
{"HOTKEY_FARMER_PROCESS_VIAL","Hotkey: Farmer, Vial"},
{"HOTKEY_FARMER_PROCESS_BAG","Hotkey: Farmer, Bag"},
{"HOTKEY_FARMER_PROCESS_BARREL","Hotkey: Farmer, Barrel"},
{"HOTKEY_FARMER_CHEESE","Hotkey: Farmer, Cheese"},
{"HOTKEY_FARMER_MILK","Hotkey: Farmer, Milk"},
{"HOTKEY_MILL_MILL","Hotkey: Mill, Mill"},
{"HOTKEY_KENNEL_TRAIN_HUNT","Hotkey: Kennel, Hunt"},
{"HOTKEY_KENNEL_CATCH_VERMIN","Hotkey: Kennel, Catch"},
{"HOTKEY_KENNEL_TAME_VERMIN","Hotkey: Kennel, Tame Small"},
{"HOTKEY_KENNEL_TAME_UNIT","Hotkey: Kennel, Tame Large"},
{"HOTKEY_KENNEL_TRAIN_WAR","Hotkey: Kennel, War"},
{"HOTKEY_FISHERY_PROCESS","Hotkey: Fishery, Process"},
{"HOTKEY_FISHERY_EXTRACT","Hotkey: Fishery, Extract"},
{"HOTKEY_FISHERY_CATCH","Hotkey: Fishery, Catch"},
{"HOTKEY_BUTCHER_BUTCHER","Hotkey: Butcher, Butcher"},
{"HOTKEY_BUTCHER_EXTRACT","Hotkey: Butcher, Extract"},
{"HOTKEY_BUTCHER_CATCH","Hotkey: Butcher, Catch"},
{"HOTKEY_TANNER_TAN","Hotkey: Tanner, Tan"},
{"HOTKEY_DYER_THREAD","Hotkey: Dyer, Thread"},
{"HOTKEY_DYER_CLOTH","Hotkey: Dyer, Cloth"},
{"HOTKEY_JEWELER_FURNITURE","Hotkey: Jeweler, Furniture"},
{"HOTKEY_JEWELER_FINISHED","Hotkey: Jeweler, Finished"},
{"HOTKEY_JEWELER_AMMO","Hotkey: Jeweler, Ammo"},
{"HOTKEY_JEWELER_CUT","Hotkey: Jeweler, Cut"},
{"HOTKEY_JEWELER_ENCRUST","Hotkey: Jeweler, Encrust"},
{"HOTKEY_MECHANIC_PARTS","Hotkey: Mechanic, Mechanisms"},
{"HOTKEY_MASON_ARMORSTAND","Hotkey: Mason, Armorstand"},
{"HOTKEY_MASON_BLOCKS","Hotkey: Mason, Blocks"},
{"HOTKEY_MASON_CHAIR","Hotkey: Mason, Chair"},
{"HOTKEY_MASON_COFFIN","Hotkey: Mason, Coffin"},
{"HOTKEY_MASON_DOOR","Hotkey: Mason, Door"},
{"HOTKEY_MASON_FLOODGATE","Hotkey: Mason, Floodgate"},
{"HOTKEY_MASON_HATCH_COVER","Hotkey: Mason, Hatch Cover"},
{"HOTKEY_MASON_GRATE","Hotkey: Mason, Grate"},
{"HOTKEY_MASON_CABINET","Hotkey: Mason, Cabinet"},
{"HOTKEY_MASON_BOX","Hotkey: Mason, Box"},
{"HOTKEY_MASON_STATUE","Hotkey: Mason, Statue"},
{"HOTKEY_MASON_TABLE","Hotkey: Mason, Table"},
{"HOTKEY_MASON_WEAPONRACK","Hotkey: Mason, Weaponrack"},
{"HOTKEY_MASON_QUERN","Hotkey: Mason, Quern"},
{"HOTKEY_MASON_MILLSTONE","Hotkey: Mason, Millstone"},
{"HOTKEY_TRAP_BRIDGE","Hotkey: Trap, Bridge"},
{"HOTKEY_TRAP_DOOR","Hotkey: Trap, Door"},
{"HOTKEY_TRAP_FLOODGATE","Hotkey: Trap, Floodgate"},
{"HOTKEY_TRAP_SPIKE","Hotkey: Trap, Spike"},
{"HOTKEY_TRAP_HATCH","Hotkey: Trap, Hatch"},
{"HOTKEY_TRAP_GRATE_WALL","Hotkey: Trap, Wall Grate"},
{"HOTKEY_TRAP_GRATE_FLOOR","Hotkey: Trap, Floor Grate"},
{"HOTKEY_TRAP_BARS_VERTICAL","Hotkey: Trap, Vertical Bars"},
{"HOTKEY_TRAP_BARS_FLOOR","Hotkey: Trap, Floor Bars"},
{"HOTKEY_TRAP_SUPPORT","Hotkey: Trap, Support"},
{"HOTKEY_TRAP_CHAIN","Hotkey: Trap, Chain"},
{"HOTKEY_TRAP_CAGE","Hotkey: Trap, Cage"},
{"HOTKEY_TRAP_GEAR_ASSEMBLY","Hotkey: Trap, Gear Assembly"},
{"HOTKEY_TRAP_PULL_LEVER","Hotkey: Trap, Lever"},

//PILEZONEKEY_START
{"BUILDJOB_STOCKPILE_MASTER","Buildjob: Stockpile, Master"},
{"BUILDJOB_STOCKPILE_DELETE_CHILD","Buildjob: Stockpile, Delete Child"},
{"BUILDJOB_STOCKPILE_SETTINGS","Buildjob: Stockpile, Settings"},
{"BUILDJOB_STOCKPILE_BARREL_UP","Buildjob: Stockpile, Barrel Up"},
{"BUILDJOB_STOCKPILE_BARREL_DOWN","Buildjob: Stockpile, Barrel Down"},
{"BUILDJOB_STOCKPILE_BARREL_ZERO","Buildjob: Stockpile, Barrel Zero"},
{"BUILDJOB_STOCKPILE_BARREL_MAX","Buildjob: Stockpile, Barrel Max"},
{"BUILDJOB_STOCKPILE_BIN_UP","Buildjob: Stockpile, Bin Up"},
{"BUILDJOB_STOCKPILE_BIN_DOWN","Buildjob: Stockpile, Bin Down"},
{"BUILDJOB_STOCKPILE_BIN_ZERO","Buildjob: Stockpile, Bin Zero"},
{"BUILDJOB_STOCKPILE_BIN_MAX","Buildjob: Stockpile, Bin Max"},
{"STOCKPILE_ANIMAL","Stockpile, Animal"},
{"STOCKPILE_FOOD","Stockpile, Food"},
{"STOCKPILE_WEAPON","Stockpile, Weapon"},
{"STOCKPILE_ARMOR","Stockpile, Armor"},
{"STOCKPILE_CUSTOM","Stockpile, Custom"},
{"STOCKPILE_CUSTOM_SETTINGS","Stockpile, Custom Settings"},
{"STOCKPILE_FURNITURE","Stockpile, Furniture"},
{"STOCKPILE_GRAVEYARD","Stockpile, Graveyard"},
{"STOCKPILE_REFUSE","Stockpile, Refuse"},
{"STOCKPILE_WOOD","Stockpile, Wood"},
{"STOCKPILE_STONE","Stockpile, Stone"},
{"STOCKPILE_GEM","Stockpile, Gem"},
{"STOCKPILE_BARBLOCK","Stockpile, Bar/Block"},
{"STOCKPILE_CLOTH","Stockpile, Cloth"},
{"STOCKPILE_LEATHER","Stockpile, Leather"},
{"STOCKPILE_AMMO","Stockpile, Ammo"},
{"STOCKPILE_COINS","Stockpile, Coins"},
{"STOCKPILE_FINISHED","Stockpile, Finished"},
{"STOCKPILE_NONE","Stockpile, None"},
{"STOCKPILE_SETTINGS_ENABLE","Stockpile Settings: Enable"},
{"STOCKPILE_SETTINGS_DISABLE","Stockpile Settings: Disable"},
{"STOCKPILE_SETTINGS_PERMIT_ALL","Stockpile Settings: Permit All"},
{"STOCKPILE_SETTINGS_FORBID_ALL","Stockpile Settings: Forbid All"},
{"STOCKPILE_SETTINGS_PERMIT_SUB","Stockpile Settings: Permit Sub"},
{"STOCKPILE_SETTINGS_FORBID_SUB","Stockpile Settings: Forbit Sub"},
{"STOCKPILE_SETTINGS_SPECIFIC1","Stockpile Settings: Specific 1"},
{"STOCKPILE_SETTINGS_SPECIFIC2","Stockpile Settings: Specific 2"},
{"CIVZONE_REMOVE","Main: Activity Zone, Remove"},
{"CIVZONE_WATER_SOURCE","Main: Activity Zone, Water Source"},
{"CIVZONE_FISH","Main: Activity Zone, Fish"},
{"CIVZONE_DUMP","Main: Activity Zone, Garbage Dump"},
{"CIVZONE_POND","Main: Activity Zone, Pond"},
{"CIVZONE_SAND_COLLECT","Main: Activity Zone, Sand Collect"},
{"CIVZONE_ACTIVE","Main: Activity Zone, Active"},
{"CIVZONE_MEETING","Main: Activity Zone, Meeting"},
{"CIVZONE_POND_OPTIONS","Main: Activity Zone, Pond Options"},
{"CIVZONE_POND_WATER","Main: Activity Zone: Pond, Water"},

//STOCKORDERKEY_START
{"STORES_VIEW","Stores, View"},
{"STORES_ZOOM","Stores, Zoom"},
{"STORES_FORBID","Stores, Forbid"},
{"STORES_MELT","Stores, Melt"},
{"STORES_DUMP","Stores, Dump"},
{"STORES_HIDE","Stores, Hide"},
{"PET_BUTCHER","Animals, Slaughter"},
{"KITCHEN_COOK","Kitchen, Cook"},
{"KITCHEN_BREW","Kitchen, Brew"},
{"ORDERS_AUTOFORBID","Orders: Forbid"},
{"ORDERS_FORBID_PROJECTILE","Orders: Forbid Projectiles"},
{"ORDERS_FORBID_YOUR_CORPSE","Orders: Forbid Your Corpse"},
{"ORDERS_FORBID_YOUR_ITEMS","Orders: Forbid Your Items"},
{"ORDERS_FORBID_OTHER_CORPSE","Orders: Forbid Other Corpse"},
{"ORDERS_FORBID_OTHER_ITEMS","Orders: Forbid Other Items"},
{"ORDERS_REFUSE_GATHER","Orders: Gather Refuse"},
{"ORDERS_REFUSE_OUTSIDE","Orders: Gather Outside"},
{"ORDERS_REFUSE_DUMP_CORPSE","Orders: Dump Corpse"},
{"ORDERS_REFUSE_DUMP_SKULL","Orders: Dump Skull"},
{"ORDERS_REFUSE_DUMP_BONE","Orders: Dump Bone"},
{"ORDERS_REFUSE_DUMP_SHELL","Orders: Dump Shell"},
{"ORDERS_REFUSE_DUMP_SKIN","Orders: Dump Skin"},
{"ORDERS_REFUSE_DUMP_OTHER","Orders: Dump Other"},
{"ORDERS_GATHER_FURNITURE","Orders: Gather Furniture"},
{"ORDERS_GATHER_ANIMALS","Orders: Gather Animals"},
{"ORDERS_GATHER_FOOD","Orders: Gather Food"},
{"ORDERS_GATHER_BODIES","Orders: Gather Bodies"},
{"ORDERS_REFUSE","Orders: Refuse"},
{"ORDERS_GATHER_STONE","Orders: Gather Stone"},
{"ORDERS_GATHER_WOOD","Orders: Gather Wood"},
{"ORDERS_ALL_HARVEST","Orders: All Harvest"},
{"ORDERS_INDOORS","Orders: Stay Indoors"},
{"ORDERS_SAMEPILE","Orders: Piles Same"},
{"ORDERS_MIXFOODS","Orders: Mix Foods"},
{"ORDERS_LOOM","Orders: Loom"},
{"ORDERS_DYED_CLOTH","Orders: Dyed Cloth"},
{"ORDERS_EXCEPTIONS","Orders: Exceptions"},
{"ORDERS_WORKSHOP","Orders: Workshop"},
{"ORDERS_COLLECT_WEB","Orders: Collect Web"},
{"ORDERS_SLAUGHTER","Orders: Slaughter"},
{"ORDERS_BUTCHER","Orders: Butcher"},
{"ORDERS_TAN","Orders: Tan"},
{"ORDERS_ZONE","Orders: Zone"},
{"ORDERS_ZONE_DRINKING","Orders: Zone, Drinking"},
{"ORDERS_ZONE_FISHING","Orders: Zone, Fishing"},

//DWARFMAINKEY_START
{"D_ONESTEP","Main: One-Step"},
{"D_PAUSE","Main: Pause/Resume"},
{"D_DEPOT","Depot Access"},
{"D_HOT_KEYS","Main: Hot Keys"},
{"D_HOTKEY1","Main: Hot Key 1"},
{"D_HOTKEY2","Main: Hot Key 2"},
{"D_HOTKEY3","Main: Hot Key 3"},
{"D_HOTKEY4","Main: Hot Key 4"},
{"D_HOTKEY5","Main: Hot Key 5"},
{"D_HOTKEY6","Main: Hot Key 6"},
{"D_HOTKEY7","Main: Hot Key 7"},
{"D_HOTKEY8","Main: Hot Key 8"},
{"D_HOTKEY9","Main: Hot Key 9"},
{"D_HOTKEY10","Main: Hot Key 10"},
{"D_HOTKEY11","Main: Hot Key 11"},
{"D_HOTKEY12","Main: Hot Key 12"},
{"D_HOTKEY13","Main: Hot Key 13"},
{"D_HOTKEY14","Main: Hot Key 14"},
{"D_HOTKEY15","Main: Hot Key 15"},
{"D_HOTKEY16","Main: Hot Key 16"},
{"D_HOTKEY_CHANGE_NAME","Hot Keys: Change Name"},
{"D_HOTKEY_ZOOM","Hot Keys: Zoom"},
{"D_ANNOUNCE","Main: Announcements"},
{"D_BUILDING","Main: Place Building"},
{"D_CIVLIST","Main: Civilizations"},
{"D_DESIGNATE","Main: Designations"},
{"D_ARTLIST","Main: Artifacts"},
{"D_NOBLES","Main: Nobles"},
{"D_ORDERS","Main: Standing Orders"},
{"D_MILITARY","Main: Military"},
{"D_ROOMS","Main: Rooms"},
{"D_SQUADS","Main: Control Squads"},
{"D_STOCKPILES","Main: Stockpiles"},
{"D_CIVZONE","Main: Activity Zone"},
{"D_VIEWUNIT","Main: View Units"},
{"D_JOBLIST","Main: Job List"},
{"D_UNITLIST","Main: Unit List"},
{"D_LOOK","Main: Look"},
{"D_NOTE","Main: Note"},
{"D_NOTE_PLACE","Main: Note, Place"},
{"D_NOTE_DELETE","Main: Note, Delete"},
{"D_NOTE_ENTER","Main: Note, Enter Text"},
{"D_NOTE_ADOPT_SYMBOL","Main: Note, Adopt Symbol"},
{"D_NOTE_CHANGE_SELECTION","Main: Note, Change Selection"},
{"D_BUILDJOB","Main: Building Jobs"},
{"D_STATUS","Main: Overall Status"},
{"D_BUILDITEM","Main: Building Items"},
{"D_VISUALIZE","Main: Visualize"},
{"D_BITEM_FORBID","Building Items: Forbid"},
{"D_BITEM_DUMP","Building Items: Dump"},
{"D_BITEM_MELT","Building Items: Melt"},
{"D_BITEM_HIDE","Building Items: Hide"},
{"D_LOOK_FORBID","Dwf Look: Forbid"},
{"D_LOOK_DUMP","Dwf Look: Dump"},
{"D_LOOK_MELT","Dwf Look: Melt"},
{"D_LOOK_HIDE","Dwf Look: Hide"},
{"ASSIGNTRADE_VIEW","Assign Trade: View"},
{"ASSIGNTRADE_STRING","Assign Trade: String"},
{"ASSIGNTRADE_EXCLUDE_PROHIBITED","Assign Trade: Exclude Prohibited"},
{"ASSIGNTRADE_PENDING","Assign Trade: Pending"},
{"ASSIGNTRADE_SORT","Assign Trade: Sort"},
{"TRADE_VIEW","Trade, View"},
{"TRADE_TRADE","Trade, Trade"},
{"TRADE_OFFER","Trade, Offer"},
{"TRADE_SEIZE","Trade, Seize"},
{"UNITJOB_REMOVE_CRE","Unitjob, Remove Unit"},
{"UNITJOB_ZOOM_CRE","Unitjob, Zoom Unit"},
{"UNITJOB_ZOOM_BUILD","Unitjob, Zoom Building"},
{"UNITJOB_VIEW","Unitjob, View"},
{"UNITJOB_MANAGER","Unitjob, Manager"},
{"MANAGER_NEW_ORDER","Manager, New Order"},
{"MANAGER_REMOVE","Manager, Remove"},
{"MANAGER_PROMOTE","Manager, Promote"},
{"MANAGER_MAX","Manager, Max"},
{"MANAGER_WAGES","Manager, Wages"},
{"NOBLELIST_REPLACE","Noble List: Replace"},
{"NOBLELIST_SETTINGS","Noble List: Settings"},
{"NOBLELIST_VIEW_CANDIDATE","Noble List: View Candidate"},
{"DESIGNATE_BITEM","Designate, Building/Item"},
{"DESIGNATE_CLAIM","Designate, Reclaim"},
{"DESIGNATE_UNCLAIM","Designate, Forbid"},
{"DESIGNATE_MELT","Designate, Melt"},
{"DESIGNATE_NO_MELT","Designate, No Melt"},
{"DESIGNATE_DUMP","Designate, Dump"},
{"DESIGNATE_NO_DUMP","Designate, No Dump"},
{"DESIGNATE_HIDE","Designate, Hide"},
{"DESIGNATE_NO_HIDE","Designate, No Hide"},
{"DESIGNATE_DIG","Designate, Dig"},
{"DESIGNATE_DIG_REMOVE_STAIRS_RAMPS","Designate, Dig Remove Stairs Ramps"},
{"DESIGNATE_TRAFFIC","Designate, Traffic"},
{"DESIGNATE_TRAFFIC_HIGH","Designate, High Traffic"},
{"DESIGNATE_TRAFFIC_NORMAL","Designate, Normal Traffic"},
{"DESIGNATE_TRAFFIC_LOW","Designate, Low Traffic"},
{"DESIGNATE_TRAFFIC_RESTRICTED","Designate, Restricted Traffic"},
{"DESIGNATE_TRAFFIC_INCREASE_WEIGHT","Designate, Increase Weight"},
{"DESIGNATE_TRAFFIC_DECREASE_WEIGHT","Designate, Decrease Weight"},
{"DESIGNATE_TRAFFIC_INCREASE_WEIGHT_MORE","Designate, Increase Weight More"},
{"DESIGNATE_TRAFFIC_DECREASE_WEIGHT_MORE","Designate, Decrease Weight More"},
{"DESIGNATE_STAIR_UP","Designate, U Stair"},
{"DESIGNATE_STAIR_DOWN","Designate, D Stair"},
{"DESIGNATE_STAIR_UPDOWN","Designate, UD Stair"},
{"DESIGNATE_RAMP","Designate, Ramp"},
{"DESIGNATE_CHANNEL","Designate, Channel"},
{"DESIGNATE_CHOP","Designate, Chop"},
{"DESIGNATE_PLANTS","Designate, Plants"},
{"DESIGNATE_SMOOTH","Designate, Smooth"},
{"DESIGNATE_ENGRAVE","Designate, Engrave"},
{"DESIGNATE_FORTIFY","Designate, Fortify"},
{"DESIGNATE_TOGGLE_ENGRAVING","Designate, Toggle Engraving"},
{"DESIGNATE_UNDO","Designate, Undo"},
{"DESIGNATE_REMOVE_CONSTRUCTION","Designate, Remove Construction"},
{"ITEM_DESCRIPTION","View item: Description"},
{"ITEM_FORBID","View item: Forbid"},
{"ITEM_MELT","View item: Melt"},
{"ITEM_DUMP","View item: Dump"},
{"ITEM_HIDE","View item: Hide"},
{"UNITVIEW_GEN","Unitview, General"},
{"UNITVIEW_INV","Unitview, Inventory"},
{"UNITVIEW_PRF","Unitview, Prefs"},
{"UNITVIEW_WND","Unitview, Wounds"},
{"UNITVIEW_NEXT","Unitview, Next Unit"},
{"UNITVIEW_SLAUGHTER","Unitview, Slaughter"},
{"UNITVIEW_PRF_PROF","Unitview, Prefs, Labor"},
{"UNITVIEW_PRF_PET","Unitview, Prefs, Pets"},
{"UNITVIEW_PRF_SOLDIER","Unitview, Prefs, Soldier"},
{"UNITVIEW_PRF_CASTLE","Unitview, Prefs, Fortress Guard"},
{"UNITVIEW_PRF_ROYAL","Unitview, Prefs, Royal Guard"},
{"UNITVIEW_PRF_TOGGLESQUAD","Unitview, Prefs, Toggle Activation"},
{"UNITVIEW_PRF_PET_HUNT","Unitview, Prefs, Pets, Hunting"},
{"UNITVIEW_PRF_PET_WAR","Unitview, Prefs, Pets, War"},
{"UNITVIEW_PRF_SOLDIER_UNARMED","Unitview, Prefs, Soldier, Unarmed"},
{"UNITVIEW_PRF_SOLDIER_AXE","Unitview, Prefs, Soldier, Axe"},
{"UNITVIEW_PRF_SOLDIER_CROSSBOW","Unitview, Prefs, Soldier, Crossbow"},
{"UNITVIEW_PRF_SOLDIER_HAMMER","Unitview, Prefs, Soldier, Hammer"},
{"UNITVIEW_PRF_SOLDIER_MACE","Unitview, Prefs, Soldier, Mace"},
{"UNITVIEW_PRF_SOLDIER_SPEAR","Unitview, Prefs, Soldier, Spear"},
{"UNITVIEW_PRF_SOLDIER_SWORD","Unitview, Prefs, Soldier, Sword"},
{"UNITVIEW_PRF_SOLDIER_WEAPONNUM","Unitview, Prefs, Soldier, Weapon Num"},
{"UNITVIEW_PRF_SOLDIER_ARMOR","Unitview, Prefs, Soldier, Armor"},
{"UNITVIEW_PRF_SOLDIER_SHIELD","Unitview, Prefs, Soldier, Shield"},
{"UNITVIEW_PRF_VIEW","Unitview, Prefs, Profile"},
{"UNITVIEW_CUSTOMIZE","Unit View, Customize"},
{"UNITVIEW_RELATIONSHIPS","Unit View, Relationships"},
{"UNITVIEW_RELATIONSHIPS_ZOOM","Unit View, Relationships, Zoom"},
{"UNITVIEW_RELATIONSHIPS_VIEW","Unit View, Relationships, View"},
{"UNITVIEW_KILLS","Unit View, Kills"},
{"CUSTOMIZE_UNIT_NICKNAME","Customize Unit, Nickname"},
{"CUSTOMIZE_UNIT_PROFNAME","Customize Unit, Profession"},

//MILITIAKEY_START
{"SQUAD_LOCK","Squad: Lock"},
{"SQUAD_STATION","Squad: Station"},
{"SQUAD_CLEAR_PATROL","Squad: Clear Patrol"},
{"SQUAD_PATROL","Squad: Patrol"},
{"SQUAD_ZOOM","Squad: Zoom"},
{"MILITARY_ACTIVATE","Military, Activate"},
{"MILITARY_VIEW","Military, View"},
{"MILITARY_WEAPON","Military, Weapon"},
{"MILITARY_ZOOM","Military, Zoom"},
{"SQUADINFO_FOOD","Squad Info, Food"},
{"SQUADINFO_SLEEP","Squad Info, Sleep"},
{"SQUADINFO_WATER","Squad Info, Water"},
{"SQUADINFO_STAYCLOSE","Squad Info, Stay Close"},
{"SQUADINFO_ATTACKWILDS","Squad Info, Attack Wilderness"},
{"SQUADINFO_STANDDOWN","Squad Info, Stand Down"},
{"SQUADINFO_ZOOM","Squad Info, Zoom"},

//TEXTENTRY
{"STRING_A000","String: Backspace"},
{"STRING_A001","String: Character 001"},
{"STRING_A002","String: Character 002"},
{"STRING_A003","String: Character 003"},
{"STRING_A004","String: Character 004"},
{"STRING_A005","String: Character 005"},
{"STRING_A006","String: Character 006"},
{"STRING_A007","String: Character 007"},
{"STRING_A008","String: Character 008"},
{"STRING_A009","String: Character 009"},
{"STRING_A010","String: Character 010"},
{"STRING_A011","String: Character 011"},
{"STRING_A012","String: Character 012"},
{"STRING_A013","String: Character 013"},
{"STRING_A014","String: Character 014"},
{"STRING_A015","String: Character 015"},
{"STRING_A016","String: Character 016"},
{"STRING_A017","String: Character 017"},
{"STRING_A018","String: Character 018"},
{"STRING_A019","String: Character 019"},
{"STRING_A020","String: Character 020"},
{"STRING_A021","String: Character 021"},
{"STRING_A022","String: Character 022"},
{"STRING_A023","String: Character 023"},
{"STRING_A024","String: Character 024"},
{"STRING_A025","String: Character 025"},
{"STRING_A026","String: Character 026"},
{"STRING_A027","String: Character 027"},
{"STRING_A028","String: Character 028"},
{"STRING_A029","String: Character 029"},
{"STRING_A030","String: Character 030"},
{"STRING_A031","String: Character 031"},
{"STRING_A032","String: Character 032"},
{"STRING_A033","String: Character 033"},
{"STRING_A034","String: Character 034"},
{"STRING_A035","String: Character 035"},
{"STRING_A036","String: Character 036"},
{"STRING_A037","String: Character 037"},
{"STRING_A038","String: Character 038"},
{"STRING_A039","String: Character 039"},
{"STRING_A040","String: Character 040"},
{"STRING_A041","String: Character 041"},
{"STRING_A042","String: Character 042"},
{"STRING_A043","String: Character 043"},
{"STRING_A044","String: Character 044"},
{"STRING_A045","String: Character 045"},
{"STRING_A046","String: Character 046"},
{"STRING_A047","String: Character 047"},
{"STRING_A048","String: Character 048"},
{"STRING_A049","String: Character 049"},
{"STRING_A050","String: Character 050"},
{"STRING_A051","String: Character 051"},
{"STRING_A052","String: Character 052"},
{"STRING_A053","String: Character 053"},
{"STRING_A054","String: Character 054"},
{"STRING_A055","String: Character 055"},
{"STRING_A056","String: Character 056"},
{"STRING_A057","String: Character 057"},
{"STRING_A058","String: Character 058"},
{"STRING_A059","String: Character 059"},
{"STRING_A060","String: Character 060"},
{"STRING_A061","String: Character 061"},
{"STRING_A062","String: Character 062"},
{"STRING_A063","String: Character 063"},
{"STRING_A064","String: Character 064"},
{"STRING_A065","String: Character 065"},
{"STRING_A066","String: Character 066"},
{"STRING_A067","String: Character 067"},
{"STRING_A068","String: Character 068"},
{"STRING_A069","String: Character 069"},
{"STRING_A070","String: Character 070"},
{"STRING_A071","String: Character 071"},
{"STRING_A072","String: Character 072"},
{"STRING_A073","String: Character 073"},
{"STRING_A074","String: Character 074"},
{"STRING_A075","String: Character 075"},
{"STRING_A076","String: Character 076"},
{"STRING_A077","String: Character 077"},
{"STRING_A078","String: Character 078"},
{"STRING_A079","String: Character 079"},
{"STRING_A080","String: Character 080"},
{"STRING_A081","String: Character 081"},
{"STRING_A082","String: Character 082"},
{"STRING_A083","String: Character 083"},
{"STRING_A084","String: Character 084"},
{"STRING_A085","String: Character 085"},
{"STRING_A086","String: Character 086"},
{"STRING_A087","String: Character 087"},
{"STRING_A088","String: Character 088"},
{"STRING_A089","String: Character 089"},
{"STRING_A090","String: Character 090"},
{"STRING_A091","String: Character 091"},
{"STRING_A092","String: Character 092"},
{"STRING_A093","String: Character 093"},
{"STRING_A094","String: Character 094"},
{"STRING_A095","String: Character 095"},
{"STRING_A096","String: Character 096"},
{"STRING_A097","String: Character 097"},
{"STRING_A098","String: Character 098"},
{"STRING_A099","String: Character 099"},
{"STRING_A100","String: Character 100"},
{"STRING_A101","String: Character 101"},
{"STRING_A102","String: Character 102"},
{"STRING_A103","String: Character 103"},
{"STRING_A104","String: Character 104"},
{"STRING_A105","String: Character 105"},
{"STRING_A106","String: Character 106"},
{"STRING_A107","String: Character 107"},
{"STRING_A108","String: Character 108"},
{"STRING_A109","String: Character 109"},
{"STRING_A110","String: Character 110"},
{"STRING_A111","String: Character 111"},
{"STRING_A112","String: Character 112"},
{"STRING_A113","String: Character 113"},
{"STRING_A114","String: Character 114"},
{"STRING_A115","String: Character 115"},
{"STRING_A116","String: Character 116"},
{"STRING_A117","String: Character 117"},
{"STRING_A118","String: Character 118"},
{"STRING_A119","String: Character 119"},
{"STRING_A120","String: Character 120"},
{"STRING_A121","String: Character 121"},
{"STRING_A122","String: Character 122"},
{"STRING_A123","String: Character 123"},
{"STRING_A124","String: Character 124"},
{"STRING_A125","String: Character 125"},
{"STRING_A126","String: Character 126"},
{"STRING_A127","String: Character 127"},
{"STRING_A128","String: Character 128"},
{"STRING_A129","String: Character 129"},
{"STRING_A130","String: Character 130"},
{"STRING_A131","String: Character 131"},
{"STRING_A132","String: Character 132"},
{"STRING_A133","String: Character 133"},
{"STRING_A134","String: Character 134"},
{"STRING_A135","String: Character 135"},
{"STRING_A136","String: Character 136"},
{"STRING_A137","String: Character 137"},
{"STRING_A138","String: Character 138"},
{"STRING_A139","String: Character 139"},
{"STRING_A140","String: Character 140"},
{"STRING_A141","String: Character 141"},
{"STRING_A142","String: Character 142"},
{"STRING_A143","String: Character 143"},
{"STRING_A144","String: Character 144"},
{"STRING_A145","String: Character 145"},
{"STRING_A146","String: Character 146"},
{"STRING_A147","String: Character 147"},
{"STRING_A148","String: Character 148"},
{"STRING_A149","String: Character 149"},
{"STRING_A150","String: Character 150"},
{"STRING_A151","String: Character 151"},
{"STRING_A152","String: Character 152"},
{"STRING_A153","String: Character 153"},
{"STRING_A154","String: Character 154"},
{"STRING_A155","String: Character 155"},
{"STRING_A156","String: Character 156"},
{"STRING_A157","String: Character 157"},
{"STRING_A158","String: Character 158"},
{"STRING_A159","String: Character 159"},
{"STRING_A160","String: Character 160"},
{"STRING_A161","String: Character 161"},
{"STRING_A162","String: Character 162"},
{"STRING_A163","String: Character 163"},
{"STRING_A164","String: Character 164"},
{"STRING_A165","String: Character 165"},
{"STRING_A166","String: Character 166"},
{"STRING_A167","String: Character 167"},
{"STRING_A168","String: Character 168"},
{"STRING_A169","String: Character 169"},
{"STRING_A170","String: Character 170"},
{"STRING_A171","String: Character 171"},
{"STRING_A172","String: Character 172"},
{"STRING_A173","String: Character 173"},
{"STRING_A174","String: Character 174"},
{"STRING_A175","String: Character 175"},
{"STRING_A176","String: Character 176"},
{"STRING_A177","String: Character 177"},
{"STRING_A178","String: Character 178"},
{"STRING_A179","String: Character 179"},
{"STRING_A180","String: Character 180"},
{"STRING_A181","String: Character 181"},
{"STRING_A182","String: Character 182"},
{"STRING_A183","String: Character 183"},
{"STRING_A184","String: Character 184"},
{"STRING_A185","String: Character 185"},
{"STRING_A186","String: Character 186"},
{"STRING_A187","String: Character 187"},
{"STRING_A188","String: Character 188"},
{"STRING_A189","String: Character 189"},
{"STRING_A190","String: Character 190"},
{"STRING_A191","String: Character 191"},
{"STRING_A192","String: Character 192"},
{"STRING_A193","String: Character 193"},
{"STRING_A194","String: Character 194"},
{"STRING_A195","String: Character 195"},
{"STRING_A196","String: Character 196"},
{"STRING_A197","String: Character 197"},
{"STRING_A198","String: Character 198"},
{"STRING_A199","String: Character 199"},
{"STRING_A200","String: Character 200"},
{"STRING_A201","String: Character 201"},
{"STRING_A202","String: Character 202"},
{"STRING_A203","String: Character 203"},
{"STRING_A204","String: Character 204"},
{"STRING_A205","String: Character 205"},
{"STRING_A206","String: Character 206"},
{"STRING_A207","String: Character 207"},
{"STRING_A208","String: Character 208"},
{"STRING_A209","String: Character 209"},
{"STRING_A210","String: Character 210"},
{"STRING_A211","String: Character 211"},
{"STRING_A212","String: Character 212"},
{"STRING_A213","String: Character 213"},
{"STRING_A214","String: Character 214"},
{"STRING_A215","String: Character 215"},
{"STRING_A216","String: Character 216"},
{"STRING_A217","String: Character 217"},
{"STRING_A218","String: Character 218"},
{"STRING_A219","String: Character 219"},
{"STRING_A220","String: Character 220"},
{"STRING_A221","String: Character 221"},
{"STRING_A222","String: Character 222"},
{"STRING_A223","String: Character 223"},
{"STRING_A224","String: Character 224"},
{"STRING_A225","String: Character 225"},
{"STRING_A226","String: Character 226"},
{"STRING_A227","String: Character 227"},
{"STRING_A228","String: Character 228"},
{"STRING_A229","String: Character 229"},
{"STRING_A230","String: Character 230"},
{"STRING_A231","String: Character 231"},
{"STRING_A232","String: Character 232"},
{"STRING_A233","String: Character 233"},
{"STRING_A234","String: Character 234"},
{"STRING_A235","String: Character 235"},
{"STRING_A236","String: Character 236"},
{"STRING_A237","String: Character 237"},
{"STRING_A238","String: Character 238"},
{"STRING_A239","String: Character 239"},
{"STRING_A240","String: Character 240"},
{"STRING_A241","String: Character 241"},
{"STRING_A242","String: Character 242"},
{"STRING_A243","String: Character 243"},
{"STRING_A244","String: Character 244"},
{"STRING_A245","String: Character 245"},
{"STRING_A246","String: Character 246"},
{"STRING_A247","String: Character 247"},
{"STRING_A248","String: Character 248"},
{"STRING_A249","String: Character 249"},
{"STRING_A250","String: Character 250"},
{"STRING_A251","String: Character 251"},
{"STRING_A252","String: Character 252"},
{"STRING_A253","String: Character 253"},
{"STRING_A254","String: Character 254"},
{"STRING_A255","String: Character 255"}
//one missing check
//,{"",""}
};

unsigned int TokenizeLine(svector<char*> &Tokens,char* Line) {
 //returns count of tokens found
 //places a pointer to the start of each token in Tokens
 //sets 0 at the end of each token in Line
 char* pos=Line;
 //skip whitespace characters
 while (*pos) {
  if ((*pos==' ')||(*pos==9)) ++pos;
  else break;
 }
 if (*pos!='[') return 0;
 Tokens.clear();
 ++pos;
 Tokens.push_back(pos);
 while (*pos) {
  if (*pos==':') {
   *pos=0;
   ++pos;
   Tokens.push_back(pos);
  } else if (*pos==']') *pos=0;
  else ++pos;
 }
 return Tokens.size();
}

keybindingst::keybindingst(int32_t ID) {
 BindID=ID;
 keys.reserve(5);
 KeyAccum=0;
}

void keybindingst::Clear() {
 keys.clear();
 KeyAccum=0;
}

int keybindingst::GetKeyDisplay (char* Display, int Limit) {
 *Display=0;
 char* Disp=Display;
 int i=0;
 int j=keys.size();
 int k=j;
 if (Limit) {
  if (k>Limit) k=Limit;
  while (i<k) {
   if (i) Disp=stpcpy(Disp,", ");
   Disp=ConvertKeyToDisplay(Disp, keys[i]);
   ++i;
  }
  if (j>Limit) {
   Disp=stpcpy(Disp,", ...");
  }
 } else if (j) Disp=ConvertKeyToDisplay(Disp, keys[0]);
 if (Disp==Display) {
  strcpy(Display,"None");
  return 4;
 }
 return Disp-Display;
}


int keybindingst::GetKeyCount() {
 int i=keys.size();
 while (i) {
  --i;
  if (!(keys[i].flags&KEY_EVENTFLAG)) return i+1;
 };
 return 0;
}

KeyUnion* keybindingst::GetKeyArray() {
 if (keys.size()) return &(keys[0]);
 return 0;
}

int keybindingst::AddKey(KeyUnion key) {
 int i=keys.size();
 int j=i;
 if (!j) {
  keys.push_back(key);
  KeyAccum=~key.Value;
  return 1;
 }
 while (i) {
  --i;
  if (keys[i].Value==key.Value) return j;
 }
 KeyAccum&=~key.Value;
 //keep the events at the end of the binding list so we don't have to
 //work too hard at display and save, users are not allowed to bind to events
 if (key.flags&KEY_EVENTFLAG) {
  keys.push_back(key);
  return j+1;
 }
 while (i<j) {
  if (keys[i].flags&KEY_EVENTFLAG) break;
  ++i;
 }
 if (i==j) {
  keys.push_back(key);
  return j+1;
 }
 keys.push_back(keys[i]);
 keys[i].Value=key.Value;
 return j+1;
}

int keybindingst::RemoveKey(KeyUnion key) {
 int i=keys.size();
 int j=i;
 while (i) {
  --i;
  if (keys[i].Value==key.Value) {
   keys.erase(i);
   --j;
   i=j;
   KeyAccum=0;
   while (i) {
    --i;
    KeyAccum|=keys[i].Value;
   }
   KeyAccum=~KeyAccum;
   return j;
  }
 }
 return j;
}

int keybindingst::RemoveKeyPos(int pos) {
 int i=keys.size();
 if (pos<i) {
  keys.erase(pos);
  --i;
  int j=i;
  KeyAccum=0;
  while (i) {
   --i;
   KeyAccum|=keys[i].Value;
  }
  KeyAccum=~KeyAccum;
  return j;
 }
 return i;
}

macrostackst::macrostackst(MacroCommand* Commands, unsigned short ID, int count) {
 coms=Commands;
 macroID=ID;
 comCount=count;
 curCom.Value=0;
 position=0;
 repeat=0;
 nextproc=0;
 overflow=0xFFFFFFFF;
}

char* keybindingst::GetDisplayName(char* Name) {
 strcpy(Name,displayname.c_str());
 if (!*Name) strcpy(Name, InterfaceNames[BindID].name);
 return Name;
}

char* keybindingst::ChangeDisplayName(char* DisplayName) {
 if (*DisplayName==0) displayname.erase();
 else displayname=DisplayName;
 return DisplayName;
}

int macrobindingst::GetComCount() {
 return coms.size();
}

MacroCommand* macrobindingst::GetComArray() {
 if (coms.size()) return &(coms[0]);
 else return 0;
}

int macrobindingst::AddCommand(MacroCommand com) {
 if (!com.Repeat) com.Repeat=1;
 coms.push_back(com);
 return coms.size();
}

int macrobindingst::RemoveCommand(int pos) {
 int i=coms.size();
 if (pos<i) {
  coms.erase(pos);
  return i-1;
 }
 return i;
}

char* macrobindingst::GetDisplayName(char* Name) {
 strcpy(Name,displayname.c_str());
 if (!*Name) sprintf(Name,"Macro %i", BindID);
 return Name;
}

int macrobindingst::AdjustRepeat(int pos, int Add) {
 int i=coms.size();
 if (pos<i) {
  MacroCommand com=coms[pos];
  unsigned char rep=com.Repeat;
  rep+=Add;
  if (!rep) com.Repeat=1;
  else com.Repeat=rep;
  coms[pos]=com;
  return rep;
 }
 return 0;
}

interfacekeyst::interfacekeyst() {
 currentBinding=0;
 currentKey.flags=0xFF;
 currentAlt.flags=0xFF;
 currentInput=0;
 bindings.reserve(1);
 macros.reserve(1);
 macrostack.reserve(1);
 RunningMacro=0;
}

interfacekeyst::~interfacekeyst() {
 unsigned int i=bindings.size();
 while (i) {
  --i;
  if (bindings[i]) delete bindings[i];
 }
 i=macros.size();
 while (i) {
  --i;
  if (macros[i]) delete macros[i];
 }
 i=macrostack.size();
 while (i) {
  --i;
  if (macrostack[i]) delete macrostack[i];
 }
 bindings.clear();
 macros.clear();
 macrostack.clear();
}

char* ConvertKeyToDisplay(char* Str, KeyUnion Key) {
 *Str=0;
 if (Key.flags&KEY_EVENTFLAG) return Str;
 if (Key.mods&KEY_CTRLFLAG) Str=stpcpy(Str,"Ctrl+");
 if (Key.mods&KEY_ALTFLAG) Str=stpcpy(Str,"Alt+");
 if (Key.mods&KEY_METAFLAG) Str=stpcpy(Str,"Meta+");
 if (Key.mods&KEY_SUPERFLAG) Str=stpcpy(Str,"Super+");
 if (Key.mods&KEY_SHIFTFLAG) Str=stpcpy(Str,"Shift+");
 if (Key.flags&KEY_UNICODEFLAG) {
  //this and the mods stuff will have to change to a wide char output
  //when we have display support for language fonts
  if (Key.symbol>255) {
   Str+=sprintf(Str,"0x%x",Key.symbol);
   return Str;
  }
 }
 if ((Key.symbol>32)&&(Key.symbol<127)) {
  *Str=Key.symbol;
  *(++Str)=0;
  return Str;
 }
 if ((Key.symbol>=SDLK_KP0)&&(Key.symbol<=SDLK_KP9)) {
  Str=stpcpy(Str,"Numpad ");
  *Str='0'+Key.symbol-SDLK_KP0;
  *(++Str)=0;
  return Str;
 }
 if ((Key.symbol>=SDLK_F1)&&(Key.symbol<=SDLK_F15)) {
  Str+=sprintf(Str,"F%i",Key.symbol-SDLK_F1+1);
  return Str;
 }
 if (Key.symbol>KEY_MOUSEDOWN) {
  if (Key.symbol<=KEY_MOUSEUP) Str+=sprintf(Str,"Mouse D%i",Key.symbol-KEY_MOUSEDOWN);
  else if (Key.symbol<=KEY_MOUSELAST) Str+=sprintf(Str,"Mouse U%i",Key.symbol-KEY_MOUSEUP);
  return Str;
 }
 switch (Key.symbol) {
  case 0: Str=stpcpy(Str,"None");break;
  case SDLK_BACKSPACE: Str=stpcpy(Str,"Backspace");break;
  case SDLK_TAB: Str=stpcpy(Str,"Tab");break;
  case SDLK_CLEAR: Str=stpcpy(Str,"Clear");break;
  case SDLK_RETURN: Str=stpcpy(Str,"Enter");break;
  case SDLK_PAUSE: Str=stpcpy(Str,"Pause");break;
  case SDLK_ESCAPE: Str=stpcpy(Str,"Escape");break;
  case SDLK_SPACE: Str=stpcpy(Str,"Space");break;
  case SDLK_DELETE: Str=stpcpy(Str,"Delete");break;
  case SDLK_UP: Str=stpcpy(Str,"Up");break;
  case SDLK_DOWN: Str=stpcpy(Str,"Down");break;
  case SDLK_RIGHT: Str=stpcpy(Str,"Right");break;
  case SDLK_LEFT: Str=stpcpy(Str,"Left");break;
  case SDLK_INSERT: Str=stpcpy(Str,"Insert");break;
  case SDLK_HOME: Str=stpcpy(Str,"Home");break;
  case SDLK_END: Str=stpcpy(Str,"End");break;
  case SDLK_PAGEUP: Str=stpcpy(Str,"Page Up");break;
  case SDLK_PAGEDOWN: Str=stpcpy(Str,"Page Down");break;
  case SDLK_MODE: Str=stpcpy(Str,"Mode");break;
  case SDLK_COMPOSE: Str=stpcpy(Str,"Compose");break;
  case SDLK_HELP: Str=stpcpy(Str,"Help");break;
  case SDLK_PRINT: Str=stpcpy(Str,"Print Screen");break;
  case SDLK_SYSREQ: Str=stpcpy(Str,"Sys Request");break;
  case SDLK_BREAK: Str=stpcpy(Str,"Break");break;
  case SDLK_MENU: Str=stpcpy(Str,"Menu");break;
  case SDLK_POWER: Str=stpcpy(Str,"Power");break;
  case SDLK_EURO: Str=stpcpy(Str,"Euro");break;
  case SDLK_UNDO: Str=stpcpy(Str,"Undo");break;
  case SDLK_KP_MINUS: Str=stpcpy(Str,"Subtract");break;
  case SDLK_KP_PLUS: Str=stpcpy(Str,"Add");break;
  case SDLK_KP_MULTIPLY: Str=stpcpy(Str,"Multiply");break;
  case SDLK_KP_PERIOD: Str=stpcpy(Str,"Decimal");break;
  case SDLK_KP_DIVIDE: Str=stpcpy(Str,"Divide");break;
  case SDLK_KP_ENTER: Str=stpcpy(Str,"Numpad Enter");break;
  case SDLK_KP_EQUALS: Str=stpcpy(Str,"Numpad Equals");break;
  default:
   if ((Key.symbol<255)&&(Key.symbol>SDLK_WORLD_0)) {
    *Str=Key.symbol;
    *(++Str)=0;
   } else Str+=sprintf(Str,"0x%x",Key.symbol);
   break;
 }
 return Str;
}

char* ConvertKeyToSave(char* Str, KeyUnion Key) {
 *Str=0;
 switch (Key.symbol) {
  case SDLK_COLON:
  case SDLK_RIGHTBRACKET:
   if (Key.mods&KEY_SHIFTFLAG) Str=stpcpy(Str,"Shift+");
   if (Key.mods&KEY_CTRLFLAG) Str=stpcpy(Str,"Ctrl+");
   if (Key.mods&KEY_ALTFLAG) Str=stpcpy(Str,"Alt+");
   if (Key.mods&KEY_METAFLAG) Str=stpcpy(Str,"Meta+");
   if (Key.mods&KEY_SUPERFLAG) Str=stpcpy(Str,"Super+");
   if (Key.symbol==SDLK_COLON) return stpcpy(Str,"Colon");
   else return stpcpy(Str,"RBracket");
  default: return ConvertKeyToDisplay(Str,Key);
 }
}

void ConvertSaveToKey(char* Str, KeyUnion* Key) {
 Key->symbol=0;
 //don't 0 all, the calling function has to tell us whether we are doing
 //unicode or keysym handling
 int len=strlen(Str);
 while (len) {
  int count=0;
  if (len==1) { //no modifiers recorded, but we may have to expand some
   unsigned char S=*((unsigned char*)Str);
   if (S>32) { //first single character part we recognize
    if (S<127) Key->symbol=S;
    //void range is 127 to 159, DELETE is mapped at 127
    else if (S>SDLK_WORLD_0) Key->symbol=S;
   }
   return;
  } else if (len<=3) { //could be a function key
   if ((*Str|32)=='f') {
    if (Key->flags&KEY_UNICODEFLAG) return;
    if (len==2) count=Str[1]-'1';
    else count=(Str[1]-'0')*10+(Str[2]-'1');
    if (count<=15) Key->symbol=SDLK_F1+count;
   } else if ((Str[0]=='0')&&(Str[1]=='x')) { //hex code
    //only 1 character available
    count=Str[2]&0xDF; //strip to upper case
    if (count>64) {
     count-=55; //'A'-55 gives 10
     if (count>15) return; //value is no good
    } else {
     count-=16; //'0' is 48, stripped of the 32 bit leaves 16
     if (count>9) return;
    }
    Key->symbol=count; //a 0 result becomes a failure elsewhere
    return;
   } else {
    if (Key->flags&KEY_KEYSYMFLAG) {
     if (stricmp(Str,"Add")==0) {Key->symbol=SDLK_KP_PLUS;return;}
     if (stricmp(Str,"Up")==0) {Key->symbol=SDLK_UP;return;}
    }
    if (stricmp(Str,"End")==0) {Key->symbol=SDLK_END;return;}
    if (stricmp(Str,"Tab")==0) {Key->symbol=SDLK_TAB;return;}
   }
   return;
  } else {
   //start by looking for a +
   while (Str[count]) {
    if (Str[count]=='+') {
     switch (*Str|32) {
      case 'a': if (strnicmp(Str,"Alt+",count)==0) Key->mods|=KEY_ALTFLAG;break;
      case 'c': if (strnicmp(Str,"Ctrl+",count)==0) Key->mods|=KEY_CTRLFLAG;break;
      case 'm': if (strnicmp(Str,"Meta+",count)==0) Key->mods|=KEY_METAFLAG;break;
      case 's': {
       if (strnicmp(Str,"Shift+",count)==0) Key->mods|=KEY_SHIFTFLAG;
       else if (strnicmp(Str,"Super+",count)==0) Key->mods|=KEY_SUPERFLAG;
       break;
      }
     }
     ++count;
     len-=count;
     Str+=count;
     break; //while Str[count]
    }
    ++count;
   } // + scan
   if (strnicmp(Str,"Mouse ",6)==0) {
    if (Key->flags&KEY_UNICODEFLAG) return; //disallowed
    Str+=6;
    len-=7;
    if (len==1) count=Str[1]-'0';
    else count=(Str[1]-'0')*10+(Str[2]-'0');
    if ((count<=NUM_MOUSE_BUTTONS)&&(count>0)) {
     if ((*Str|32)=='d') Key->symbol=KEY_MOUSEDOWN+count;
     else if ((*Str|32)=='u') Key->symbol=KEY_MOUSEUP+count;
    }
    return;
   }
   if ((Str[0]=='0')&&(Str[1]=='x')) { //hex code
    count=strtol(Str,0,16);
    if (!count) return; //negative invalid
    if (Key->flags&KEY_KEYSYMFLAG) {
     if (count>255) return;
    } else if (count>0xFFFF) return;
    Key->symbol=count;
    return;
   }
   if (len<=3) continue; //covers a scan finish
   switch (len) {
    case 4:
     if (Key->flags&KEY_UNICODEFLAG) return;
     switch (*Str|32) {
      case 'd': if (stricmp(Str,"Down")==0) Key->symbol=SDLK_DOWN;break;
      case 'e': if (stricmp(Str,"Euro")==0) Key->symbol=SDLK_EURO;break;
      case 'h':
       if (stricmp(Str,"Help")==0) Key->symbol=SDLK_HELP;
       if (stricmp(Str,"Home")==0) Key->symbol=SDLK_HOME;
      break;
      case 'l': if (stricmp(Str,"Left")==0) Key->symbol=SDLK_LEFT;break;
      case 'm':
       if (stricmp(Str,"Mode")==0) Key->symbol=SDLK_MODE;
       if (stricmp(Str,"Menu")==0) Key->symbol=SDLK_MENU;
      break;
//      case 'n': if (stricmp(Str,"None")==0) Key->symbol=0;break;
      case 'u': if (stricmp(Str,"Undo")==0) Key->symbol=SDLK_UNDO;break;
     }
    break; //switch len
    case 5:
     if (stricmp(Str,"Colon")==0) Key->symbol=SDLK_COLON;
     else if (stricmp(Str,"Space")==0) Key->symbol=SDLK_SPACE;
     else if (stricmp(Str,"Enter")==0) Key->symbol=SDLK_RETURN;
     if (Key->flags&KEY_UNICODEFLAG) return;
     switch (*Str|32) {
      case 'b': if (stricmp(Str,"Break")==0) Key->symbol=SDLK_BREAK;break;
      case 'c': if (stricmp(Str,"Clear")==0) Key->symbol=SDLK_CLEAR;break;
      case 'p':
       if (stricmp(Str,"Pause")==0) Key->symbol=SDLK_PAUSE;
       if (stricmp(Str,"Power")==0) Key->symbol=SDLK_POWER;
      break;
      case 'r': if (stricmp(Str,"Right")==0) Key->symbol=SDLK_RIGHT;break;
     }
    break; //switch len
    case 6:
     if (stricmp(Str,"Delete")==0) Key->symbol=SDLK_DELETE;
     else if (stricmp(Str,"Escape")==0) Key->symbol=SDLK_ESCAPE;
     if (Key->flags&KEY_UNICODEFLAG) return;
     if (stricmp(Str,"Divide")==0) Key->symbol=SDLK_KP_DIVIDE;
     else if (stricmp(Str,"Insert")==0) Key->symbol=SDLK_INSERT;
    break; //switch len
    case 7:
     if (Key->flags&KEY_UNICODEFLAG) return;
     if (stricmp(Str,"Compose")==0) Key->symbol=SDLK_COMPOSE;
     else if (stricmp(Str,"Decimal")==0) Key->symbol=SDLK_KP_PERIOD;
     else if (stricmp(Str,"Page Up")==0) Key->symbol=SDLK_PAGEUP;
    break; //switch len
    case 8: case 9:
     if (stricmp(Str,"Backspace")==0) Key->symbol=SDLK_BACKSPACE;
     else if (stricmp(Str,"RBracket")==0) Key->symbol=SDLK_RIGHTBRACKET;
     if (Key->flags&KEY_UNICODEFLAG) return;
     switch (*Str|32) {
      case 'm': if (stricmp(Str,"Multiply")==0) Key->symbol=SDLK_KP_MULTIPLY;break;
      case 'n': if (strnicmp(Str,"Numpad ",7)==0) Key->symbol=Str[7]+SDLK_KP0-'0';break;
      case 'p': if (stricmp(Str,"Page Down")==0) Key->symbol=SDLK_PAGEDOWN;break;
      case 's': if (stricmp(Str,"Subtract")==0) Key->symbol=SDLK_KP_MINUS;break;
     }
    break; //switch len
    default:
     if (Key->flags&KEY_UNICODEFLAG) return;
     switch (*Str|32) {
      case 'n':
       if (stricmp(Str,"Numpad Enter")==0) Key->symbol=SDLK_KP_ENTER;
       if (stricmp(Str,"Numpad Equals")==0) Key->symbol=SDLK_KP_EQUALS;
      break;
      case 'p': if (stricmp(Str,"Print Screen")==0) Key->symbol=SDLK_PRINT;break;
      case 's': if (stricmp(Str,"Sys Request")==0) Key->symbol=SDLK_SYSREQ;break;
     }
    break; //switch len
   } //switch len
   return;
  } //else len>3
 } //while len
}

macrobindingst* interfacekeyst::AddMacro(char* DisplayName) {
 int i=macros.size();
 macrobindingst* Res=new macrobindingst(i);
 macros.push_back(Res);
 if (DisplayName) Res->ChangeDisplayName(DisplayName);
 return Res;
}

int interfacekeyst::MatchBindingName(const char* Name) {
 int i=1;
 while (i<INTERFACEKEYNUM) {
  if (strcmp(Name,InterfaceNames[i].token)) ++i;
  else return i;
 }
 return -1;
}

int interfacekeyst::MatchMacroName(const char* Name) {
 int i=macros.size();
 int j=0;
 if (strncmp(Name,"MACRO",5)==0) {
  Name+=5;
  while (*Name) {
   j*=10;
   if (*Name<'0') return -1;
   if (*Name>'9') return -1;
   j+=*Name-'0';
   ++Name;
  }
  return j;
 }
 return -1;
}

void interfacekeyst::Load(const char* filename) {
 int Binding=bindings.size();
 int count=0;
 //make sure things are clear, zeroed, and that the vector has set its size correctly
 while (count<INTERFACEKEYNUM) {
  if (Binding<INTERFACEKEYNUM) {
   bindings.push_back(0);
   ++Binding;
  }
  if (bindings[count]) {
   bindings[count]->Clear();
  }
  ++count;
 }
 //completely remove macros
 Binding=macros.size();
 while (Binding) {
  --Binding;
  if (macros[Binding]) delete macros[Binding];
 }
 macros.clear();
 Binding=macrostack.size();
 while (Binding) {
  --Binding;
  if (macrostack[Binding]) delete macrostack[Binding];
 }
 macrostack.clear();
 Binding=-1;
 std::ifstream fseed(filename);
 string str;
 char cstr[256];
 char BindType=0;
 svector<char*> Tokens;
 if(fseed.is_open()) {
  while(std::getline(fseed,str)) {
   strcpy(cstr,str.c_str());
   count=TokenizeLine(Tokens,cstr);
   if (count>1) {
    int dir=strcmp(Tokens[0],"KEY");
    if (dir==0) {
     if (Binding>-1) {
      KeyUnion Key;
      Key.Value=0;
      Key.flags=KEY_UNICODEFLAG;
      ConvertSaveToKey(Tokens[1],&Key);
      if (BindType) macros[Binding]->AddKey(Key);
      else bindings[Binding]->AddKey(Key);
     }
    }
    else if (dir>0) {
     if (Binding>-1) {
      if (strcmp("SYM",Tokens[0])==0) {
        KeyUnion Key;
        Key.Value=0;
        Key.flags=KEY_KEYSYMFLAG;
        ConvertSaveToKey(Tokens[1],&Key);
        if (BindType) macros[Binding]->AddKey(Key);
        else bindings[Binding]->AddKey(Key);
      } else if (strcmp("MACRO",Tokens[0])==0) {
       MacroCommand com;
       com.Value=0;
       if (strcmp("DELAY",Tokens[1])==0) {
        if (count==3) {
         dir=atoi(Tokens[2]);
         if (dir<1) com.Repeat=1;
         else if (dir>255) com.Repeat=255;
         else com.Repeat=dir;
         macros[Binding]->AddCommand(com);
        }
       } else {
        if (count==3) {
         dir=atoi(Tokens[2]);
         if (dir>255) com.Repeat=255;
         else com.Repeat=dir;
        } else com.Repeat=1;
        dir=MatchBindingName(Tokens[1]);
        if (dir==-1) {
         dir=MatchMacroName(Tokens[1]);
         if (dir!=-1) {
          com.flags=1;
          com.Binding=dir;
         } else com.Value=0;
        } else com.Binding=dir;
        if (com.Value) macros[Binding]->AddCommand(com);
       } //else macro command
      } //match macro
     } //in binding
    } else if (strcmp("BIND",Tokens[0])==0) {
     Binding=MatchBindingName(Tokens[1]);
     if (Binding!=-1) {
      BindType=0;
      if (!bindings[Binding]) bindings[Binding]=new keybindingst(Binding);
      if ((count==3)&&(*Tokens[2])) bindings[Binding]->ChangeDisplayName(Tokens[2]);
     } else {
      Binding=MatchMacroName(Tokens[1]);
      if (Binding!=-1) {
       BindType=1;
       int i=Binding-macros.size();
       while (i>0) {
        AddMacro();
        --i;
       }
       if (count==3) AddMacro(Tokens[2]);
       else AddMacro();
      }
     }
    }
   } //if count>1
  } //while
  fseed.close();
 }
 if (!bindings[INTERFACEKEY_OPTIONS])
  bindings[INTERFACEKEY_OPTIONS]= new keybindingst(INTERFACEKEY_OPTIONS);
 KeyUnion key;
 key.symbol=INTERFACEEVENT_QUIT;
 key.flags=KEY_EVENTFLAG;
 key.mods=0;
 bindings[INTERFACEKEY_OPTIONS]->AddKey(key);
}

void interfacekeyst::Save(const char* filename) {
 std::ofstream fseed(filename);
 if(fseed.is_open()) {
  int count=bindings.size()+macros.size();
  int binding=1;
  keybindingst* Bind;
  while (binding<count) {
   if (binding<INTERFACEKEYNUM) Bind=bindings[binding];
   else Bind=macros[binding-INTERFACEKEYNUM];
   if (Bind) {
    int keys=Bind->GetKeyCount();
    int coms=Bind->GetComCount();
    char savekey[128];
    //have some data for this one
    if (binding>=INTERFACEKEYNUM) {
     sprintf(savekey,"%i",binding-INTERFACEKEYNUM);
     fseed<<"[BIND:MACRO"<<savekey;
    } else fseed<<"[BIND:"<<InterfaceNames[binding].token;
    if (*(Bind->displayname.c_str())) fseed<<':'<<Bind->displayname<<']'<<std::endl;
    else fseed<<']'<<std::endl;
    //bound keys next
    if (keys) {
     int i=0;
     KeyUnion* key=Bind->GetKeyArray();
     do {
      //events drop back quickly and return the original pointer
      if (ConvertKeyToSave(savekey,*key)!=savekey) {
       if (key->flags&KEY_UNICODEFLAG) fseed<<"[KEY:";
       else fseed<<"[SYM:";
       fseed<<savekey<<']'<<std::endl;
      }
      ++i;
      ++key;
     } while (i<keys);
    }
    //now the macro data
    if (coms) {
     int i=0;
     MacroCommand* com=Bind->GetComArray();
     do {
      fseed<<"[MACRO:";
      if (com->flags) { //only flag so far is macro/keybinding
       sprintf(savekey,"%i",com->Binding);
       fseed<<"MACRO"<<savekey;
       sprintf(savekey,":%i]",com->Repeat);
       fseed<<savekey<<std::endl;
      } else {
       sprintf(savekey,":%i]",com->Repeat);
       if (com->Binding) fseed<<InterfaceNames[com->Binding].token<<savekey<<std::endl;
       else fseed<<"DELAY"<<savekey<<std::endl;
      }
      ++i;
      ++com;
     } while (i<coms);
    }
   } //if have data
   ++binding;
  } //while binding<count
  fseed.close();
 } //if isopen
}

MacroResult macrostackst::DoMacro(unsigned int now) {
 MacroResult Res;
 Res.Value=0;
 if ((now<overflow)&&(now>=nextproc)) {
  //do macro
  repeat++;
  if ((int)curCom.Repeat>=repeat) {
   //same command again
   nextproc=now+init.input.macro_time;
   if (nextproc<now) overflow=now-1;
   else overflow=0xFFFFFFFF;
   Res.bind=curCom.Binding;
   if (!curCom.flags) {
    Res.action=MACROACTION_EXECUTE;
    return Res;
   }
   //else got another macro
   if (curCom.Binding!=macroID) {
    Res.action=MACROACTION_NEW;
   } else { //same macro, reset to start
    position=0;
    repeat=0;
    Res.action=MACROACTION_RECALL;
   }
   return Res;
  }
  if (position==comCount) {
   //hit the end jump up a level on the macro stack
   Res.action=MACROACTION_DONE;
   return Res;
  }
  curCom.Value=coms[position].Value;
  if (!curCom.flags) { //binding or delay
   if (curCom.Binding) { //binding
    Res.bind=curCom.Binding;
    Res.action=MACROACTION_EXECUTE;
    nextproc=now+init.input.macro_time;
    if (nextproc<now) overflow=now-1;
    else overflow=0xFFFFFFFF;
    repeat=1;
    position++;
   } else { //delay
    nextproc=now+curCom.Repeat;
    if (nextproc<now) overflow=now-1;
    else overflow=0xFFFFFFFF;
    //time set, max repeat so it will move on
    repeat=255;
    Res.action=MACROACTION_WAIT;
   }
   return Res;
  } //else got another macro
  Res.bind=curCom.Binding;
  if (curCom.Binding!=macroID) {
   repeat=1;
   position++;
   Res.action=MACROACTION_NEW;
  } else { //same macro, reset to start
   position=0;
   repeat=0;
   Res.action=MACROACTION_RECALL;
  }
 } else { //in macro but it is waiting
  Res.action=MACROACTION_WAIT;
 }
 return Res;
}

// Work functions
int interfacekeyst::RunMacros(int count) {
 macrostackst* Cur=macrostack[count];
 do {
  MacroResult Res=Cur->DoMacro(enabler.now);
  switch (Res.action) {
   case MACROACTION_WAIT: return 0;
   case MACROACTION_EXECUTE: return currentBinding=Res.bind;
   case MACROACTION_NEW:
    RunningMacro=1;
    Cur=new macrostackst(&(macros[Res.bind]->coms[0]),Res.bind,macros[Res.bind]->coms.size());
    macrostack.push_back(Cur);
    count++;
   break;
   case MACROACTION_DONE:
    macrostack.pop_back();
    count--;
    if (count>=0) Cur=macrostack[count];
    else {
     RunningMacro=0;
     return -1;
    }
   break;
   case MACROACTION_RECALL: break;
   default: {
    errorlog_string("Macro attempted illegal action");
    count=macrostack.size();
    while (count) {
     --count;
     delete macrostack[count];
    }
    macrostack.clear();
    return 0;
   }
  }
 } while (count>=0); //still doing the macro
}

int interfacekeyst::keynext() {
 //first check for an active macro
 int count=macrostack.size();
 int enCount=enabler.inputcount();
 InputRec* input;
 if (count) {
  //running macro, have to check for the macro break binding before we go with it
  //go from latest to oldest
  int i=enCount;
  currentBinding=0;
  currentInput=0;
  while (i) {
   --i;
   input=enabler.getinput(i);
   currentKey.Value=input->key.Value;
   currentAlt.Value=input->key2.Value;
   if (keypress(INTERFACEKEY_MACRO_BREAK)) {
    currentKey.flags=0xFF;
    currentAlt.flags=0xFF;
    currentBinding=0;
    RunningMacro=0;
    while (count) {
     --count;
     delete macrostack[count];
    }
    macrostack.clear();
    enabler.clear_input();
    return 0;
   }
  }
  if (enCount) {
   enabler.clear_input();
   enCount=0;
  }
  if (count) {
   currentKey.flags=0xFF;
   currentAlt.flags=0xFF;
   count=RunMacros(--count);
   if (count>=0) return count;
   else return 0;
  } //if macrostack still has something
 } //if count of macro stack
 //check for key press
 int i=0;
 if (enCount) input=enabler.currentinput(enabler.now);
 else return 0;
 if (input) {
  currentInput=input;
  currentKey.Value=input->key.Value;
  currentAlt.Value=input->key2.Value;
  //check to see if this is a macro
  i=0;
  count=macros.size();
  while (i<count) {
   macrobindingst* Bind=macros[i];
   int keyCount=Bind->keys.size();
   switch (keyCount) {
    case 0: break; //no keys, no check
    case 1:
     if ((Bind->keys[0].Value==currentKey.Value)||(Bind->keys[0].Value==currentAlt.Value)) {
      macrostack.push_back(new macrostackst(&(Bind->coms[0]),i,Bind->coms.size()));
      //safe to figure this is the first in the stack
      keydone();
      currentKey.flags=0xFF;
      currentAlt.flags=0xFF;
      int j;
      if ((j=RunMacros(0))>=0) { //macro took it
       RunningMacro=1;
       return j;
      } else return 0;
     }
    break;
    default:
     if (!(Bind->KeyAccum&currentKey.Value)) {
      //don't have any bits that aren't amongst the keys do loop checking
      int j=0;
      do {
       if (currentKey.Value==Bind->keys[j].Value) {
        macrostack.push_back(new macrostackst(&(Bind->coms[0]),i,Bind->coms.size()));
        keydone();
        currentKey.flags=0xFF;
        currentAlt.flags=0xFF;
        if ((j=RunMacros(0))>=0) {
         RunningMacro=1;
         return j;
        } else return 0;
       }
       ++j;
      } while (j<keyCount);
     }
     if (!(Bind->KeyAccum&currentAlt.Value)) {
      int j=0;
      do {
       if (currentAlt.Value==Bind->keys[j].Value) {
        macrostack.push_back(new macrostackst(&(Bind->coms[0]),i,Bind->coms.size()));
        keydone();
        currentKey.flags=0xFF;
        currentAlt.flags=0xFF;
        if ((j=RunMacros(0))>=0) {
         RunningMacro=1;
         return j;
        } else return 0;
       }
       ++j;
      } while (j<keyCount);
     }
    break;
   } //switch keycount
   ++i;
  } //while macros
  return currentKey.Value;
 } //if had a key waiting
 return 0;
}

int interfacekeyst::keypress(int KeyNo) {
 if (currentBinding) {
  if (currentBinding!=KeyNo) return 0;
  else return KeyNo;
 }
 if (currentKey.flags==0xFF) return 0;
 //this is for the key bindings view so it can claim keypresses away from everything
 switch (KeyNo) {
  case KEY_BIND_SCAN:
   if (!(currentKey.flags&KEY_EVENTFLAG)) {
    currentBinding=KEY_BIND_SCAN;
    return currentKey.Value;
   } else return 0;
  case KEY_BIND_UNICODE:
   if (currentAlt.Value) {
    currentBinding=KEY_BIND_UNICODE;
    return currentAlt.Value;
   } else return 0;
  default: break;
 }
 if (KeyNo>INTERFACEKEYNUM) return 0;
 keybindingst* Bind=bindings[KeyNo];
 if (Bind==0) return 0;
 int keyCount=Bind->keys.size();
 switch (keyCount) {
  case 0: return 0;
  case 1:
   if ((Bind->keys[0].Value==currentKey.Value)||(Bind->keys[0].Value==currentAlt.Value)) {
    currentKey.flags=0xFF;
    currentAlt.flags=0xFF;
    return currentBinding=KeyNo;
   }
  return 0;
  default:
   if (!(Bind->KeyAccum&currentKey.Value)) {
    //don't have any bits that aren't amongst the keys do loop checking
    int j=0;
    do {
     if (currentKey.Value==Bind->keys[j].Value) {
      currentKey.flags=0xFF;
      currentAlt.flags=0xFF;
      return currentBinding=KeyNo;
     }
     ++j;
    } while (j<keyCount);
   }
   if (!(Bind->KeyAccum&currentAlt.Value)) {
    int j=0;
    do {
     if (currentAlt.Value==Bind->keys[j].Value) {
      currentKey.flags=0xFF;
      currentAlt.flags=0xFF;
      return currentBinding=KeyNo;
     }
     ++j;
    } while (j<keyCount);
   }
  return 0;
 } //switch keycount
}

int interfacekeyst::pressedList(const int* KeyList, int ListSize) {
 int i=0;
 if (currentBinding) {
  while (i<ListSize) {
   if (currentBinding==KeyList[i]) return currentBinding;
   ++i;
  }
  return 0;
 }
 if (currentKey.flags==0xFF) return 0;
 keybindingst* Bind;
 while (i<ListSize) {
  if (KeyList[i]<INTERFACEKEYNUM) {
   if (KeyList[i]>0) Bind=bindings[KeyList[i]];
   else Bind=0;
  } else Bind=0;
  if (Bind) {
   int keyCount=Bind->keys.size();
   switch (keyCount) {
    case 0: break;
    case 1:
     if ((Bind->keys[0].Value==currentKey.Value)||(Bind->keys[0].Value==currentAlt.Value)) {
      currentKey.flags=0xFF;
      currentAlt.flags=0xFF;
      return currentBinding=KeyList[i];
     }
    break;
    default:
     if (!(Bind->KeyAccum&currentKey.Value)) {
      int j=0;
      do {
       if (currentKey.Value==Bind->keys[j].Value) {
        currentKey.flags=0xFF;
        currentAlt.flags=0xFF;
        return currentBinding=KeyList[i];
       }
       ++j;
      } while (j<keyCount);
     }
     if (!(Bind->KeyAccum&currentAlt.Value)) {
      int j=0;
      do {
       if (currentAlt.Value==Bind->keys[j].Value) {
        currentKey.flags=0xFF;
        currentAlt.flags=0xFF;
        return currentBinding=KeyList[i];
       }
       ++j;
      } while (j<keyCount);
     }
   } //switch keycount
  } //if Bind
  ++i;
 }
 return 0;
}

void interfacekeyst::keydone() {
 if (currentInput==0) {
  currentBinding=0; //macro may have set the binding
  return;
 }
 int i=init.input.repeat_time;
 InputRec* Input=currentInput;
 currentInput=0;
 if (Input->processed) Input->next_process=enabler.now+i; //already done once
 else {
  if (currentBinding==0) { //it was looked at and the view didn't want it
   Input->processed=1;
   Input->next_process=0xFFFFFFFF;
   //process it again never
   currentBinding=0;
   return;
  }
  i=init.input.hold_time;
  if ((currentBinding<INTERFACEKEY_STANDARDSCROLL_UP)||
   (currentBinding>INTERFACEKEY_CURSOR_DOWN_Z_AUX)) i*=3;
  Input->next_process=enabler.now+i;
 }
 if (Input->next_process<enabler.now) Input->processed=enabler.now-1;
 else Input->processed=0xFFFFFFFF;
 currentBinding=0;
}

keybindingst* interfacekeyst::GetBinding(int Binding) {
 if (Binding>=INTERFACEKEYNUM) {
  int i=Binding-INTERFACEKEYNUM;
  if (i<macros.size()) return macros[i];
  return 0;
 } else return bindings[Binding];
}

const int FTabKey[]={INTERFACEKEY_CHANGETAB};
const int RTabKey[]={INTERFACEKEY_REVERSETAB};
const int DownKey[]={INTERFACEKEY_STANDARDSCROLL_DOWN};
const int UpKey[]={INTERFACEKEY_STANDARDSCROLL_UP};
const int PgDnKey[]={
 INTERFACEKEY_STANDARDSCROLL_PAGEDOWN,
 INTERFACEKEY_CURSOR_DOWN_FAST
};
const int PgUpKey[]={
 INTERFACEKEY_STANDARDSCROLL_PAGEUP,
 INTERFACEKEY_CURSOR_UP_FAST
};

void viewscreen_keybindingsst::SetScrollKeys(ScrollListWidget* List) {
 List->UpKeyList=UpKey;
 List->UKeySize=1;
 List->DownKeyList=DownKey;
 List->DKeySize=1;
 List->PgUpKeyList=PgUpKey;
 List->PUKeySize=2;
 List->PgDnKeyList=PgDnKey;
 List->PDKeySize=2;
}

void viewscreen_keybindingsst::UpdateBinding() {
 if (DoBind) {
  curBind=gview.GetBinding(DoBind);
  if (curBind) {
   curKeys=curBind->GetKeyArray();
   if (curKeys) Keys->SetListCount(curBind->GetKeyCount()+2);
   else Keys->SetListCount(2);
   curComs=curBind->GetComArray();
  } else {
   curKeys=0;
   Keys->SetListCount(2);
   curComs=0;
  }
 } else {
  curBind=0;
  curKeys=0;
  curComs=0;
  Keys->SetListCount(0); //selector is at New Macro
 }
}

viewscreen_keybindingsst::viewscreen_keybindingsst() {
 DoBind=BindCats[0].ofset;
 OnSetting=Mode=0;
 short x=init.display.grid_x-1;
 short y=init.display.grid_y-2;
 Tabs=new TabBarWidget(1,x,1,1);
 Tabs->ForwardKeyList=FTabKey;
 Tabs->ReverseKeyList=RTabKey;
 Tabs->FKeySize=Tabs->RKeySize=1;
 Tabs->HasFocus=1;
 Tabs->ListSrc=this;
 Tabs->AdjustSpacing(12);
 Tabs->SetListCount(BINDCATSNUM);
 Bindings=new ScrollListWidget(1,x*2/3,3,y);
 Bindings->ListSrc=this;
 Bindings->HasFocus=1;
 SetScrollKeys(Bindings);
 Bindings->SetListCount(BindCats[0].count);
 Keys=new ScrollListWidget(x*2/3+1,x,3,y);
 Keys->ListSrc=this;
 SetScrollKeys(Keys);
 Commands=new ScrollListWidget(x>>1,x,3,y);
 Commands->ListSrc=this;
 SetScrollKeys(Commands);
 UpdateBinding();
}

viewscreen_keybindingsst::~viewscreen_keybindingsst() {
 delete Tabs;
 delete Bindings;
 delete Keys;
 delete Commands;
}

viewscreen_keybindingsst* viewscreen_keybindingsst::create(char pushtype,viewscreenst *scr) {
 viewscreen_keybindingsst *newv=new viewscreen_keybindingsst();
 gview.addscreen(newv,pushtype,scr);
 enabler.flag&=~ENABLERFLAG_MAXFPS;
 enabler.qprate=init.display.dwarf_frame_rate;
#ifndef NO_FMOD
 if(gamemode==GAMEMODE_DWARF)musicsound.startbackgroundmusic(SONG_GAME);
#endif
 return newv;
}

void viewscreen_keybindingsst::input() {
 const int KeyList[]={
  INTERFACEKEY_SECONDSCROLL_UP,
  INTERFACEKEY_SECONDSCROLL_DOWN,
  INTERFACEKEY_SECONDSCROLL_PAGEUP,
  INTERFACEKEY_SECONDSCROLL_PAGEDOWN,
  INTERFACEKEY_STANDARDSCROLL_LEFT,
  INTERFACEKEY_STANDARDSCROLL_RIGHT,
  INTERFACEKEY_SELECT,
  INTERFACEKEY_SAVE_BINDINGS,
  INTERFACEKEY_LOAD_BINDINGS,
  INTERFACEKEY_LEAVESCREEN,
  INTERFACEKEY_LEAVESCREEN_ALL,
 };
 enabler.flag|=ENABLERFLAG_RENDER;
 //this gets a bit messy having 3 different lists and a few different modes
 //Mode 0 standard display binding names on left, keys on right
 //Mode 1 Adding a key to the selected binding
 //Mode 2 Locks binding display to macros list, and shows commands on right
 //Mode 3 Adding a command to the macro, unlocks bindings for user selection
 int i;
 if (Mode==1) {
  KeyUnion keyval;
  if (Keys->selected==0) keyval.Value=gview.keypress(KEY_BIND_UNICODE);
  else keyval.Value=gview.keypress(KEY_BIND_SCAN);
  if (keyval.symbol) {
   i=curBind->AddKey(keyval);
   UpdateBinding(); //have to call this incase of realloc of the key vector
   Keys->SetListCount(i+2);
   Mode=0;
  }
  return;
 }
 if (Tabs->input()) {
  OnSetting=0;
  Bindings->HasFocus=1;
  Bindings->Jump(0);
  if (Tabs->selected==BINDCATSNUM-1) { //macros
   if (!Mode) {
    DoBind=0;
    UpdateBinding();
   }
   int i=gview.GetMacroCount();
   Bindings->SetListCount(i+1);
  } else {
   Bindings->SetListCount(BindCats[Tabs->selected].count);
   if (!Mode) {
    DoBind=BindCats[Tabs->selected].ofset;
    UpdateBinding();
   }
  }
  if (!Mode) {
   Keys->HasFocus=0;
   Keys->Jump(0);
  }
  return;
 }
 if (Bindings->input()) {
  OnSetting=0;
  if (!Mode) {
   DoBind=Bindings->selected;
   if (Tabs->selected==BINDCATSNUM-1) { //macros
    if (DoBind) DoBind+=BindCats[Tabs->selected].ofset-1;
   } else DoBind+=BindCats[Tabs->selected].ofset;
   UpdateBinding();
   Keys->Jump(0);
  }
  return;
 }
 if (Mode==0) { //better to check mode then make calls we don't have to
  if (Keys->input()) {
   OnSetting=(Keys->selected>1);
   return;
  }
 } else if (Commands->input()) {
  OnSetting=Commands->selected;
  return;
 }
 int Pressed=gview.pressedList(KeyList,sizeof(KeyList)/sizeof(KeyList[0]));
 switch (Pressed) {
  case INTERFACEKEY_LEAVESCREEN:
   switch (Mode) {
    case 2:
     Mode=0;
     Bindings->HasFocus=1;
     Tabs->HasFocus=1;
    return;
    case 3:
     Mode=2;
     Tabs->HasFocus=0;
     Tabs->Jump(BINDCATSNUM-1);
     Bindings->HasFocus=0;
     Bindings->SetListCount(gview.GetMacroCount()+1);
     Bindings->Jump(DoBind+1);
     Commands->HasFocus=1;
    return;
   }
   breakdownlevel=INTERFACE_BREAKDOWN_STOPSCREEN;
  break;
  case INTERFACEKEY_LEAVESCREEN_ALL:
   breakdownlevel=INTERFACE_BREAKDOWN_TOFIRST;
  break;
  case INTERFACEKEY_SAVE_BINDINGS:
   gview.Save("data/init/interface.txt");
  break;
  case INTERFACEKEY_LOAD_BINDINGS:
   gview.Load("data/init/interface.txt");
   Mode=0;
   DoBind=BindCats[0].ofset;
   Tabs->HasFocus=1;
   Tabs->Jump(0);
   Bindings->HasFocus=1;
   Bindings->Jump(0);
   Bindings->SetListCount(BindCats[0].count);
   Keys->HasFocus=0;
   Keys->Jump(0);
   UpdateBinding();
  break;
  case INTERFACEKEY_STANDARDSCROLL_LEFT:
   if (!Mode) {
    Keys->HasFocus=0;
    Bindings->HasFocus=1;
   }
  break;
  case INTERFACEKEY_STANDARDSCROLL_RIGHT:
   if (!Mode) {
    if (Keys->GetCount()) {
     Keys->HasFocus=1;
     Bindings->HasFocus=0;
    }
   }
  break;
  case INTERFACEKEY_SECONDSCROLL_UP:
   if ((Mode==2)&&(Commands->selected>0))
    ((macrobindingst*)curBind)->AdjustRepeat(Commands->selected-1, -1);
  break;
  case INTERFACEKEY_SECONDSCROLL_DOWN:
   if ((Mode==2)&&(Commands->selected>0))
    ((macrobindingst*)curBind)->AdjustRepeat(Commands->selected-1, 1);
  break;
  case INTERFACEKEY_SECONDSCROLL_PAGEUP:
   if ((Mode==2)&&(Commands->selected>0))
    ((macrobindingst*)curBind)->AdjustRepeat(Commands->selected-1, -10);
  break;
  case INTERFACEKEY_SECONDSCROLL_PAGEDOWN:
   if ((Mode==2)&&(Commands->selected>0))
    ((macrobindingst*)curBind)->AdjustRepeat(Commands->selected-1, 10);
  break;
  case INTERFACEKEY_SELECT:
   switch (Mode) {
    case 0: //normal binding view
     if (Keys->HasFocus) {
      i=Keys->selected;
      if (i>1) {
       i-=2;
       curBind->RemoveKeyPos(i);
       Keys->SetListCount(-1);
      } else Mode=1; //add key
      return;
     }
     if (Tabs->selected==BINDCATSNUM-1) { //macros
      Mode=2;
      Bindings->HasFocus=0;
      Commands->HasFocus=1;
      Tabs->HasFocus=0;
      if (!Bindings->selected) {
       gview.AddMacro();
       DoBind=gview.GetMacroCount();
       Bindings->SetListCount(DoBind+1); //+1 for AddMacro
       Bindings->Jump(DoBind+1);
       DoBind+=INTERFACEKEYNUM-1;
       UpdateBinding();
      }
      if (curComs) Commands->SetListCount(curBind->GetComCount()+1);
      else Commands->SetListCount(1);
      Commands->Jump(0);
     }
    break;
    case 2:
     i=Commands->selected;
     if (i) {
      ((macrobindingst*)curBind)->RemoveCommand(i-1);
      Commands->SetListCount(-1);
     } else { //add command to macro, set the bindings list to the beginning
      Mode=3;
      Tabs->HasFocus=1;
      Tabs->Jump(0);
      Bindings->HasFocus=1;
      Bindings->Jump(0);
      Bindings->SetListCount(BindCats[0].count);
      Commands->HasFocus=0;
     }
    break;
    case 3: { //add command mode
     Mode=2;
     i=Bindings->selected;
     Bindings->HasFocus=0;
     Bindings->SetListCount(gview.GetMacroCount()+1);
     Bindings->Jump(DoBind+1);
     Commands->HasFocus=1;
     Commands->SetListCount(Commands->GetCount()+1); //adding a command will always succeed
     MacroCommand com;
     com.Value=0;
     com.Repeat=1;
     if (Tabs->selected==BINDCATSNUM-1) { //macro
      if (i) {
       com.flags=1; //macro command
       --i;
      } else com.Repeat=100; //delay command
     } else {
      i+=BindCats[Tabs->selected].ofset;
     }
     com.Binding=i;
     Tabs->HasFocus=0;
     Tabs->Jump(BINDCATSNUM-1);
     ((macrobindingst*)curBind)->AddCommand(com);
     UpdateBinding(); //pick up any reallocs
     break;
    }
   } //switch mode
  break; //case select
 } //switch key press
}

typedef struct {
 int cat;
 int which;
} BindingSelector;

void viewscreen_keybindingsst::render() {
 char text[256];
 drawborder("  Dwarf Fortress Options: Key Bindings  ");
 BindingSelector P;
 P.cat=Tabs->selected;
 P.which=Bindings->selected;
 //captioning
 gps.changecolor(7,0,1);
 gps.locate(init.display.grid_y-2,1);
 gview.print_interface_token(INTERFACEKEY_SAVE_BINDINGS);
 gps.addst(":Save ");
 gview.print_interface_token(INTERFACEKEY_LOAD_BINDINGS);
 gps.addst(":Load ");
 if (Tabs->HasFocus) {
  gview.print_interface_token(INTERFACEKEY_CHANGETAB);
  gps.addchar(',',1);
  gview.print_interface_token(INTERFACEKEY_REVERSETAB);
  gps.addst(":Category ");
 }
 //draw panels
 Tabs->render(0);
 if (Mode==1) { //bind key
  gps.changecolor(4,0,1);
  gps.locate(init.display.grid_y-2,40);
  gps.addst("Press key to bind to ");
  gps.addst(curBind->GetDisplayName(text));
 } else {
  gps.changecolor(7,0,1);
  if (!Bindings->HasFocus) {
   if (Mode==2) gps.locate(init.display.grid_y-2,20);
   else gps.locate(init.display.grid_y-2,40);
   gview.print_interface_token(INTERFACEKEY_SELECT);
   if (OnSetting) {
    gps.addst(" - Remove");
    if (Mode==2) {
     gps.locate(init.display.grid_y-2,40);
     gview.print_interface_token(INTERFACEKEY_SECONDSCROLL_PAGEUP);
     gview.print_interface_token(INTERFACEKEY_SECONDSCROLL_PAGEDOWN);
     gview.print_interface_token(INTERFACEKEY_SECONDSCROLL_UP);
     gview.print_interface_token(INTERFACEKEY_SECONDSCROLL_DOWN);
     gps.addst(" - Adjust count");
    }
   } else gps.addst(" - Select");
  } else {
   if (Tabs->selected==BINDCATSNUM-1) {
    gps.locate(init.display.grid_y-2,40);
    gview.print_interface_token(INTERFACEKEY_SELECT);
    if (Bindings->selected) gps.addst(" - Commands");
    else gps.addst(" - Select");
   }
  }
  if (Mode>1) {
   gps.locate(init.display.grid_y-2,60);
   gview.print_interface_token(INTERFACEKEY_LEAVESCREEN);
   gps.addst(" - Done");
  }
 }
 P.which=0;
 Bindings->render(&P);
 P.which++;
 if (Mode<2) Keys->render(&P);
 else { //mode 2 display commands, mode 3 add command
  P.which++;
  // P.cat available
  Commands->render(&P);
 }
 gps.renewscreen();
}

const char* viewscreen_keybindingsst::ListItem
 (int Index, void* Param, char* Store) {
 BindingSelector* P=(BindingSelector*)Param;
 if (P) {
  switch (P->which) {
   case 2: // Commands
    if (Index) {
     MacroCommand com;
     com.Value=curComs[Index-1].Value;
     if ((com.flags)||(com.Binding)) {//macro or binding
      if (com.flags) com.Binding+=INTERFACEKEYNUM;
      gview.GetBinding(com.Binding)->GetDisplayName(Store);
//      curBind->GetDisplayName(Store);
      sprintf(strchr(Store,0),": %i",com.Repeat);
     } else sprintf(Store,"Delay: %i",com.Repeat);
     return Store;
    }
    return strcpy(Store,"Add Command");
   break;
   case 1: // Keys
    switch (Index) {
     case 0: return strcpy(Store,"Add Unicode Key");
     case 1: return strcpy(Store,"Add Scan Key");
     default:
      if (curKeys[Index-2].flags&KEY_UNICODEFLAG) strcpy(Store,"U: ");
      else strcpy(Store,"S: ");
      ConvertKeyToDisplay(Store+3,curKeys[Index-2]);
     return Store;
    }
   break;
   case 0: // Bindings
   default:
    if (P->cat==BINDCATSNUM-1) {
     if (!Index) {
      if (Mode==3) return strcpy(Store,"Delay 100ms");
      else return strcpy(Store,"New Macro");
     } else --Index;
    }
   return gview.GetBinding(BindCats[P->cat].ofset+Index)->GetDisplayName(Store);
  } //switch param->which
 } else return BindCats[Index].name; //TabBar
}

//To Do
//pare down the includes list
//must be able to delete macros
//add pressedRange for string entry
//make binding view mouse enabled, clicks for select
//put in the binding renaming

//Bugs
