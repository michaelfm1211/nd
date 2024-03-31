#include "libnd.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

struct options {
  bool print_header;
  bool lineno;
  bool hex;
  bool range;

  uint64_t range_start;
  uint64_t range_end;
};

void usage(void) {
  fprintf(stderr, "usage: ndcat [-hlx] [-r range_start[,range_end]] file\n");
  exit(1);
}

void parse_range(struct options *opts, char *str) {
  char *endptr;

  opts->range_start = strtoll(str, &endptr, 10);
  if (*endptr == '\0') {
    opts->range_end = opts->range_start;
    return;
  }

  if (*endptr != ',') {
    fprintf(stderr, "error: Invalid range syntax.\n");
    exit(1);
  }

  opts->range_end = strtoll(endptr + 1, &endptr, 10);
  if (*endptr != '\0') {
    fprintf(stderr, "error: Invalid range syntax.\n");
    exit(1);
  }
}

void print_header(uint32_t flags, uint64_t len) {
  int i;

  printf("header: magic: ND\n");
  for (i = 0; i < 32; i++) {
    if ((flags & ((uint32_t)1 << i)) == 0)
      continue;
    printf("header: flag: %s\n", nd_flag_strings[i]);
  }
  printf("header: length: %" PRIu64 "\n", len);
}

void print_record64(int64_t record, uint32_t flags, struct options opts) {
  if (opts.hex)
    printf("0x%016" PRIx64 "\n", record);
  else if (flags & FLAG_UNSIGNED)
    printf("%" PRIu64 "\n", record);
  else
    printf("%" PRId64 "\n", record);
}

void print_record32(int32_t record, uint32_t flags, struct options opts) {
  if (opts.hex)
    printf("0x%08" PRIx32 "\n", record);
  else if (flags & FLAG_UNSIGNED)
    printf("%" PRIu32 "\n", record);
  else
    printf("%" PRId32 "\n", record);
}

void print_records(FILE *file, uint32_t flags, uint64_t len,
                   struct options opts) {
  size_t int_len;
  void *buf;
  uint64_t records_read;
  size_t read_len;

  if (flags & FLAG_64BITS)
    int_len = sizeof(int64_t);
  else
    int_len = sizeof(int32_t);

  buf = malloc(int_len * 1024);
  if (buf == NULL) {
    perror("malloc()");
    exit(1);
  }

  records_read = 0;
  while ((read_len = fread(buf, int_len, 1024, file)) > 0) {
    size_t i;

    for (i = 0; i < read_len; i++) {
      if (opts.range && (records_read + 1 < opts.range_start ||
                         records_read + 1 > opts.range_end))
        goto skip;

      if (opts.lineno)
        printf("%" PRIu64 ": ", records_read + 1);

      if (flags & FLAG_64BITS)
        print_record64(((int64_t *)buf)[i], flags, opts);
      else
        print_record32(((int32_t *)buf)[i], flags, opts);

    skip:
      records_read++;
    }
  }
  free(buf);
  if (ferror(file)) {
    perror("fread()");
    exit(1);
  } else if (records_read != len) {
    fprintf(stderr,
            "warning: File header has incorrect length (expected %" PRIu64
            ", found %" PRIu64 ").\n",
            len, records_read);
  }
}

int main(int argc, char *argv[]) {
  struct options opts;
  char optch;
  FILE *file;
  uint32_t flags;
  uint64_t len;

  bzero(&opts, sizeof(struct options));
  while ((optch = getopt(argc, argv, "hlxnr:")) != -1) {
    switch (optch) {
    case 'h':
      opts.print_header = true;
      break;
    case 'l':
      opts.lineno = true;
      break;
    case 'x':
      opts.hex = true;
      break;
    case 'r':
      opts.range = true;
      parse_range(&opts, optarg);
      break;
    case '?':
    default:
      usage();
    }
  }
  argc -= optind;
  argv += optind;

  if (argc != 1)
    usage();

  file = fopen(argv[0], "r");
  if (file == NULL) {
    perror("fopen()");
    return 1;
  }

  if (nd_readhdr(file, &flags, &len) == -1)
    return 1;

  if (opts.print_header)
    print_header(flags, len);
  print_records(file, flags, len, opts);
  return 0;
}
