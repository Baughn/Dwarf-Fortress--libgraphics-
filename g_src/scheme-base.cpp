#include "graphics.h"
#include "init.h"
#include "enabler.h"
#include "ViewBase.h"
#include "interface.h"
#include "console.h"
#include "keybindings.h"
#include "curses.h"

#include "scheme-base.h"


static void register_function(Scheme_Env *e, const char *name, Scheme_Prim *prim, int arity) {
  scheme_add_global(name, scheme_make_prim_w_arity(prim, name, arity, arity), e);
}

static Scheme_Object *simticks(int argc, Scheme_Object **argv) {
  return scheme_make_integer(enabler.simticks.read());
}

static Scheme_Object *gputicks(int argc, Scheme_Object **argv) {
  return scheme_make_integer(enabler.gputicks.read());
}

class viewscreen_scheme : public viewscreenst {
  Scheme_Object *feedp, *logicp, *renderp;
  bool passthrough;
public:
  virtual void feed(set<InterfaceKey> &events) {
    // Build a list from the interfacekey set
    Scheme_Object *list = scheme_null;
    for (set<InterfaceKey>::iterator it = events.begin(); it != events.end(); ++it)
      list = scheme_make_pair(scheme_intern_symbol(bindingNames.left[*it].c_str()),
                              list);
    // Aand apply the function
    Scheme_Object *ret = scheme_apply(feedp, 1, &list);
    if (SCHEME_BOOLP(ret) && SCHEME_TRUEP(ret))
      parent->feed(events);
  }
  virtual void logic() {
    if (passthrough && parent)
      parent->logic();
    scheme_apply_safe(logicp, 0, NULL);
  }
  virtual void render() {
    if (passthrough && parent)
      parent->render();
    scheme_apply_safe(renderp, 0, NULL);
  }
  viewscreen_scheme(Scheme_Object *feed, Scheme_Object *logic, Scheme_Object *render, bool pass) {
    feedp = feed; logicp = logic; renderp = render;
    passthrough = pass;
  };
};

static Scheme_Object *push_interface(int argc, Scheme_Object **argv) {
  // Make sure the passed functions are valid
  Scheme_Object *feed = argv[0],
    *logic = argv[1],
    *render = argv[2],
    *passthrough = argv[3];
  for (int f=0; f<3; f++)
    if (!(SCHEME_PROCP(argv[f]))) {
      scheme_wrong_type("push-interface", "function", f, argc, argv);
      return scheme_void;
    }
  if (!(SCHEME_BOOLP(passthrough))) {
    scheme_wrong_type("push-interface", "bool", 3, argc, argv);
    return scheme_void;
  }
  // Okay. Construct and insert a new viewscreen_scheme.
  viewscreen_scheme *v = new viewscreen_scheme(feed, logic, render, SCHEME_TRUEP(passthrough));
  gview.addscreen(v, INTERFACE_PUSH_AT_BACK, NULL);
  return scheme_make_external_cptr(v, scheme_intern_symbol("viewscreen"));
}

static Scheme_Object *del_interface(int argc, Scheme_Object **argv) {
  if (!SCHEME_CPTRP(argv[0]) ||
      !scheme_eq(SCHEME_CPTR_TYPE(argv[0]), scheme_intern_symbol("viewscreen")))
    scheme_wrong_type("del-interface", "viewscreen", 0, argc, argv);
  
  viewscreen_scheme *v = (viewscreen_scheme*)SCHEME_CPTR_VAL(argv[0]);
  v->breakdownlevel = INTERFACE_BREAKDOWN_STOPSCREEN;
  return scheme_void;
}

static Scheme_Object *print(int argc, Scheme_Object **argv) {
  for (int i=0; i < 2; i++)
    if (!SCHEME_INTP(argv[i]))
      scheme_wrong_type("addstr", "fixnum", i, argc, argv);
  if (!SCHEME_CHAR_STRINGP(argv[2]))
    scheme_wrong_type("addstr", "string", 2, argc, argv);

  gps.locate(SCHEME_INT_VAL(argv[1]), SCHEME_INT_VAL(argv[0]));

  mzchar *s = SCHEME_CHAR_STR_VAL(argv[2]);
  int len = SCHEME_CHAR_STRLEN_VAL(argv[2]);
  string cp437(len,0);
  for (int i=0; i<len; ++i)
    cp437[i] = unicode_charmap[s[i]] ? unicode_charmap[s[i]] : s[i];
  gps.addst(cp437);
  
  return scheme_void;
}

static Scheme_Object *set_color(int argc, Scheme_Object **argv) {
  for (int i=0; i < 3; i++)
    if (!SCHEME_INTP(argv[i]))
      scheme_wrong_type("set-color%", "fixnum", i, argc, argv);

  gps.changecolor(SCHEME_INT_VAL(argv[0]),
                  SCHEME_INT_VAL(argv[1]),
                  SCHEME_INT_VAL(argv[2]));
}

