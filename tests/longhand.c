#include "test.h"

int main() {
  // clang-format off
  const char *argv[] = {
    "./app/path",
    "--boolean",
    "--string", "a_str",
    "--integer", "1",
    "--long", "1234567890l",
    "--float", "0.0f",
    "--double", "0.0",
    "--callback"
  };
  // clang-format on

  struct cli_opts_app app;

  cli_opts_init(&app, opts, "Longhand test");
  cli_opts_parse(&app, ARRAY_LENGTH(argv), argv);
  test_print();
}
