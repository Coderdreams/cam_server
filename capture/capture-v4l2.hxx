
#include <asm/types.h>		/* for videodev2.h */
#include <linux/videodev2.h>

#include "ICapture.hxx"

#ifndef CAPTUREV4L2_H
#define CAPTUREV4L2_H

typedef enum
{
    IO_METHOD_READ,
    IO_METHOD_MMAP,
    IO_METHOD_USERPTR,
} io_method;

struct buffer
{
    void *start;
    size_t length;
};


class CaptureV4L2 : public ICapture
{
  public:
    CaptureV4L2(const char* dev_name);
    virtual ~CaptureV4L2();
    void init_device();
    void start_capturing();
    int* getImageSize();
    unsigned char* read_frame(int* bufferSize, int* palette, int* width, int* height);

  private:
    void open_device();
    void close_device();
    void uninit_device(void);
    void stop_capturing(void);
    void mainloop();

    int xioctl(int fd, int request, void *arg);
    void errno_exit(const char *s);
    void init_userp(unsigned int buffer_size);
    void init_mmap(void);
    void init_read(unsigned int buffer_size);
    const char* dev_name;
    io_method io;
	/* can be V4L2_PIX_FMT_YUV420 or V4L2_PIX_FMT_PWC2 */
    int pixel_format;
    int fd;
    buffer *buffers;
    unsigned int n_buffers;
    unsigned int width;
    unsigned int height;
    unsigned int count;
};

#endif // (CAPTUREV4L2_H)