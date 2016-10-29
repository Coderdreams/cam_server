/**********************************************
 SelectHttpServer
 Author: Pablo Baena
**********************************************/

#ifndef SELECTHTTPSERVER_H
#define SELECTHTTPSERVER_H

#include "TcpServer.hxx"
#include <vector>

class SelectHttpServer
{
  public:
    SelectHttpServer(int startPortNumber);
    virtual ~SelectHttpServer();
    void create();

  protected:
    virtual bool handleClientRequest (int sock) = 0;
    virtual void writeClientResponse (int sock) = 0;
    virtual bool timeoutReached () = 0;

    int readBuffer (int sock, void* buffer, long bufferSize);
    int writeBuffer (int sock, const void* buffer, long bufferSize);
    void closeSocket (int sock);
    void scheduleForWriting (int sock);
    void unscheduleFromWriting (int sock);
    
  private:
    fd_set read_set_orig, write_set_orig;
    std::vector<TcpServer*> server_socks;
    int max_conn;
    int max_sock;
    int startPortNumber;

    void init_server();
    void handleRequest (int sock, bool isServer);
    void startProcessing ();
};

#endif // (SELECTHTTPSERVER_H)
