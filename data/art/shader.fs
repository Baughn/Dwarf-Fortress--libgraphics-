#version 140 // -*- mode: C++ -*-

uniform float time;     // Seconds since DF was started

flat in vec4 color_fg, color_bg;
smooth in vec3 texpos;

// Texture array according to the tileset
uniform sampler2DArray base;

void main() {
  // gl_FragColor = color_fg;
  vec4 color_tex = texture(base, texpos);
  gl_FragColor = mix(color_bg, color_tex * color_fg, color_tex.a);
}
