class init_fontst
{
	public:
		long small_font_texpos[256];
		long large_font_texpos[256];
		long small_font_datapos[256];
		long large_font_datapos[256];
		float small_font_adjx;
		float small_font_adjy;
		float large_font_adjx;
		float large_font_adjy;
		long small_font_dispx;
		long small_font_dispy;
		long large_font_dispx;
		long large_font_dispy;
};

enum InitDisplayFlag
{
	INIT_DISPLAY_FLAG_USE_GRAPHICS,
	INIT_DISPLAY_FLAG_BLACK_SPACE,
	INIT_DISPLAY_FLAG_PARTIAL_PRINT,
	INIT_DISPLAY_FLAG_FRAME_BUFFER,
	INIT_DISPLAY_FLAG_SINGLE_BUFFER,
	INIT_DISPLAY_FLAG_ACCUM_BUFFER,
	INIT_DISPLAY_FLAG_UNUSED_01_07,//
	INIT_DISPLAY_FLAG_UNUSED_01_08,//
	INIT_DISPLAY_FLAGNUM
};

enum InitDisplayWindow
{
	INIT_DISPLAY_WINDOW_TRUE,
	INIT_DISPLAY_WINDOW_FALSE,
	INIT_DISPLAY_WINDOW_PROMPT,
	INIT_DISPLAY_WINDOWNUM
};

class init_displayst
{
	public:
		flagarrayst flag;
		InitDisplayWindow windowed;
		LARGE_INTEGER dwarf_frame_rate;
		LARGE_INTEGER g_frame_rate;

		long grid_x,grid_y;
		long orig_grid_x,orig_grid_y;
		long small_grid_x,small_grid_y;
		long large_grid_x,large_grid_y;

		char partial_print_count;

		init_displayst();
};

enum InitMediaFlag
{
	INIT_MEDIA_FLAG_SOUND_OFF,
	INIT_MEDIA_FLAG_INTRO_OFF,
	INIT_MEDIA_FLAGNUM
};

class init_mediast
{
	public:
		flagarrayst flag;
		long volume;

		init_mediast()
			{
			flag.set_size_on_flag_num(INIT_MEDIA_FLAGNUM);
			volume=255;
			}
};

enum InitInputFlag
{
	INIT_INPUT_FLAG_MOUSE_OFF,
	INIT_INPUT_FLAG_MOUSE_PICTURE,
	INIT_INPUT_FLAGNUM
};

class init_inputst
{
	public:
		long hold_time;
		long pause_zoom_no_interface_ms;
		flagarrayst flag;

		init_inputst()
			{
			hold_time=150;
			pause_zoom_no_interface_ms=0;
			flag.set_size_on_flag_num(INIT_INPUT_FLAGNUM);
			}
};

enum InitWindowFlag
{
	INIT_WINDOW_FLAG_TOPMOST,
	INIT_WINDOW_FLAG_VSYNC_ON,
	INIT_WINDOW_FLAG_VSYNC_OFF,
	INIT_WINDOW_FLAG_TEXTURE_LINEAR,
	INIT_WINDOW_FLAGNUM
};

class init_windowst
{
	public:
		flagarrayst flag;

		init_windowst()
			{
			flag.set_size_on_flag_num(INIT_WINDOW_FLAGNUM);
			}
};

class initst
{
	public:
		init_displayst display;
		init_mediast media;
		init_inputst input;
		init_fontst font;
		init_windowst window;
};