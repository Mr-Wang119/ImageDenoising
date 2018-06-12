#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <png.h>

#define PNG_SIG_LEN 4
#define IO_PNG_U8  0x0001       /*  8bit unsigned integer */
#define IO_PNG_F32 0x0002       /* 32bit float */



static void* io_png_read_abort(FILE* fp, png_structp* png_ptr_p, png_infop* info_ptr_p) {
	png_destroy_read_struct(png_ptr_p, info_ptr_p, NULL);
	if (NULL != fp && stdin != fp)
		(void)fclose(fp);
	return NULL;
}

static void* io_png_read_raw(const char *fname, size_t * nxp, size_t * nyp, size_t * ncp, int png_transform, int dtype) {

	png_byte png_sig[PNG_SIG_LEN];
	png_structp png_ptr;
	png_infop info_ptr;
	png_bytepp row_pointers;
	png_bytep row_ptr;
	/* volatile: because of setjmp/longjmp */
	FILE *volatile fp = NULL;
	void *data = NULL;
	unsigned char *data_u8 = NULL;
	unsigned char *data_u8_ptr = NULL;
	float *data_f32 = NULL;
	float *data_f32_ptr = NULL;
	size_t size;
	size_t i, j, k;

	/* parameters check */
	if (NULL == fname || NULL == nxp || NULL == nyp || NULL == ncp)
		return NULL;
	if (IO_PNG_U8 != dtype && IO_PNG_F32 != dtype)
		return NULL;

	/* open the PNG input file */
	if (0 == strcmp(fname, "-"))
		fp = stdin;
	else if (NULL == (fp = fopen(fname, "rb")))
		return NULL;

	/* read in some of the signature bytes and check this signature */
	if ((PNG_SIG_LEN != fread(png_sig, 1, PNG_SIG_LEN, fp))
		|| 0 != png_sig_cmp(png_sig, (png_size_t)0, PNG_SIG_LEN))
		return io_png_read_abort(fp, NULL, NULL);

	/*
	* create and initialize the png_struct
	* with the default stderr and error handling
	*/
	if (NULL == (png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
		NULL, NULL, NULL)))
		return io_png_read_abort(fp, NULL, NULL);

	/* allocate/initialize the memory for image information */
	if (NULL == (info_ptr = png_create_info_struct(png_ptr)))
		return io_png_read_abort(fp, &png_ptr, NULL);

	/* set error handling */
	if (0 != setjmp(png_jmpbuf(png_ptr)))
		/* if we get here, we had a problem reading the file */
		/* free all of the memory associated with the png_ptr and info_ptr */
		return io_png_read_abort(fp, &png_ptr, &info_ptr);

	/* set up the input control using standard C streams */
	png_init_io(png_ptr, fp);

	/* let libpng know that some bytes have been read */
	png_set_sig_bytes(png_ptr, PNG_SIG_LEN);

	/*
	* set the read filter transforms, to get 8bit RGB whatever the
	* original file may contain:
	* PNG_TRANSFORM_STRIP_16      strip 16-bit samples to 8 bits
	* PNG_TRANSFORM_PACKING       expand 1, 2 and 4-bit
	*                             samples to bytes
	*/
	png_transform |= (PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING);

	/* read in the entire image at once */
	png_read_png(png_ptr, info_ptr, png_transform, NULL);

	/* get image informations */
	*nxp = (size_t)png_get_image_width(png_ptr, info_ptr);
	*nyp = (size_t)png_get_image_height(png_ptr, info_ptr);
	*ncp = (size_t)png_get_channels(png_ptr, info_ptr);
	row_pointers = png_get_rows(png_ptr, info_ptr);

	/*
	* allocate the output data RGB array
	* deinterlace and convert png RGB RGB RGB 8bit to RRR GGG BBB
	* the image is deinterlaced layer after layer
	* this generic loop also works for one single channel
	*/
	size = *nxp * *nyp * *ncp;
	switch (dtype) {
	case IO_PNG_U8:
		if (NULL == (data_u8 =
			(unsigned char *)malloc(size * sizeof(unsigned char))))
			return io_png_read_abort(fp, &png_ptr, &info_ptr);
		data = (void *)data_u8;
		for (k = 0; k < *ncp; k++) {
			/* channel loop */
			data_u8_ptr = data_u8 + (size_t)(*nxp * *nyp * k);
			for (j = 0; j < *nyp; j++) {
				/* row loop */
				row_ptr = row_pointers[j] + k;
				for (i = 0; i < *nxp; i++) {
					/* pixel loop */
					*data_u8_ptr++ = (unsigned char)*row_ptr;
					row_ptr += *ncp;
				}
			}
		}
		break;
	case IO_PNG_F32:
		if (NULL == (data_f32 = (float *)malloc(size * sizeof(float))))
			return io_png_read_abort(fp, &png_ptr, &info_ptr);
		data = (void *)data_f32;
		for (k = 0; k < *ncp; k++) {
			/* channel loop */
			data_f32_ptr = data_f32 + (size_t)(*nxp * *nyp * k);
			for (j = 0; j < *nyp; j++) {
				/* row loop */
				row_ptr = row_pointers[j] + k;
				for (i = 0; i < *nxp; i++) {
					/* pixel loop */
					*data_f32_ptr++ = (float)*row_ptr;
					row_ptr += *ncp;
				}
			}
		}
		break;
	}

	/* clean up and free any memory allocated, close the file */
	(void)io_png_read_abort(fp, &png_ptr, &info_ptr);

	return data;
}



float *readPNG(const char *fname,
	size_t * nxp, size_t * nyp, size_t * ncp) {
	/* read the image as float */
	return (float *)io_png_read_raw(fname, nxp, nyp, ncp,
		PNG_TRANSFORM_IDENTITY, IO_PNG_F32);
}