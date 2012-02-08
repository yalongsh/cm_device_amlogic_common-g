#ifndef AM_JPEG_ENCODER_9908049
#define AM_JPEG_ENCODER_9908049

typedef struct {
	int width;
	int height;
	int quality;
	
	unsigned char* idata;
	unsigned char* odata;
	unsigned char* data_in_app1;
	int app1_data_size;
	int ibuff_size;
	int obuff_size;
} jpeg_enc_t;

#ifdef __cplusplus
extern "C"
{
#endif
int encode_jpeg(jpeg_enc_t* enc);
int encode_jpeg2(jpeg_enc_t* enc);
#ifdef __cplusplus
}
#endif

#endif /* AM_JPEG_ENCODER_9908049 */
