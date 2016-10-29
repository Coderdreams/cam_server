package org.com.uy.scanntech.surveillance;

public class CameraServer {
	
	static
	{
		System.load("/usr/local/lib/libcameraserver.so");
	}
	
	public native void startServer(CameraServerOptions options);
	
	public native void stopServer();
	
	public native void setPositionedText(String positionedText);
	
	public native void clearPositionedText();
	
	public native int[] getImageSize();

}
