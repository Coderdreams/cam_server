#include "Exceptions.hxx"
#include "SelectHttpServer.hxx"
#include <stdio.h>

// Take away
#include <errno.h>
/**
 * Constructor
 *
 */
SelectHttpServer::SelectHttpServer(int _startPortNumber) 
  : max_conn(1), max_sock(0), startPortNumber(_startPortNumber)
{
}

/**
 * Destructor
 *
 */
SelectHttpServer::~SelectHttpServer()
{
  for (int ns=0; ns < server_socks.size(); ns++) {
    delete server_socks[ns];
  }
}

/**
 * init_server
 *
 */
void SelectHttpServer::create()
{
  init_server ();
  startProcessing ();
}

/**
 * init_server
 *
 */
void SelectHttpServer::init_server()
{
  printf("Init SelectHttpServer.");

  FD_ZERO (&read_set_orig);
  FD_ZERO (&write_set_orig);

  try
  {
    for (int i=0, p=startPortNumber; i < max_conn; i++, p++)
    {
      TcpServer* conn = new TcpServer(p);
      conn->create ();

      FD_SET (conn->id(), &read_set_orig);

      max_sock = conn->id();
      printf("http socket created %d", max_sock);
      server_socks.push_back(conn);
    }
  }
  catch (SocketCreationErr &ex)
  {
    printf("Init http server failed.");
  }
}

void SelectHttpServer::startProcessing ()
{
  int result;
  struct timeval tm;
  fd_set read_set, write_set;

  for (;;)
  {
  
    tm.tv_sec = 0; /* Timeout in seconds */
    tm.tv_usec = 50000; // Timeout in usec for simulating FPS, this is actually like polling
                        // but otherwise I must use threads
    //tm.tv_usec = 10000000; // i'm testing

    FD_ZERO (&read_set);
    FD_ZERO (&write_set);
    read_set = read_set_orig;
    write_set = write_set_orig;

    result = select (max_sock + 1, &read_set, &write_set, NULL, &tm); 
    if (result == -1)
    {
      printf("Select failed err: %d desc: %s.", errno, strerror(errno));
      return;
    }

    if (result > 0) 
    {
      //printf("Got activity!");
      for (int i=0; i<=max_sock; i++) 
      {
	if (FD_ISSET(i, &read_set)) 
	{
	  bool isServerSocket = false;
	  for (int ns=0; ns < server_socks.size(); ns++)
	    if (server_socks[ns]->id() == i)
	      isServerSocket = true;

	  handleRequest (i, isServerSocket);
	}
      }

      for (int i=0; i<=max_sock; i++) 
      {
	if (FD_ISSET(i, &write_set)) 
	{
	  writeClientResponse (i);
	}
      }
    }

    // If an event ocurred or timeout was reached, let's send data
    // I thought of only triggering this on timeout, but there can
    // be times where there are lots of incoming connections that
    // would prevent this to be reached (but it would be rare)
    if (!timeoutReached ()) {
        break;
    }

  }
}

/**
 * handleCommand
 * 
 */
void SelectHttpServer::handleRequest(int socket, bool isServer)
{
  int curfd;
  if (isServer) 
  { 
    //printf("Server socket: %d accepting", socket);
    try 
    {
      if ((curfd = TcpServer::accept(socket)) >0) 
      {
	//printf("Server socket  %d connection accepted.", socket);
	FD_SET(curfd, &read_set_orig);
	if (max_sock < curfd)
	  max_sock = curfd;
      }
    }
    catch (SocketCreationErr &ex)
    {
      printf("Error on SelectHttpServer socket accept:", ex.getErrorDescription());
      // TODO: restart the socket?
      return;
    }

    return;
  }

  // is client
  if (handleClientRequest (socket)) // this should return the header 
  {
    // clear from readfs
    FD_CLR(socket, &read_set_orig);
    // return to select
  }
  else
    FD_CLR(socket, &read_set_orig); // there was an error reading
}

void SelectHttpServer::scheduleForWriting (int socket)
{
    // set socket on writefds
    FD_SET (socket, &write_set_orig);
}

void SelectHttpServer::unscheduleFromWriting (int socket)
{
    FD_CLR (socket, &write_set_orig);
}

void SelectHttpServer::closeSocket (int socket)
{
  TcpServer::close (socket);
  //printf("Close socket %d.", socket);
  unscheduleFromWriting (socket);
}

int SelectHttpServer::writeBuffer(int soket, const void *buffer, long bufferSize )
{
  return Socket::writeBuffer (soket, buffer, bufferSize, 0);
}

int SelectHttpServer::readBuffer(int soket, void *buffer, long bufferSize )
{
  return Socket::readBuffer (soket, buffer, bufferSize, 0);
}


