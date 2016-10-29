#include <linux/types.h>
#include <linux/videodev.h>

#include "ICapture.hxx"

#ifndef CAPTUREV4L1_H
#define CAPTUREV4L1_H

class CaptureV4L1 : public ICapture
{
  public:
    CaptureV4L1(const char* dev_name);
    virtual ~CaptureV4L1();
    void init_device();
    void start_capturing();
    int* getImageSize();
    unsigned char* read_frame(int* bufferSize, int* palette, int* width, int* height);

  private:
    int get_brightness_adj(unsigned char* image, long size, int *brightness);
    int fd; 
    unsigned char *buffer, *src;
    int bpp, r, g, b;
    const char* dev_name;
    struct video_capability cap;
    struct video_window win;
    struct video_picture vpic;
};

#endif // (CAPTUREV4L1_H)