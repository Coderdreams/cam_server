/**********************************************
 HttpClient: keeps track of client connections
 Author: Pablo Baena
**********************************************/
#include "Camera.hxx"

#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#define HEADSIZE 4096

class HttpClient
{
  public:
    HttpClient(int _socket, const char* cam_type);
    ~HttpClient();
    int id ();

    char header[HEADSIZE];
    FStatus status;
    double lastFrameTime;
    int count; 
    Camera* camera;

  private:
    int socket;
    // A few stubs just in case we use non-blocking sockets
    int respOffset; 
};

#endif // (HTTPCLIENT_H)
