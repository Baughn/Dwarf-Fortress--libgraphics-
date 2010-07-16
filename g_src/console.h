#ifndef CONSOLE_H
#define CONSOLE_H

#include "ViewBase.h"
#include "enabler.h"
#include "interface.h"
#include "scheme-base.h"
#include "init.h"
#include <deque>
#include <string>

using namespace std;

class viewscreen_console : public viewscreenst {
  struct logentry {
    unsigned char f, b, bold;
    string text;
  };
  
  int logsize;
  deque<logentry> lines;
  deque<string> history;
  string prompt;

  int scroll, promptscroll, promptpos;
  bool displaying;
  
public:
  viewscreen_console() {
    logsize = scroll = promptpos = 0;
    promptscroll = -1;
    displaying = false;
  }

  bool is_displaying() { return displaying; }
  bool toggle_display() {
    if (displaying)
      displaying = false;
    else {
      displaying = true;
      enabler.flag |= ENABLERFLAG_RENDER;
    }
  }

  void log(unsigned char f, unsigned char b, unsigned char bold, const string &text) {
    logentry entry;
    entry.f = f; entry.b = b; entry.bold = bold; entry.text = text;
    lines.push_front(entry);
    if (logsize > 9000)
      lines.pop_back();
    else
      logsize++;
    if (scroll && scroll < 9000) scroll++;
    if (displaying)
      enabler.flag |= ENABLERFLAG_RENDER;
  }
  
  virtual void render() {
    if (parent) parent->render();
    // We cover the top half of the screen
    int height = init.display.grid_y / 2;
    gps.erasescreen_rect(0, init.display.grid_x-1, 0, height);
    // Render the prompt
    gps.changecolor(7,0,0);
    gps.locate(height-1, 0);
    gps.addst("> ");
    gps.changecolor(7,0, promptscroll == -1);
    if (promptscroll >= 0)
      gps.addst(history[promptscroll]);
    else
      gps.addst(prompt);
    if (SDL_GetTicks() % 1000 > 500) {
      gps.changecolor(7,7,0);
      gps.addchar(' ');
    }
    // And the log
    deque<logentry>::iterator it = lines.begin();
    if (scroll < lines.size()) it += scroll;
    else it = lines.end() - 1;
    for (int i = height - 2; it != lines.end() && i >= 0; --i, ++it) {
      gps.changecolor(it->f, it->b, it->bold);
      gps.locate(i, 0);
      gps.addst(it->text);
    }
  }

  void history_to_prompt() {
    if (promptscroll >= 0)
      prompt = history[promptscroll];
    promptscroll = -1;
  }

  virtual void feed(set<InterfaceKey> &input) {
    enabler.flag |= ENABLERFLAG_RENDER;
    if (input.count(INTERFACEKEY_WORLD_PARAM_ENTER_VALUE)) {
      scroll = 0;
      if (promptscroll >= 0)
        prompt = history[promptscroll];
      if (!prompt.size()) return;
      history.push_front(prompt);
      // Attempt to evaluate as scheme code
      Scheme_Object *val = scheme_eval_safe(prompt);
      if (val) {
        Scheme_Object *out = scheme_get_param(scheme_current_config(), MZCONFIG_OUTPUT_PORT);
        scheme_display(val, out);
        scheme_display(scheme_make_char('\n'), out);
        scheme_add_global("it", val, scheme_get_env(scheme_current_config()));
        prompt.clear();
      }
    } else {
      // Attempt to parse input
      string temp;
      if (input.count(INTERFACEKEY_STRING_A000)) {
        history_to_prompt();
        if (prompt.size()) prompt.resize(prompt.size() - 1);
      } else if (standardstringentry(temp, 65535, STRINGENTRY_PRINTABLE, input)) {
        history_to_prompt();
        prompt.append(temp);
      } else if (input.count(INTERFACEKEY_STANDARDSCROLL_UP)) {
        if (promptscroll + 1 < history.size()) promptscroll++;
      } else if (input.count(INTERFACEKEY_STANDARDSCROLL_DOWN)) {
        if (promptscroll >= 0) promptscroll--;
        else prompt.clear();
      } else if (input.count(INTERFACEKEY_STANDARDSCROLL_PAGEUP)) {
        scroll += init.display.grid_y / 4;
        if (scroll >= lines.size()) scroll = lines.size();
      } else if (input.count(INTERFACEKEY_STANDARDSCROLL_PAGEDOWN)) {
        scroll -= init.display.grid_y / 4;
        if (scroll < 0) scroll = 0;
      }
    }
  }

  virtual void logic() {
    if (parent) parent->logic();
  }
};

extern viewscreen_console console;

#endif
