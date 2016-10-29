/**********************************************
 Simple HttpServer
 Author: Pablo Baena
**********************************************/

#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "SelectHttpServer.hxx"
#include "HttpClient.hxx"
#include "IFrameFetcher.hxx"
#include <list>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>


class HttpServer : public SelectHttpServer
{
  public:
    HttpServer(IFrameFetcher* fetcher, int portNumber);
    virtual ~HttpServer();
    void startThread();
    void stopThread();
    void startServer();
    void serveBuffer (const char* buffer, int siz);

  protected:
    bool handleClientRequest (int sock);
    void writeClientResponse (int sock);
    bool timeoutReached ();
    void Stats ();
    
  private:
    // this is a list because we have to delete from it all the time (when returning a snapshot)
    std::list<HttpClient*> vClient; 

    char* image_buffer;
    int image_size;
    int count, secondsRunning;
    double lastStatsTime, lastUTime, lastSTime;
    int bytesSent;

    volatile bool m_stoprequested;
    boost::shared_ptr<boost::thread> m_thread;
    boost::mutex m_mutex;
 
    void getImage ();
    void deleteFromClientVec (int socket);
    double getCurrentTime ();
    void timer_handler (int signum);
    HttpClient* getClientFromVec (int socket);
    IFrameFetcher* frameFetcher;
};

#endif // (HTTPSERVER_H)
