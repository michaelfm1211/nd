#pragma once

#include <stdint.h>
#include <stdio.h>

#define FLAG_64BITS (1 << 0)
#define FLAG_UNSIGNED (1 << 1)

extern const char *nd_flag_strings[32];

int nd_readhdr(FILE *file, uint32_t *flags, uint64_t *len);
