package org.com.uy.scanntech.surveillance;

public class CameraServerOptions {
	private String dev_name = null;
	private Integer v4l_version = null;
	private Boolean banner = null;
	private String title = null;
	private String subtitle = null;
	private String positionedText = null;
	private Integer portNumber = null;

	public void setDeviceName(String dev_name) {
		this.dev_name = dev_name;
	}
	
	public void setV4LVersion(int v4l_version) {
		this.v4l_version = v4l_version;
	}
	
	public void setBanner(boolean banner) {
		this.banner = banner;
	}
	
	public void setTitle(String title) {
		this.title = title;
	}
	
	public void setSubtitle(String subtitle) {
		this.subtitle = subtitle;
	}
	
	public void setDefaultPositionedText(String positionedText) {
		this.positionedText = positionedText;
	}
	
	public void setPortNumber(int portNumber) {
		this.portNumber = portNumber;
	}
}
