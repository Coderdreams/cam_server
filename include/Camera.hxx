/**********************************************
 Camera: by default we use the Axis protocol
 defined on: http://www.axis.com/techsup/cam_servers/dev/cam_http_api.htm
 Author: Pablo Baena
**********************************************/

#ifndef CAMERA_H
#define CAMERA_H

enum FStatus { GETTING_CLIENT_HEADER, GOT_HEADER, IS_WRITING_RESPONSE }; // flag
enum EFormat { SNAPSHOT, MJPEG }; // enum

class Camera
{
  public:
    Camera();
    ~Camera();
    virtual void parseClientRequest (const char* header);
    virtual const char* getSnapshotResponse ();
    virtual const char* getMJPEGResponseHeader ();
    virtual const char* getMJPEGImageResponse ();
    virtual const int getDefaultFPS ();
    virtual const char* getDefaultResolution ();
    virtual EFormat getFormat ();

  private:
    EFormat return_fmt;
};

#endif // (CAMERA_H)
