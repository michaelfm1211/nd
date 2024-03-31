#pragma once

#include <stdint.h>
#include <stdio.h>

#define ND_FLAG_64BITS (1 << 0)
#define ND_FLAG_UNSIGNED (1 << 1)

extern const char *nd_flag_strings[32];

int nd_readhdr(FILE *file, uint32_t *flags, uint64_t *len);
