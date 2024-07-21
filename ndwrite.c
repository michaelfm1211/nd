#include "libnd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void usage(void) {
  fprintf(stderr, "usage: ndwrite [-h type:length] [-type value]\n");
  exit(1);
}

uint32_t str_to_type(const char *str) {
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

  fprintf(stderr, "error: No such type '%s'.\n", str);
  exit(1);
}

void write_hdr(char *arg) {
  char *lasts, *part, *endptr;
  uint32_t type;
  uint64_t length;

  // handle type
  part = strtok_r(arg, ":", &lasts);
  if (part == NULL)
    goto err_arg;
  type = str_to_type(part);

  // handle length
  part = strtok_r(NULL, ":", &lasts);
  if (part == NULL)
    goto err_arg;
  length = strtoll(part, &endptr, 10);
  if (*endptr != '\0')
    goto err_arg;

  if (fwrite("ND", sizeof(char), 2, stdout) != 2)
    goto err_fwrite;
  if (fwrite(&type, sizeof(uint32_t), 1, stdout) != 1)
    goto err_fwrite;
  if (fwrite(&length, sizeof(uint64_t), 1, stdout) != 1)
    goto err_fwrite;

  return;
err_fwrite:
  perror("fwrite()");
  exit(1);
err_arg:
  fprintf(stderr, "error: Invalid header argument format.\n");
  exit(1);
}

int conv_datatype(const char *arg, uint32_t type, uint64_t *out) {
  char *endptr;
  int base;

  switch (type) {
  case ND_TYPE_INT64:
  case ND_TYPE_INT32:
  case ND_TYPE_UINT64:
  case ND_TYPE_UINT32:
    if (strlen(arg) > 2 && !memcmp(arg, "0x", 2))
      base = 16;
    else
      base = 10;
    
    *out = (uint64_t)strtoll(arg, &endptr, base);
    if (*endptr != '\0')
      return 0;
    break;
  case ND_TYPE_FLOAT64:
    *(float *)out = strtof(arg, &endptr);
    if (*endptr != '\0')
      return 0;
    break;
  case ND_TYPE_FLOAT32:
    *(double *)out = strtod(arg, &endptr);
    if (*endptr != '\0')
      return 0;
    break;
  case ND_TYPE_DATETIME:
    fprintf(stderr, "error: ndwrite does not yet support writing datetime.\n");
    return 0;
    break;
  }
  return 1;
}

void write_record(const char *arg, uint32_t type) {
  size_t record_len;
  uint64_t data;

  if (type % 2 == 0)
    record_len = sizeof(uint64_t);
  else
    record_len = sizeof(uint32_t);

  if (!conv_datatype(arg, type, &data))
    goto err_arg;
  if (fwrite(&data, record_len, 1, stdout) != 1)
    goto err_fwrite;

  return;
err_fwrite:
  perror("fwrite()");
  exit(1);
err_arg:
  fprintf(stderr, "error: Invalid header argument format.\n");
  exit(1);
}

int main(int argc, char *argv[]) {
  if (argc < 2)
    usage();

  argc--;
  argv++;
  while (argc > 0) {
    if (strlen(argv[0]) < 2 || argv[0][0] != '-') {
      usage();
      return 1;
    }

    if (argv[0][1] == 'h') {
      write_hdr(argv[1]);
    } else {
      uint32_t type;

      type = str_to_type(argv[0] + 1);
      write_record(argv[1], type);
    }

    argc -= 2;
    argv += 2;
  };
}
