#include "libnd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void usage(void) {
  fprintf(stderr, "usage: ndwrite [-h flag1,flag2:length] [-d|-x|-t|-f]"
                  "bits:value\n");
  exit(1);
}

uint32_t str_to_flag(const char *str) {
  int i;

  if (str[0] == '@') {
    // flags of the syntax @N mean the N-th bit is set
    char *endptr;

    i = (uint32_t)strtol(str + 1, &endptr, 10);
    if (*endptr == '\0' && i > 0 && i < 32)
      return (uint32_t)1 << i;
  }

  for (i = 0; i < 32; i++) {
    if (!strcmp(str, nd_flag_strings[i]))
      return (uint32_t)1 << i;
  }

  fprintf(stderr, "error: No such flag '%s'.\n", str);
  exit(1);
}

void write_hdr(char *arg) {
  char *lasts, *part, *lasts_flags, *flag_str, *endptr;
  uint32_t flags;
  uint64_t length;

  part = strtok_r(arg, ":", &lasts);
  if (part == NULL)
    goto err_arg;
  flag_str = strtok_r(part, ",", &lasts_flags);
  if (flag_str == NULL)
    goto err_arg;
  flags = 0;
  while (flag_str != NULL) {
    flags |= str_to_flag(flag_str);
    flag_str = strtok_r(NULL, ",", &lasts_flags);
  }

  part = strtok_r(NULL, ":", &lasts);
  if (part == NULL)
    goto err_arg;
  length = strtoll(part, &endptr, 10);
  if (*endptr != '\0')
    goto err_arg;

  if (fwrite("ND", sizeof(char), 2, stdout) != 2)
    goto err_fwrite;
  if (fwrite(&flags, sizeof(uint32_t), 1, stdout) != 1)
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

int conv_datatype(const char *str, int bits, char datatype, uint64_t *out) {
  uint64_t data_dx;
  float data_f32;
  double data_f64;
  char *endptr;

  switch (datatype) {
  case 'd':
    data_dx = (uint64_t)strtoll(str, &endptr, 10);
    if (*endptr != '\0')
      return 0;
    *out = data_dx;
    return 1;
  case 'x':
    if (!strncmp(str, "0x", 2))
      str += 2;
    data_dx = (uint64_t)strtoll(str, &endptr, 16);
    if (*endptr != '\0')
      return 0;
    *out = data_dx;
    return 1;
  case 'f':
    if (bits == sizeof(uint32_t)) {
      data_f32 = strtof(str, &endptr);

      if (*endptr != '\0')
        return 0;
      *out = *(uint32_t *)&data_f32;
    } else {
      data_f64 = strtod(str, &endptr);
      if (*endptr != '\0')
        return 0;
      *out = *(uint64_t *)&data_f64;
    }
    return 1;
  default:
    return 0;
  }
}

void write_record(char *arg, char datatype) {
  char *lasts, *part;
  size_t record_len;
  uint64_t data;

  part = strtok_r(arg, ":", &lasts);
  if (part == NULL)
    goto err_arg;
  if (!strcmp(part, "64"))
    record_len = sizeof(uint64_t);
  else if (!strcmp(part, "32"))
    record_len = sizeof(uint32_t);
  else
    goto err_arg;

  part = strtok_r(NULL, ":", &lasts);
  if (part == NULL)
    goto err_arg;
  if (!conv_datatype(part, record_len, datatype, &data))
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
  char optch;

  if (argc < 2)
    usage();

  while ((optch = getopt(argc, argv, "h:d:x:f:")) != -1) {
    switch (optch) {
    case 'h':
      write_hdr(optarg);
      break;
    case 'd':
    case 'x':
    case 'f':
      write_record(optarg, optch);
      break;
    case '?':
    default:
      usage();
    }
  }
  return 0;
}
