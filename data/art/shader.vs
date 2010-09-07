#version 150 // -*- Mode: C++ -*-

// Inserted by DF:
// GRID_X: Number of tiles (horizontally)
// GRID_Y: Number of tiles (vertically)
// DISP_X: Horizontal tile size, in pixels
// DISP_Y: Vertical tile size, in pixels
// DISP_X_SCR: // Horizontal tile size, in normalized device coordinates (0-1)
// DISP_Y_SCR: // Vertical tile size, same
// vec4[16] colors: colors.txt
#line 11

// Inputs from DF. DO NOT RENAME!
in vec4 screenpos;      // Normalized device location for this vertex
in ivec2 rel_tile;      // rel_tile.xy  = screen location.
in int gl_VertexID;
uniform uvec3 abs_tile; // abs_tile + rel_tile = fort location
uniform float time;     // Seconds since DF was started
// Buffer containing DF screen data
uniform usamplerBuffer tile_array;

// Outputs to the geometry shader
out vec4 gl_Position;   // Bottom left corner of the tile
// Colors
out vec4 v_color_fg, v_color_bg;
// Tile to use
out float tile;

uint get_data(ivec2 loc) { return texelFetch(tile_array, loc.x * GRID_Y + loc.y).r; }
uint get_tile(uint data) { return           data & 0x00ffffffu; }
vec4 get_fg(uint data) { return colors[int((data & 0x0f000000u) >> 24u)]; }
vec4 get_bg(uint data) { return colors[int((data & 0xf0000000u) >> 28u)]; }

void main() {
  // Write the position through.
  gl_Position = screenpos;
  // Compute the foreground and background colors
  uint data = get_data(rel_tile);
  v_color_fg  = get_fg(data);
  v_color_bg  = get_bg(data);
  tile = get_tile(data);
}