// Define primitive calls, i.e. Scheme->C++ calls
static void define_primitives(Scheme_Env *e) {
  register_function(e, "simticks", simticks, 0);
  register_function(e, "gputicks", gputicks, 0);
  
  register_function(e, "push-interface", push_interface, 4);
  register_function(e, "del-interface", del_interface, 1);
  register_function(e, "addstr", print, 3);
  register_function(e, "set-color%", set_color, 3);
}

static long console_write_primitive(const char *buffer, long offset, long size, bool as_error) {
  static string line;
  line.append(buffer+offset, size);
  size_t pos, start = 0;
  while ((pos = line.find("\n", start)) != string::npos) {
    string str(line, start, pos - start);
    if (as_error)
      errorlog_string(str);
    else
      console.log(7,0,0, str);
    start = pos + 1;
  }
  if (start) {
    line = string(line, start);
  }
  return size;
}

static long console_write(Scheme_Output_Port *port, const char *buffer, long offset,
                           long size, int rarely_block, int enable_break) {
  return console_write_primitive(buffer, offset, size, false);
}

static long console_write_plus_errorlog(Scheme_Output_Port *port, const char *buffer, long offset,
                                        long size, int rarely_block, int enable_break) {
  return console_write_primitive(buffer, offset, size, true);
}

static int always_ready_fun(Scheme_Output_Port *port) { return 1; }
static void console_fake_closer(Scheme_Output_Port *port) {}
static void console_zero_wakeup(Scheme_Output_Port *port, void *fds) {}

Scheme_Object *scheme_eval_safe(const string &sexp) {
  mz_jmp_buf * volatile save, handler;
  save = scheme_current_thread->error_buf;
  scheme_current_thread->error_buf = &handler;
  Scheme_Object *ret = NULL;
  if (!scheme_setjmp(handler))
    ret = scheme_eval_string(sexp.c_str(), scheme_get_env(scheme_current_config()));
  scheme_current_thread->error_buf = save;
  return ret;
}

Scheme_Object *scheme_apply_safe(Scheme_Object *f, int argc, Scheme_Object **argv) {
  mz_jmp_buf * volatile save, handler;
  save = scheme_current_thread->error_buf;
  scheme_current_thread->error_buf = &handler;
  Scheme_Object *ret = NULL;
  if (!scheme_setjmp(handler))
    ret = scheme_apply(f, argc, argv);
  scheme_current_thread->error_buf = save;
  return ret;
}

void call_declare_modules(Scheme_Env *env);
  
int scheme_start(Scheme_Env *e, int argc, char **argv) {
  // Declare embedded modules from scheme-modules.c
  call_declare_modules(e);
  scheme_namespace_require(scheme_intern_symbol("racket"));
  scheme_namespace_require(scheme_intern_symbol("racket/date"));
  scheme_namespace_require(scheme_intern_symbol("rnrs"));

  // Declare primitive functions
  define_primitives(e);

  // Create an error handler
  mz_jmp_buf handler;
  scheme_current_thread->error_buf = &handler;
  if (scheme_setjmp(handler)) {
    // There was an error
    MessageBox(NULL, "Fatal error while initializing scheme!", 0, 0);
    abort();
  } else {
    // Redefine scheme's stdin/stdout/stderr to work with the console
    scheme_set_param(scheme_current_config(), MZCONFIG_OUTPUT_PORT,
                     (Scheme_Object*)scheme_make_output_port(scheme_intern_symbol("console"),
                                                             NULL,
                                                             scheme_intern_symbol("console"),
                                                             scheme_write_evt_via_write,
                                                             console_write,
                                                             always_ready_fun,
                                                             console_fake_closer,
                                                             console_zero_wakeup,
                                                             NULL,
                                                             NULL,
                                                             0));
    scheme_set_param(scheme_current_config(), MZCONFIG_ERROR_PORT,
                     (Scheme_Object*)scheme_make_output_port(scheme_intern_symbol("console"),
                                                             NULL,
                                                             scheme_intern_symbol("console"),
                                                             scheme_write_evt_via_write,
                                                             console_write_plus_errorlog,
                                                             always_ready_fun,
                                                             console_fake_closer,
                                                             console_zero_wakeup,
                                                             NULL,
                                                             NULL,
                                                             0));

    // Load and execute init.scm
    gamelog_string("Initializing scheme..");
    Scheme_Object *out = scheme_get_param(scheme_current_config(), MZCONFIG_OUTPUT_PORT);
    Scheme_Object *ret = scheme_load("scripts/init.scm");
    if (!ret)
      errorlog_string("Loading scripts/init.scm failed!");
    else
      gamelog_string("Scheme initialized");
  }

  // Set the exit handler (to one that doesn't exit)
  scheme_set_param(scheme_current_config(), MZCONFIG_EXIT_HANDLER, scheme_void);

  // Set our permanent error handler
  bool loop_initialized = false;
  for (;;) {
    scheme_current_thread->error_buf = &handler;
    if (scheme_setjmp(handler)) {
      // Call back into the enabler
      MessageBox(NULL, "Unhandled scheme exception! Dazed and confused, but attempting to continue. DF may be about to crash.", 0, 0);
      loop_initialized = true;
    } else {
      enabler.async_loop(loop_initialized);
    }
  }
  return 0;
}
