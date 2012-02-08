/****************************************************
*
* api to encoder rgb data into jpeg format. 
* 01/27/2011.
* kasin.li@amlogic.com
*
*****************************************************/
#include"amljpeg_enc.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "jpeglib.h"
#include "jconfig.h"
#include <setjmp.h>
#include "amljpeg_enc.h"
#include <cutils/log.h>

#include <tjutil.h>
#include <turbojpeg.h>

typedef struct {
  struct jpeg_destination_mgr pub; /* public fields */

  char* obuf;
  int buf_size;
  int data_size;
} aml_destination_mgr;

typedef aml_destination_mgr * aml_dest_ptr;

static void init_destination (j_compress_ptr cinfo)
{
  aml_dest_ptr dest = (aml_dest_ptr) cinfo->dest;

  dest->pub.next_output_byte = dest->obuf;
  dest->pub.free_in_buffer = dest->buf_size;
}

static boolean empty_output_buffer (j_compress_ptr cinfo)
{
  aml_dest_ptr dest = (aml_dest_ptr) cinfo->dest;

  /* error handling, input buffer too small. */
  /* maybe you need not use jpeg encoder. */

  return FALSE;
}

static void term_destination (j_compress_ptr cinfo)
{
  aml_dest_ptr dest = (aml_dest_ptr) cinfo->dest;
  size_t datacount = dest->buf_size - dest->pub.free_in_buffer;

  dest->data_size = datacount;
}

int encode_jpeg(jpeg_enc_t* enc) 
{

	/* create compress. information data. */
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	aml_destination_mgr dest;
	
	JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
	int row_stride;		/* physical row width in image buffer */
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	//jpeg_stdio_dest(&cinfo, outfile);
	cinfo.dest=(struct jpeg_destination_mgr *)&dest;
	dest.pub.init_destination = init_destination;
	dest.pub.empty_output_buffer = empty_output_buffer;
	dest.pub.term_destination = term_destination;
	
	dest.obuf = enc->odata;
	dest.buf_size = enc->obuff_size;

	/* set parameters. */
	cinfo.image_width = enc->width; 	
	cinfo.image_height = enc->height;
	cinfo.input_components = 3;		
	cinfo.in_color_space = JCS_RGB;
	jpeg_set_defaults(&cinfo);
	if(enc->quality==0)
		enc->quality=75;
	jpeg_set_quality(&cinfo, enc->quality, TRUE);

	jpeg_start_compress(&cinfo, TRUE);
	
	if(enc->data_in_app1)
		jpeg_write_marker(&cinfo,0xe1,enc->data_in_app1,enc->app1_data_size);
	
	row_stride = enc->width * 3;	

	/* processing. */
	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer[0] = & enc->idata[cinfo.next_scanline * row_stride];
		(void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
	return dest.data_size;
}

#define _throwtj() {LOGD("TurboJPEG ERROR:\n%s\n", tjGetErrorStr());  \
	enc->obuff_size=0;goto exit;}
#define _tj(f) {if((f)==-1) _throwtj();}
#define _throw(m) {LOGD("ERROR: %s\n", m); enc->obuff_size=0; goto exit;}

int encode_jpeg2(jpeg_enc_t* enc) 
{
	tjhandle handle=NULL;

	if(enc->quality==0)
		enc->quality=75;
		
	if((handle=tjInitCompress())==NULL) _throwtj();
	_tj(tjCompressRGB2YUV420WithMark(handle, enc->idata, enc->width, enc->height, &enc->odata,
		&enc->obuff_size, enc->quality, enc->data_in_app1,enc->app1_data_size));

exit:
	if(handle) tjDestroy(handle);
	return enc->obuff_size;
}

/* example. */
#ifdef ENC_TEST

#define iwidth 3000
#define iheight 2000
#define buf_size (iwidth*iheight)

char jpeg_dat[buf_size*3];
char jpeg_out[buf_size*3];
int main() {
	FILE* fp;
	int data_size,i;
	jpeg_enc_t enc;
	
	struct timeval start;
	struct timeval end;
	float time_use=0;
	
	memset(&enc,0,sizeof(jpeg_enc_t));
	
	enc.width=iwidth;
	enc.height=iheight;
	enc.quality=75;
	enc.idata = jpeg_dat;
	enc.ibuff_size = buf_size*3;
	enc.odata = jpeg_dat;
	enc.obuff_size = buf_size*3;
	
	fp=fopen("test1.jpeg","wb");
	if(!fp) {
		LOGD(" open error\n");
		exit(1);
	}
	for(i=0;i<buf_size;i+=3) {
		jpeg_dat[i]=0;
		jpeg_dat[i+1]=0;
		jpeg_dat[i+2]=0xff;
	}
	for(i=buf_size;i<buf_size*2;i+=3) {
		jpeg_dat[i]=0;
		jpeg_dat[i+1]=0xff;
		jpeg_dat[i+2]=0;
	}
	for(i=buf_size*2;i<buf_size*3;i+=3) {
		jpeg_dat[i]=0xff;
		jpeg_dat[i+1]=0;
		jpeg_dat[i+2]=0;
	}
	gettimeofday(&start,NULL); 
	for(i=0;i<20;i++) {
		enc.obuff_size = buf_size*3;
		data_size=encode_jpeg(&enc);
	}
	gettimeofday(&end,NULL); 
	time_use=(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);
	LOGD("time_use in libjpeg2 %f\n",time_use/20);
	fwrite(jpeg_dat,1,data_size,fp);
	fclose(fp);
	
/*******************************************/
    LOGD("start decoder2\n"); 
    memset(&enc,0,sizeof(jpeg_enc_t));
	enc.width=iwidth;
	enc.height=iheight;
	enc.quality=75;
	enc.idata = jpeg_dat;
	enc.ibuff_size = buf_size*3;
	enc.odata = jpeg_out;
	enc.obuff_size = buf_size*3;
	
	fp=fopen("test2.jpeg","wb");
	if(!fp) {
		LOGD(" open error\n");
		exit(1);
	}
	for(i=0;i<buf_size;i+=3) {
		jpeg_dat[i]=0;
		jpeg_dat[i+1]=0;
		jpeg_dat[i+2]=0xff;
	}
	for(i=buf_size;i<buf_size*2;i+=3) {
		jpeg_dat[i]=0;
		jpeg_dat[i+1]=0xff;
		jpeg_dat[i+2]=0;
	}
	for(i=buf_size*2;i<buf_size*3;i+=3) {
		jpeg_dat[i]=0xff;
		jpeg_dat[i+1]=0;
		jpeg_dat[i+2]=0;
	}
	gettimeofday(&start,NULL);
	for(i=0;i<20;i++) {
		enc.obuff_size = buf_size*3;
		data_size=encode_jpeg2(&enc);
	}
	gettimeofday(&end,NULL); 
	time_use=(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);
	LOGD("time_use in turbojpeg2 %f\n",time_use/20);
	if(data_size)
		fwrite(jpeg_out,1,data_size,fp);
	else 
		LOGD("fucking error\n");
	fclose(fp);
	
	return 0;
}
#endif
