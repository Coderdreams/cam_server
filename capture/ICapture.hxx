#ifndef ICAPTURE_H
#define ICAPTURE_H

class ICapture
{
  public:
    virtual void init_device() = 0;
    virtual void start_capturing() = 0;
    virtual int* getImageSize() = 0;
    virtual unsigned char* read_frame(int* bufferSize, int* palette, int* width, int* height) = 0;
    virtual ~ICapture() {};
};

#endif // (ICAPTURE_H)