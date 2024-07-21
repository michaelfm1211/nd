#pragma once

#include <stdint.h>
#include <stdio.h>

#define ND_TYPE_INT64 0
#define ND_TYPE_INT32 1
#define ND_TYPE_UINT64 2
#define ND_TYPE_UINT32 3
#define ND_TYPE_FLOAT64 4
#define ND_TYPE_FLOAT32 5
#define ND_TYPE_DATETIME 6

extern const char *nd_type_strings[32];

int nd_readhdr(FILE *file, uint32_t *type, uint64_t *len);
uint32_t nd_str_to_type(const char *str);
