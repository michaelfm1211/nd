#include "libnd.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *nd_type_strings[] = {"int64",   "int32",   "uint64",  "uint32",
                                 "float64", "float32", "datetime"};

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
int nd_readhdr(FILE *file, uint32_t *type, uint64_t *len) {
  char magic[2];

  if (fread(magic, sizeof(char), 2, file) != 2) {
    fread_error(file, "libnd error: File has no header.\n");
    return -1;
  }
  if (strncmp(magic, "ND", 2)) {
    fprintf(stderr, "libnd error: File has invalid magic.\n");
    return -1;
  }

  if (fread(type, sizeof(uint32_t), 1, file) != 1) {
    fread_error(file, "libnd error: File has incomplete header (no type).\n");
    return -1;
  }
  if (fread(len, sizeof(uint64_t), 1, file) != 1) {
    fread_error(file, "libnd error: File has incomplete header (no length).\n");
    return -1;
  }

  return 0;
}

uint32_t nd_str_to_type(const char *str) {
  uint32_t i;

  if (str[0] == '#') {
    // type of the syntax @N mean a nonstandard type with ID N
    char *endptr;

    i = (uint32_t)strtol(str + 1, &endptr, 10);
    if (*endptr == '\0' && i > 0 && i < 32)
      return i;
  }

  for (i = 0; i < sizeof(nd_type_strings) / sizeof(nd_type_strings[0]); i++) {
    if (!strcmp(str, nd_type_strings[i]))
      return i;
  }

  return (uint32_t)-1;
}
