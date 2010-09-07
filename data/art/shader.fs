#version 140 // -*- mode: C++ -*-

// Inserted by DF:
// GRID_X: Number of tiles (horizontally)
// GRID_Y: Number of tiles (vertically)
// DISP_X: Horizontal tile size, in pixels
// DISP_Y: Vertical tile size, in pixels
// DISP_X_SCR: // Horizontal tile size, in normalized device coordinates (0-1)
// DISP_Y_SCR: // Vertical tile size, same
// vec4[16] colors: colors.txt
#line 11

uniform float time;     // Seconds since DF was started

in vec4 color_fg, color_bg;
smooth in vec3 texpos;

// Texture array according to the tileset
uniform sampler2DArray base;

void main() {
  vec4 color_tex = texture(base, texpos);
  gl_FragColor = mix(color_bg, color_tex * color_fg, color_tex.a);
}
