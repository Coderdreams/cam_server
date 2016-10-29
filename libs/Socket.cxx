/**************************************************
  Simple Socket abstract class to use with Tcp or Udp
  Author: Pablo Baena
**************************************************/
#include "Exceptions.hxx"
#include "Socket.hxx"
#include <errno.h>


// Beware of socket signals

Socket::Socket()
{
  soket_ = -1;
  port_ = -1;
  isBlocking_ = true;
}

Socket::~Socket()
{
  this->close( soket_ );
}

void Socket::close( int socket )
{
  if ( !isValid( socket ) ) return;

  ::close( socket );
}

int Socket::id( void ) const
{
  return soket_;
}

int Socket::port( void ) const
{
  return port_;
}

bool Socket::isBlocking ( void ) const
{
  return isBlocking_;
}

bool Socket::isValid( int socket )
{
  return socket != -1;
}

void Socket::setBlocking( bool enable )
{
  if ( !isValid( soket_ ) ) return;

  int tmp = ::fcntl(soket_, F_GETFL, 0);
  if ( tmp >= 0 )
  {
    tmp = ::fcntl( soket_, F_SETFL, enable ? (tmp &~ O_NONBLOCK) : (tmp | O_NONBLOCK) );
    isBlocking_ = enable;
  }
}

int Socket::writeBuffer(int socket, const void *buffer, long bufferSize, int flags )
{
  int result;
  if ( !isValid( socket ) ) return -1;

  // avoiding SIG_PIPE the hacky way for now
  if (flags == 0) {
    flags = MSG_NOSIGNAL;
  }

  result = send( socket, (const char *)buffer, bufferSize, flags );
  if ( result == -1 )
  {
    // This is not an error. Operation should continue because this is a nonblockable socket.
    if (errno == EAGAIN || errno == EWOULDBLOCK)
      return result;
    else
      throw SocketIOErr (errno); // Just to make the user code a little less messy.
  }
  return result;
}

int Socket::readBuffer(int socket, void *buffer, long bufferSize, int flags )
{
  int result;
  if ( !isValid( socket ) ) return -1;
  result = recv( socket, (char *)buffer, bufferSize, flags );

  if ( result == -1 )
  {
    // This is not an error. Operation should continue because this is a nonblockable socket.
    if (errno == EAGAIN || errno == EWOULDBLOCK)
      return result;
    else
      throw SocketIOErr (errno); // Just to make the user code a little less messy.
  }

  return result;
}

