/**************************************************
 Simple TCP socket server class.

 Maybe a few methods to return exceptions on error
 could be fine, but I'm worried about perfomance
 issues.
 Author: Pablo Baena
**************************************************/

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "Socket.hxx"

class TcpServer : public Socket
{
 public:
  TcpServer( int port );

  ~TcpServer();

  // Creates the socket. Throws SocketCreationErr exception on error.
  void create ();

  /* 
     Extract the first pending connection request from the queue and create a new connection, 
     returning the descriptor for the accepted socket.

     If no connection requests are pending and the socket has not
     been set non-blocking, this function will block until a connection
     is present.  If an irrecuperable error occurs, SocketCreationErr exception is thrown.
  */
  int accept();

  // Static method to use on external sockets.
  static int accept( int socket);

  // Write a buffer over the socket connection.  Returns the number of bytes written or -1 if an error occurs.
  int writeBuffer(const void *buffer, long bufferSize, int flags = 0);

  // Read a buffer from the socket connection.  Returns the number of bytes read, 0 if connection closed, 
  // or SocketIOErr exception on irrecuperable errors.
  int readBuffer(void *buffer, long bufferSize, int flags = 0);

 protected:

};

#endif // defined(TCPSERVER_H)
