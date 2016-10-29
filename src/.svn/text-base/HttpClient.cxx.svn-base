#include "Exceptions.hxx"
#include "HttpClient.hxx"
#include "Camera.hxx"
#include <cstring>


/**
 * Constructor
 *
 */
HttpClient::HttpClient(int _socket, const char* camera_type) : socket(_socket)
{
  memset(header, 0, HEADSIZE);
  lastFrameTime = 0.0;
  count = 0;
  
  if (strcmp("axis", camera_type) == 0) // stub: fill with other types when needed
    camera = new Camera ();
  else
    camera = new Camera ();
}
  
/**
 * Destructor
 *
 */
HttpClient::~HttpClient()
{
  delete camera;
}

/**
 * get socket id
 *
 */
int HttpClient::id()
{
  return socket;
}

