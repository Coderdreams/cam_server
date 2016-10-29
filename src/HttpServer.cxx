#include "Exceptions.hxx"
#include "HttpServer.hxx"
#include "HttpClient.hxx"
#include "Camera.hxx"
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <list>
#include <cassert>
#include <signal.h>
#include <string.h>
#include <stdio.h>

using namespace std;

void HttpServer::Stats ()
{
  struct rusage usage;
  double ut, st, tt;
  getrusage (RUSAGE_SELF, &usage);

  ut = usage.ru_utime.tv_sec+(usage.ru_utime.tv_usec/1000000.0);
  st = usage.ru_stime.tv_sec+(usage.ru_stime.tv_usec/1000000.0);
  tt = (ut - lastUTime) + (st - lastSTime);
/*  printf("%d,%d,%d,%f,%f,%f\n", secondsRunning, count, bytesSent, 
	 ut - lastUTime, st - lastSTime, tt);
*/
  fflush(NULL);
  count=0;
  bytesSent=0;
  lastUTime = ut;
  lastSTime = st;
  secondsRunning++;
}

/**
 * Constructor
 *
 */
HttpServer::HttpServer(IFrameFetcher* fetcher, int startPortNumber) 
  : m_stoprequested(false), SelectHttpServer(startPortNumber)
{
  bytesSent = count = secondsRunning = 0;
  lastUTime = lastSTime = 0.00;
  lastStatsTime = getCurrentTime ();
  m_stoprequested = false;
  frameFetcher = fetcher;
}

void HttpServer::startThread()
{
  assert(!m_thread);
  m_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&HttpServer::startServer, this)));
}

void HttpServer::stopThread()
{
  assert(m_thread);
  m_stoprequested = true;
  m_thread->join();
}

void HttpServer::startServer()
{
  create();
}

/*
void HttpServer::serveBuffer (const char* buffer, int buffer_size)
{
  boost::mutex::scoped_lock l(m_mutex);

  if (image_size != 0) {
    free(image_buffer);
  }
  image_size = buffer_size;
  image_buffer = (char*) malloc (buffer_size);
  assert(image_buffer != NULL);
 
  memcpy(image_buffer, buffer, buffer_size);
}
*/
  
/**
 * Destructor
 *
 */
HttpServer::~HttpServer()
{
  if (m_thread && m_stoprequested == false) {
    stopThread();
  }
}


bool HttpServer::handleClientRequest(int socket)
{
  HttpClient* client = new HttpClient (socket, "axis"); //TODO: set from cmdline
  vClient.push_back(client);

  client->status = FStatus(GETTING_CLIENT_HEADER);

  try {
    readBuffer (socket, client->header, HEADSIZE);
  }
  catch (SocketIOErr &ex)
  {
      deleteFromClientVec (socket);

      return false;
  }
  client->status = FStatus(GOT_HEADER);
  // We shouldn't make this, it's the client's job, change later
  client->camera->parseClientRequest (client->header);
  return true;
}

void HttpServer::deleteFromClientVec (int socket)
{
  closeSocket (socket); // This takes it out from the write fdset

  for (list<HttpClient*>::iterator p = vClient.begin(); p != vClient.end(); p++)
    if ((*p)->id() == socket)
    {
      delete *p;
      vClient.erase(p);
      break;
    }
}

HttpClient* HttpServer::getClientFromVec (int socket)
{

  for (list<HttpClient*>::iterator p = vClient.begin(); p != vClient.end(); p++)
    if ((*p)->id() == socket)
    {
      return *p;
    }

  return NULL;
}

bool HttpServer::timeoutReached ()
{
  double t = getCurrentTime ();

  if (m_stoprequested) {
    return false;
  }

  if ((t - lastStatsTime) >= 1.00)
  {
    Stats ();
    lastStatsTime = t;
  }
    
  // If it passed the 200 ms, we send the image
  HttpClient* cl;
  for (list<HttpClient*>::iterator p = vClient.begin(); p != vClient.end(); p++)
  {
    cl = *p;
    if (cl->status == FStatus(GOT_HEADER) || cl->status == FStatus(IS_WRITING_RESPONSE))
    {
        scheduleForWriting (cl->id());
    }
  }

  return true;
}

void HttpServer::getImage () 
{
  image_buffer = frameFetcher->readFrame(&image_size);
}

void HttpServer::writeClientResponse (int socket)
{
  char respHeader[256];
  char final[] = "\r\n";
  char final2[] = "\r\n\r\n";
  int sent;
  HttpClient* pclient = getClientFromVec (socket);
  assert (pclient != NULL);

  count++;

  getImage();

  if (image_buffer == NULL) {
    // no hay nada para enviar
    return;
  }

  if (pclient->camera->getFormat() == EFormat(SNAPSHOT))
  {
    sprintf (respHeader, pclient->camera->getSnapshotResponse(), image_size);

    try {
      sent = writeBuffer (socket, respHeader, strlen(respHeader));
      bytesSent+=sent;
      sent = writeBuffer (socket, image_buffer, image_size);
      bytesSent+=sent;
      sent = writeBuffer (socket, final, strlen(final));
      bytesSent+=sent;
    }
    catch (SocketIOErr &ex)
    {
      deleteFromClientVec (socket);
      //return false;
    }    

    // We should check here if the whole contents were written
    deleteFromClientVec (socket);
  }
  else if (pclient->camera->getFormat() == EFormat(MJPEG))
  {
    // Let's send frames
    if (pclient->status == FStatus(GOT_HEADER)) // This means we have to send the top header
    {
      sprintf (respHeader, pclient->camera->getMJPEGResponseHeader());
      try {
      	sent = writeBuffer (socket, respHeader, strlen(respHeader));
	    bytesSent+=sent;
      }
      catch (SocketIOErr &ex)
      {
        deleteFromClientVec (socket);
      }

      pclient->status = FStatus(IS_WRITING_RESPONSE);
    }
    
    memset (respHeader, 0, 256);
    sprintf (respHeader, pclient->camera->getMJPEGImageResponse(), image_size);
    try {
      sent = writeBuffer (socket, respHeader, strlen(respHeader));
      bytesSent+=sent;
      writeBuffer (socket, image_buffer, image_size);
      bytesSent+=sent;
      writeBuffer (socket, final, strlen(final));
      bytesSent+=sent;
    }
    catch (SocketIOErr &ex)
    {
      deleteFromClientVec (socket);
      //return false;
    }

    pclient->lastFrameTime = getCurrentTime ();
    pclient->count++;
    unscheduleFromWriting (socket);
  }
  free(image_buffer);
}

double HttpServer::getCurrentTime ()
{
  timeval tim;
  gettimeofday(&tim, NULL);
  double t1=tim.tv_sec+(tim.tv_usec/1000000.0);
  return t1;
}
