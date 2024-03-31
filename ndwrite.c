#include "libnd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void usage(void) {
  fprintf(stderr, "usage: ndwrite [-h flag1,flag2:length] [-r bits:value] [-x "
                  "bits:hex_value]\n");
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

void write_record(char *arg, int base) {
  char *lasts, *part, *endptr;
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
  if (base == 16 && !strncmp(part, "0x", 2))
    part += 2;
  data = (uint64_t)strtoll(part, &endptr, base);
  if (*endptr != '\0')
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

  while ((optch = getopt(argc, argv, "h:r:x:")) != -1) {
    switch (optch) {
    case 'h':
      write_hdr(optarg);
      break;
    case 'r':
      write_record(optarg, 10);
      break;
    case 'x':
      write_record(optarg, 16);
      break;
    case '?':
    default:
      usage();
    }
  }
  return 0;
}
