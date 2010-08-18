#version 140 // -*- Mode: C++ -*-

// Inserted by DF:
// GRID_X: Number of tiles (horizontally)
// GRID_Y: Number of tiles (vertically)
// DISP_X: Horizontal tile size, in pixels
// DISP_Y: Vertical tile size, in pixels
// vec4[16] colors: colors.txt
#line 9

// Inputs from DF. DO NOT RENAME!
in vec4 screenpos;      // Normalized device location for this vertex
in ivec2 rel_tile;      // rel_tile.xy  = screen location.
in int gl_VertexID;
uniform uvec3 abs_tile; // abs_tile + rel_tile = fort location
uniform float time;     // Seconds since DF was started
// Buffer containing DF screen data
uniform usamplerBuffer tile_array;

// Output defined by OpenGL
out vec4 gl_Position;   // Location to actually draw the vertex at
// Output to fragment shader. Feel free to use whatever.
flat out vec4 color_fg, color_bg;
// Three-dimensional texture position. The actual tile to use is in z,
// but we also need texture coordinates ranging from 0 to 1.
smooth out vec3 texpos;

uint get_data(ivec2 loc) { return texelFetch(tile_array, loc.x * GRID_Y + loc.y).r; }
uint get_tile(uint data) { return           data & 0x00ffffffu; }
vec4 get_fg(uint data) { return colors[int((data & 0x0f000000u) >> 24u)]; }
vec4 get_bg(uint data) { return colors[int((data & 0xf0000000u) >> 28u)]; }

void main() {
  // Write the position through.
  gl_Position = screenpos;
  // Compute the foreground and background colors
  uint data = get_data(rel_tile);
  color_fg  = get_fg(data);
  color_bg  = get_bg(data);
  // Compute the texture position. Notice how we prefer a bit of extra calculation to ifs..
  // Though TODO: We can save quite a lot of work with a geometry shader.
  vec2 coords[6] = vec2[](vec2(0,1), vec2(1,1), vec2(0,0),
                          vec2(0,0), vec2(1,1), vec2(1,0));
  texpos = vec3(coords[gl_VertexID % 6], get_tile(data));
}
