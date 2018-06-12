#ifndef _IO_PNG_H
#define _IO_PNG_H

#ifdef __cplusplus
extern "C" {
#endif

#define IO_PNG_VERSION "0.20110608"

#include <stddef.h>

	float *readPNG(const char *fname, size_t * nxp, size_t * nyp, size_t * ncp);

#ifdef __cplusplus
}
#endif

#endif /* !_IO_PNG_H */
