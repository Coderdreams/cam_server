/**************************************************
 * Simple Tcp socket wrapper
 * Author: Pablo Baena
 **************************************************/
#include "Exceptions.hxx"
#include "TcpServer.hxx"
#include <errno.h>

TcpServer::TcpServer( int port )
{
  port_ = port;
}

TcpServer::~TcpServer()
{
}

void TcpServer::create ()
{
  // Create the server-side socket
  soket_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (soket_ < 0) {
    throw SocketCreationErr (errno);
  }

  int flag = 1;
  int result = setsockopt( soket_, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int) );
  if (result < 0) {
    throw SocketCreationErr (errno);
  }
  
  result = setsockopt( soket_, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(int) );
  if (result < 0) {
    throw SocketCreationErr (errno);
  }

  result = setsockopt( soket_, SOL_SOCKET, SO_KEEPALIVE, (char *)&flag, sizeof(int) );
  if (result < 0) {
    throw SocketCreationErr (errno);
  }

  /* Isn't defined
  result = setsockopt( soket_, SOL_SOCKET, SO_REUSEPORT, (char *)&flag, sizeof(int) );
  if (result < 0) {
    throw SocketCreationErr (errno);
  }
  */

  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons (port_);

  // Bind socket to the appropriate port and interface (INADDR_ANY)
  if ( bind( soket_, (struct sockaddr *)&address, sizeof(address) ) < 0 ) {
    throw SocketCreationErr (errno);
  }

  // Listen for incoming connection(s)
  if ( listen( soket_, 1 ) < 0 ) {
    throw SocketCreationErr (errno);
  }

}

int TcpServer::accept( void )
{
  return accept(soket_);
}

int TcpServer::accept( int socket )
{
  int result = ::accept( socket, NULL, NULL );
  if (result == -1)
  { 
    // These are not errors according to the manpage.
    if ((errno == ENETDOWN || errno == EPROTO || errno == ENOPROTOOPT || errno == EHOSTDOWN ||
	 errno == ENONET || errno == EHOSTUNREACH || errno == EOPNOTSUPP || errno == ENETUNREACH))
      return result;
    else
      throw SocketCreationErr (errno); // Just to make the user code a little less messy.
  }

  return result;
}

int TcpServer::writeBuffer(const void *buffer, long bufferSize, int flags )
{
  return Socket::writeBuffer (soket_, buffer, bufferSize, flags);
}

int TcpServer::readBuffer(void *buffer, long bufferSize, int flags )
{
  return Socket::readBuffer (soket_, buffer, bufferSize, flags);
}
