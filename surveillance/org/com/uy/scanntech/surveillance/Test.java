package org.com.uy.scanntech.surveillance;

public class Test {
	public static void main(String[] args) {
		
		System.out.println("levantando\n");
		

		CameraServerOptions cam_options = new CameraServerOptions();
		cam_options.setDeviceName("/dev/video0");
		cam_options.setPortNumber(10500);
		cam_options.setV4LVersion(1);
		cam_options.setBanner(true);
		cam_options.setTitle("hola");
		cam_options.setSubtitle("hola2");


		CameraServer cam = new CameraServer();
		cam.startServer(cam_options);
				
		int i = 0;
		while(true) {
			int[] size = cam.getImageSize();

			System.out.println("width: " + (size[0]) + " height: " + (size[1]) + "\n");	
			cam.clearPositionedText();
			cam.setPositionedText("10|20|RUN COUNT: " + (i++));
			cam.setPositionedText("10|40|SOME OTHER TEXT");
	
			try {
				java.lang.Thread.currentThread().sleep(1000);
			} catch (InterruptedException ie){
			}
		}
	}
}
