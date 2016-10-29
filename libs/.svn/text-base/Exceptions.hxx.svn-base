/**************************************************
 Exceptions thrown by the socket libs
 Author: Pablo Baena
**************************************************/

#ifndef SOCKETEXCEPTIONS_H
#define SOCKETEXCEPTIONS_H

#include <string.h>

class SocketErr
{

public:
  SocketErr(int _err) : errno_(_err) { }
  ~SocketErr () { }
  int getErrorCode () { return errno_; }
  char* getErrorDescription () { return strerror (errno_); }

private:
  int errno_;
};

class SocketCreationErr : public SocketErr
{
public:
  SocketCreationErr(int _err) : SocketErr (_err) { }
  ~SocketCreationErr () { }
};

class SocketIOErr : public SocketErr
{
public:
  SocketIOErr(int _err) : SocketErr (_err) { }
  ~SocketIOErr () { }
};

#endif // (SOCKETEXCEPTIONS_H)
