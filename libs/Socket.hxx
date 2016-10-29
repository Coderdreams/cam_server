/**************************************************
  Simple Socket abstract class to use with Tcp or Udp
  Author: Pablo Baena
**************************************************/

#ifndef SOCKET_H
#define SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

class Socket
{
 public:
  
  enum ProtocolType {
    PROTO_TCP,
    PROTO_UDP
    };

  Socket();

  virtual ~Socket();

  static void close( int socket );

  // Return the socket descriptor.
  int id( void ) const;

  // Return the socket port number.
  int port( void ) const;

  // Gets blocking attribute of socket
  bool isBlocking ( ) const;

  // Sets blocking attribute of socket
  void setBlocking( bool enable );

  // Returns true if the socket descriptor is valid.
  static bool isValid( int socket );

  // Overridable method to write to this socket. Returns the number of bytes written or -1 if an error occurs.
  virtual int writeBuffer(const void *buffer, long bufferSize, int flags = 0) = 0;

  // Overridable method to read from this socket. Returns the number of bytes read or -1 if an error occurs.
  virtual int readBuffer(void *buffer, long bufferSize, int flags = 0) = 0;

  // Write a buffer via the specified socket. Static method to use from the outside.
  static int writeBuffer(int socket, const void *buffer, long bufferSize, int flags );

  // Read a buffer via the specified socket.  Static method to use from the outside.
  static int readBuffer(int socket, void *buffer, long bufferSize, int flags );

 protected:

  int soket_;
  int port_;
  bool isBlocking_;

};

#endif // SOCKET_H
