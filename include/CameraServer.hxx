#include "HttpServer.hxx"
#include "ImageProcessor.hxx"
#include "capture-v4l1.hxx"
#include "capture-v4l2.hxx"
#include "IFrameFetcher.hxx"

#ifndef CAMERASERVER_HXX
#define CAMERASERVER_HXX

class CameraServer : public IFrameFetcher
{
  public:
    CameraServer(char* dev_name, unsigned int version, ImageProcessorOptions options, int portNumber);
    ~CameraServer();
    char* readFrame(int* size);
    void start();
    void startThread();
    void setPositionedText(char *text);
    void clearPositionedText();
    int* getImageSize();
    void stopThread();

  private:
    ImageProcessor* processor;
    ICapture* capture;
    HttpServer* server;
    int portNumber;
};

#endif // (CAMERASERVER_HXX)
