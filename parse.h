#ifndef SENDFILE_PARSE_H
#define SENDFILE_PARSE_H
#pragma once

#include <inttypes.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool parse_size(const char *str, size_t *size);
bool parse_offset(const char *str, off_t *size);

#ifdef __cplusplus
}
#endif

#endif
