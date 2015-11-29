#include <stdlib.h>
#include <ctype.h>
#include <strings.h>
#include <errno.h>
#include <sys/types.h>

#include "parse.h"

bool parse_size_p(const char *str, uint64_t *size)
{
	char sizeunit = 'B';
	char *endptr = NULL;
	uint64_t sz = strtoull(str, &endptr, 10);

	if (endptr == str)
	{
		errno = EINVAL;
		return false;
	}

	// skip whitespace
	while (isspace(*endptr)) ++ endptr;

	sizeunit = *endptr;
	// if something follows
	if (sizeunit)
	{
		const char *suffix = ++ endptr;
		
		// find end of word
		while (*endptr && !isspace(*endptr)) ++ endptr;

		// allow suffix "B" and "iB" for units other than "B"
		if ((sizeunit == 'B' && endptr != suffix) || (
			strncasecmp("B",  suffix, endptr - suffix) != 0 &&
		    strncasecmp("iB", suffix, endptr - suffix) != 0))
		{
			errno = EINVAL;
			return false;
		}

		// skip whitespace
		while (isspace(*endptr)) ++ endptr;

		// but now there may not be anything more
		if (*endptr)
		{
			errno = EINVAL;
			return false;
		}
	}

	switch (toupper(sizeunit))
	{
		case '\0':
		case 'B':
			break;

		case 'K':
			if (UINT64_MAX / 1024ll < sz)
			{
				errno = ERANGE;
				return false;
			}
			sz *= 1024ll;
			break;

		case 'M':
			if (UINT64_MAX / (1024ll * 1024ll) < sz)
			{
				errno = ERANGE;
				return false;
			}
			sz *= 1024ll * 1024ll;
			break;

		case 'G':
			if (UINT64_MAX / (1024ll * 1024ll * 1024ll) < sz)
			{
				errno = ERANGE;
				return false;
			}
			sz *= 1024ll * 1024ll * 1024ll;
			break;

		case 'T':
			if (UINT64_MAX / (1024ll * 1024ll * 1024ll * 1024ll) < sz)
			{
				errno = ERANGE;
				return false;
			}
			sz *= 1024ll * 1024ll * 1024ll * 1024ll;
			break;

		case 'P':
			if (UINT64_MAX / (1024ll * 1024ll * 1024ll * 1024ll * 1024ll) < sz)
			{
				errno = ERANGE;
				return false;
			}
			sz *= 1024ll * 1024ll * 1024ll * 1024ll * 1024ll;
			break;

		case 'E':
			if (UINT64_MAX / (1024ll * 1024ll * 1024ll * 1024ll * 1024ll * 1024ll) < sz)
			{
				errno = ERANGE;
				return false;
			}
			sz *= 1024ll * 1024ll * 1024ll * 1024ll * 1024ll * 1024ll;
			break;

		default:
			errno = EINVAL;
			return false;
	}

	if (size) *size = sz;

	return true;
}

bool parse_size(const char *str, size_t *size) {
	uint64_t sz = 0;
	if (!parse_size_p(str, &sz)) {
		return false;
	}

	/* might be bigger than max. size_t on 32bit plattforms */
	if (sz > SIZE_MAX) {
		errno = ERANGE;
		return false;
	}

	if (size) *size = (size_t)sz;

	return true;
}

bool parse_offset(const char *str, off_t *size) {
	uint64_t sz = 0;
	if (!parse_size_p(str, &sz)) {
		return false;
	}

	/* offset is always signed 64bit (because of compiler flags) */
	if (sz > INT64_MAX) {
		errno = ERANGE;
		return false;
	}

	if (size) *size = sz;

	return true;
}
