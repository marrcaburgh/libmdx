#include "cli_opts.h"

#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void cli_opts_init(struct cli_opts_app *const app, struct cli_opt *const opts,
                   const char *const desc) {
  app->opts = opts;
  app->desc = desc;
}

static void cli_opts_error(const char *errstr, ...) {
  va_list ap;

  va_start(ap, errstr);
  fprintf(stderr, "cli_opts error: ");
  vfprintf(stderr, errstr, ap);
  fprintf(stderr, "\n");
  va_end(ap);
}

static void cli_opts_usage(const struct cli_opt *const opt) {}

static bool cli_opts_to_num(struct cli_opts_app *const app,
                            const struct cli_opt *opt) {
  char *endptr;
  errno = 0;

  union {
    long i;
    float f;
  } val;

  if (opt->type == CLI_OPT_TYPE_INTEGER) {
    val.i = strtol(*app->argv, &endptr, 10);
    // else if long
    // else if float
    // else if doubl
  } else {
    val.f = strtof(*app->argv, &endptr);
  }

  if (endptr == *app->argv) {
    cli_opts_error("not a number '%s'", app->argv);
    return false;
  } else if (errno == ERANGE) {
    cli_opts_error("out of range '%s'", app->argv);
    return false;
  }

  if (opt->type == CLI_OPT_TYPE_INTEGER) {
    if (val.i > INT_MAX || val.i < INT_MIN) {
      cli_opts_error("integer out of range '%s'", app->argv);
      return false;
    }

    *(int *)opt->outval = (int)val.i;
  } else {
    *(float *)opt->outval = val.f;
  }

  return true;
}

static bool cli_opt_populate(struct cli_opts_app *const app,
                             const struct cli_opt *opt) {
  if (app->argc > 1) {
    app->argc--;
    app->argv++;
  } else {
    cli_opts_error("no value '%s'", *app->argv);
    return false;
  }

  if (opt->type == CLI_OPT_TYPE_ARRAY) {
    // TODO: populate array
    return true;
  } else if (opt->type == CLI_OPT_TYPE_STRING) {
    *(const char **)opt->outval = *app->argv;
    return true;
  }

  char *endptr;
  union {
    long l;
    double d;
  } val;

  if (opt->type == CLI_OPT_TYPE_INTEGER || opt->type == CLI_OPT_TYPE_LONG) {
    val.l = strtol(*app->argv, &endptr, 10);
  } else {
    val.d = strtod(*app->argv, &endptr);
  }

  if (endptr == *app->argv) {
    cli_opts_error("value not a number '%s'", *app->argv);
    return false;
  } else if (errno == ERANGE) {
    cli_opts_error("value out of range '%s'", *app->argv);
    return false;
  }

  switch (opt->type) {
  case CLI_OPT_TYPE_INTEGER:
    if (val.l > INT_MAX || val.l < INT_MIN) {
      cli_opts_error("integer value out of range '%s'", *app->argv);
      return false;
    }
    *(int *)opt->outval = (int)val.l;
    break;
  case CLI_OPT_TYPE_LONG:
    *(long *)opt->outval = val.l;
    break;
  case CLI_OPT_TYPE_FLOAT:
    *(float *)opt->outval = (float)val.d;
    break;
  case CLI_OPT_TYPE_DOUBLE:
    *(double *)opt->outval = val.d;
    break;
  default:
    cli_opts_error("how the fuck?");
    return false;
  }

  return true;
}

void cli_opts_help(struct cli_opts_app *app) {
  //
}

static bool cli_opt_assign(struct cli_opts_app *const app,
                           const struct cli_opt *const opt) {
  switch (opt->type) {
  case CLI_OPT_TYPE_HELP:
    cli_opts_help(app);
    break;
  case CLI_OPT_TYPE_CALLBACK:
    opt->cb(opt->ctx);
    break;
  case CLI_OPT_TYPE_BOOLEAN:
    *(bool *)opt->outval = !*(bool *)opt->outval;
    break;
  default:
    return cli_opt_populate(app, opt);
  }

  return true;
}

static bool cli_opts_match_short(struct cli_opts_app *const app) {
  for (const char *p = app->opt; *p != '\0'; p++) {
    for (const struct cli_opt *o = app->opts; o->type != CLI_OPT_TYPE_END;
         o++) {
      if (o->shorthand == *p) {
        cli_opt_assign(app, o);
        return true;
      }
    }
  }

  return false;
}

static bool cli_opts_verify(const struct cli_opt *const opts) {
  // TODO: verify options
  return true;
}

void cli_opts_parse(struct cli_opts_app *const app, const int argc,
                    const char **const argv) {
  app->argc = argc - 1;
  app->argv = argv + 1;

  for (; app->argc; app->argc--, app->argv++) {
    const char *arg = app->argv[0];

    if (arg[0] != '-' || !arg[1]) {
      continue;
    }

    // shorthand option
    if (arg[1] != '-') {
      app->opt = arg + 1;
      cli_opts_match_short(app);
      continue;
    }
  }
}
