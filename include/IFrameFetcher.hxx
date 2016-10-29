#ifndef IFRAMEFETCHER_HXX
#define IFRAMEFETCHER_HXX

class IFrameFetcher
{
  public:
    virtual char* readFrame(int* bufferSize) = 0;
};

#endif // (IFRAMEFETCHER_HXX)