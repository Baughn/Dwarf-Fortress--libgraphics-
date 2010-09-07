#version 150 // -*- C++-mode -*-

// Inserted by DF:
// GRID_X: Number of tiles (horizontally)
// GRID_Y: Number of tiles (vertically)
// DISP_X: Horizontal tile size, in pixels
// DISP_Y: Vertical tile size, in pixels
// DISP_X_SCR: // Horizontal tile size, in normalized device coordinates (0-1)
// DISP_Y_SCR: // Vertical tile size, same
// vec4[16] colors: colors.txt
#line 11

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

// The vertex-shader outputs need to be copied here.
// Colors
in vec4 v_color_fg[1], v_color_bg[1];
// Tile
in float tile[1];

// Outputs to fragment shader
// Three-dimensional texture position. The actual tile to use is in z,
// but we also need texture coordinates ranging from 0 to 1.
smooth out vec3 texpos;
out vec4 color_fg, color_bg;


void main() {
  gl_Position = gl_in[0].gl_Position;
  texpos = vec3(0,1,tile[0]);
  color_fg = v_color_fg[0];
  color_bg = v_color_bg[0];
  EmitVertex();
  gl_Position = gl_in[0].gl_Position + vec4(DISP_X_SCR,0,0,0);
  texpos = vec3(1,1,tile[0]);
  color_fg = v_color_fg[0];
  color_bg = v_color_bg[0];
  EmitVertex();
  gl_Position = gl_in[0].gl_Position + vec4(0,DISP_Y_SCR,0,0);
  texpos = vec3(0,0,tile[0]);
  color_fg = v_color_fg[0];
  color_bg = v_color_bg[0];
  EmitVertex();
  gl_Position = gl_in[0].gl_Position + vec4(DISP_X_SCR,DISP_Y_SCR,0,0);
  texpos = vec3(1,0,tile[0]);
  color_fg = v_color_fg[0];
  color_bg = v_color_bg[0];
  EmitVertex();
  EndPrimitive();
}
