#ifndef CLI_OPTS_H
#define CLI_OPTS_H

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

typedef void (*cli_opt_callback)(void *data);

enum cli_opt_type {
  CLI_OPT_TYPE_BOOLEAN,
  CLI_OPT_TYPE_INTEGER,
  CLI_OPT_TYPE_LONG,
  CLI_OPT_TYPE_FLOAT,
  CLI_OPT_TYPE_DOUBLE,
  CLI_OPT_TYPE_STRING,
  CLI_OPT_TYPE_ARRAY,
  CLI_OPT_TYPE_CALLBACK,
  CLI_OPT_TYPE_HELP,
  CLI_OPT_TYPE_END
};

struct cli_opt {
  void *const outval;
  const char *longhand;
  enum cli_opt_type type;
  const char shorthand;
  const char *helpmsg;
  cli_opt_callback const cb;
  void *const ctx;
};

struct cli_opts_app {
  const struct cli_opt *opts;
  const char **argv;
  const char *opt;
  int argc;
  const char *desc;
};

#define CLI_OPT(sh, lh, typ, out, msg)                                         \
  {.shorthand = sh, .longhand = lh, .type = typ, .outval = out, .helpmsg = msg}

#if __STDC_VERSION__ >= 201112L
#define CLI_STATIC_ASSERT(cond, msg) _Static_assert(cond, msg)
#else
#define CLI_STATIC_ASSERT(cond, msg)                                           \
  typedef char static_assertion_##__LINE__[(cond) ? 1 : -1]
#endif

#define CLI_OPTS(name, ...)                                                    \
  struct cli_opt name[] = {                                                    \
      __VA_ARGS__,                                                             \
      CLI_OPT('h', "help", CLI_OPT_TYPE_HELP, NULL, "prints this help"),       \
      {.type = CLI_OPT_TYPE_END}};                                             \
  CLI_STATIC_ASSERT((sizeof((struct cli_opt[]){__VA_ARGS__}) > 0),             \
                    "cli_opts error: options list cannot be empty")

void cli_opts_init(struct cli_opts_app *const app, struct cli_opt *const opts,
                   const char *const desc);
void cli_opts_parse(struct cli_opts_app *const app, const int argc,
                    const char **const argv);

#endif // CLI_OPTS_H
