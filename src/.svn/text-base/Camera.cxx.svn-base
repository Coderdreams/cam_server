#include "Camera.hxx"
#include <cstring>

/**
 * Constructor
 *
 */
Camera::Camera()
{
  
}

/**
 * Destructor
 *
 */
Camera::~Camera()
{
}


const char* Camera::getSnapshotResponse ()
{
  return "HTTP/1.0 200 OK\r\nContent-Type: image/jpeg\r\nContent-Length: %d\r\n\r\n";
}

const char* Camera::getMJPEGResponseHeader ()
{
  return "HTTP/1.0 200 OK\r\nServer: Camera Web Server/1.0\r\nAuthor: Pablo B.\r\nMIME-version: 1.0\r\nCache-Control: no-cache\r\nContent-Type: multipart/x-mixed-replace;boundary=video boundary\r\n\r\n";
}

const char* Camera::getMJPEGImageResponse ()
{
  return "--video boundary\r\nContent-type: image/jpeg\r\nContent-length: %d\r\n\r\n";
}

const int Camera::getDefaultFPS ()
{
  return 5;
}

const char* Camera::getDefaultResolution ()
{
  return "320x200";
}

void Camera::parseClientRequest (const char* header)
{
  //TODO: father, give me hands
  if ((strstr(header, "image.cgi") != NULL) || (strstr(header, ".jpg") != NULL))
    return_fmt = EFormat(SNAPSHOT);
  else
    return_fmt = EFormat(MJPEG);
}

EFormat Camera::getFormat ()
{
  return return_fmt;
}


