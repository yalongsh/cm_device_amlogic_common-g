#ifndef AMVIDEO_UTILS_H_
#define AMVIDEO_UTILS_H_

void amvideo_overlay_get_disp(int *width, int *height);
int amvideo_overlay_open(int id);
int amvideo_overlay_init(int fd, uint32_t w, uint32_t h, uint32_t fmt);
int amvideo_overlay_stream_on(int fd);
int amvideo_overlay_stream_off(int fd);
int amvideo_overlay_set_position(int fd, int32_t x, int32_t y, int32_t w,
                              int32_t h);
int amvideo_overlay_get_position(int fd, int32_t *x, int32_t *y, int32_t *w,
                              int32_t *h);
int amvideo_overlay_set_crop(int fd, uint32_t x, uint32_t y, uint32_t w,
                              uint32_t h);
int amvideo_overlay_get_crop(int fd, uint32_t *x, uint32_t *y, uint32_t *w,
                              uint32_t *h);
int amvideo_overlay_set_rotation(int fd, int degree, int step);

enum {
  AMVIDEO_OVERLAY_PLANE_GRAPHICS,
  AMVIDEO_OVERLAY_PLANE_VIDEO,
};

#endif  // AMVIDEO_UTILS_H_
