#include "libnd.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define U(n) "undefined (bit " #n ")"
const char *nd_flag_strings[32] = {
    "64bits", "unsigned", "float", U(3),  U(4),  U(5),  U(6),  U(7),
    U(8),     U(9),       U(10),   U(11), U(12), U(13), U(14), U(15),
    U(16),    U(17),      U(18),   U(19), U(20), U(21), U(22), U(23),
    U(24),    U(25),      U(26),   U(27), U(28), U(29), U(30), U(31)};

// Print fread() error on error, or print a message on EOF.
static void fread_error(FILE *file, const char *msg) {
  if (ferror(file)) {
    perror("fread()");
  } else {
    fprintf(stderr, "%s", msg);
  }
}

// Read a ND header from file. This function validates the magic, then stores
// the flags and reported length into the pointers passed.
int nd_readhdr(FILE *file, uint32_t *flags, uint64_t *len) {
  char magic[2];

  if (fread(magic, sizeof(char), 2, file) != 2) {
    fread_error(file, "error: File has no header.\n");
    return -1;
  }
  if (strncmp(magic, "ND", 2)) {
    fprintf(stderr, "error: File has invalid magic.\n");
    return -1;
  }

  if (fread(flags, sizeof(uint32_t), 1, file) != 1) {
    fread_error(file, "error: File has incomplete header (no flags).\n");
    return -1;
  }
  if (fread(len, sizeof(uint64_t), 1, file) != 1) {
    fread_error(file, "error: File has incomplete header (no length).\n");
    return -1;
  }

  return 0;
}
