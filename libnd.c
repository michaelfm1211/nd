#include "libnd.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

const char *nd_type_strings[] = {
    "int64", "int32", "uint64", "uint32", "float64", "float32", "datetime"};

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
