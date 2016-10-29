CPP=g++
XCFLAGS=-g  -DDEBUG # -pg -Wall -pedantic

incldir=include
srcdir=src
libsdir=libs
capturedir=capture
java_incldir=/usr/lib/jvm/java-6-sun-1.6.0.10/include
java_linux_incldir=/usr/lib/jvm/java-6-sun-1.6.0.10/include/linux

CPPCMD=$(CPP) $(CFLAGS) $(XCFLAGS) -I$(incldir) -I$(libsdir) -I$(capturedir) -I$(java_incldir) -I$(java_linux_incldir)

CameraObjs=Socket.o TcpServer.o  SelectHttpServer.o Camera.o HttpClient.o HttpServer.o ImageProcessor.o capture-v4l1.o capture-v4l2.o CameraServer.o

all: libcameraserver.so camera

libcameraserver.so: $(CameraObjs) Server.o
	$(CPPCMD) -lgd -lpthread -lboost_thread -shared -o libcameraserver.so $(CameraObjs) Server.o

camera: $(CameraObjs) Main.o
	$(CPPCMD) -lgd -lpthread -lboost_thread  -o camera $(CameraObjs) Main.o

Main.o: $(srcdir)/Main.cxx  $(incldir)/CameraServer.hxx
	$(CPPCMD) -c $(srcdir)/Main.cxx

Server.o: $(srcdir)/Server.cxx  $(incldir)/CameraServer.hxx $(incldir)/Server.hxx
	$(CPPCMD) -c $(srcdir)/Server.cxx

CameraServer.o: $(srcdir)/CameraServer.cxx  $(incldir)/CameraServer.hxx $(capturedir)/capture-v4l1.hxx $(capturedir)/capture-v4l2.hxx $(capturedir)/ImageProcessor.hxx $(incldir)/IFrameFetcher.hxx 
	$(CPPCMD) -c $(srcdir)/CameraServer.cxx

ImageProcessor.o: $(capturedir)/ImageProcessor.cxx  $(capturedir)/ImageProcessor.hxx 
	$(CPPCMD) -c $(capturedir)/ImageProcessor.cxx

capture-v4l1.o: $(capturedir)/capture-v4l1.cxx  $(capturedir)/capture-v4l1.hxx $(capturedir)/ICapture.hxx
	$(CPPCMD) -c $(capturedir)/capture-v4l1.cxx

capture-v4l2.o: $(capturedir)/capture-v4l2.cxx  $(capturedir)/capture-v4l2.hxx $(capturedir)/ICapture.hxx
	$(CPPCMD) -c $(capturedir)/capture-v4l2.cxx

Socket.o: $(libsdir)/Socket.cxx $(libsdir)/Socket.hxx 
	$(CPPCMD) -c $(libsdir)/Socket.cxx 

TcpServer.o: $(libsdir)/TcpServer.cxx $(libsdir)/TcpServer.hxx $(libsdir)/Socket.hxx $(libsdir)/Exceptions.hxx
	$(CPPCMD) -c $(libsdir)/TcpServer.cxx

Camera.o: $(srcdir)/Camera.cxx $(incldir)/Camera.hxx 
	$(CPPCMD) -c $(srcdir)/Camera.cxx 

HttpClient.o: $(srcdir)/HttpClient.cxx $(incldir)/HttpClient.hxx $(incldir)/Camera.hxx
	$(CPPCMD) -c $(srcdir)/HttpClient.cxx 

HttpServer.o: $(srcdir)/HttpServer.cxx $(incldir)/HttpServer.hxx $(libsdir)/SelectHttpServer.hxx $(libsdir)/Exceptions.hxx $(incldir)/HttpClient.hxx $(incldir)/Camera.hxx $(incldir)/IFrameFetcher.hxx
	$(CPPCMD) -c $(srcdir)/HttpServer.cxx 

SelectHttpServer.o: $(libsdir)/SelectHttpServer.cxx $(libsdir)/SelectHttpServer.hxx $(libsdir)/TcpServer.hxx $(libsdir)/Exceptions.hxx 
	$(CPPCMD) -c $(libsdir)/SelectHttpServer.cxx 

clean:
	rm -f *.o *~ camera libcameraserver.so $(incldir)/*~ $(srcdir)/*~ $(libsdir)/*~ $(capturedir)/*~

install: all
	cp libcameraserver.so /usr/local/lib

