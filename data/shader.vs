// -*- mode: C -*-
// Version and defines emitted by gridrectst::init_gl at load:
// dimx, dimy:      Grid size of the screen array
// dimy_grid:       Displayed rows
// dispx, dispy:    Grid-cell (font) size
// vec4 colors[16]: Color palette; first non-bold, then bold

uniform usamplerBuffer data;
uniform samplerBuffer coords;

in vec4 gl_Vertex;
in int gl_VertexID;

flat out vec4 frontColor;
flat out vec4 backColor;
smooth out vec2 texCoords;

void main() {
  int tile = gl_VertexID / 6;
  int off_screen = tile * 4;

  int ch   = int(texelFetch(data, off_screen).a);
  int fg   = int(texelFetch(data, off_screen+1).a);
  int bg   = int(texelFetch(data, off_screen+2).a);
  int bold = clamp(int(texelFetch(data, off_screen+3).a), 0, 1);

  vec4 tex_square = texelFetch(coords, ch);
  vec2 coords[6] = vec2[](tex_square.sq, tex_square.tq, tex_square.tp,
                          tex_square.sq, tex_square.tp, tex_square.sp);
  texCoords = coords[gl_VertexID % 6];

  gl_Position = gl_Vertex;
  frontColor  = colors[fg + bold * 8];
  backColor   = colors[bg];
}
