
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>

#include "capture-v4l1.hxx"

CaptureV4L1::CaptureV4L1(const char* _dev_name) {
    fd = -1;
    bpp = 24;
    dev_name = _dev_name;
}

CaptureV4L1::~CaptureV4L1() {
    close(fd);
}

int CaptureV4L1::get_brightness_adj(unsigned char *image, long size, int *brightness) {
    long i, tot = 0;
    for (i=0;i<size*3;i++)
        tot += image[i];
    *brightness = (128 - tot/(size*3))/3;
    return !((tot/(size*3)) >= 126 && (tot/(size*3)) <= 130);
}

void CaptureV4L1::init_device()
{
    fd = open(dev_name, O_RDONLY);
    if (fd < 0) {
        perror(dev_name);
        exit(1);
    }

    if (ioctl(fd, VIDIOCGCAP, &cap) < 0) {
        perror("VIDIOGCAP");
        fprintf(stderr, "(%s not a video4linux device?)\n", dev_name);
        close(fd);
        exit(1);
    }

    fprintf(stderr, "%d type\n", cap.type);

	if(cap.type & VID_TYPE_CAPTURE)       fprintf(stderr, "- CAPTURE\n");
	if(cap.type & VID_TYPE_TUNER)         fprintf(stderr, "- TUNER\n");
	if(cap.type & VID_TYPE_TELETEXT)      fprintf(stderr, "- TELETEXT\n");
	if(cap.type & VID_TYPE_OVERLAY)       fprintf(stderr, "- OVERLAY\n");
	if(cap.type & VID_TYPE_CHROMAKEY)     fprintf(stderr, "- CHROMAKEY\n");
	if(cap.type & VID_TYPE_CLIPPING)      fprintf(stderr, "- CLIPPING\n");
	if(cap.type & VID_TYPE_FRAMERAM)      fprintf(stderr, "- FRAMERAM\n");
	if(cap.type & VID_TYPE_SCALES)        fprintf(stderr, "- SCALES\n");
	if(cap.type & VID_TYPE_MONOCHROME)    fprintf(stderr, "- MONOCHROME\n");
	if(cap.type & VID_TYPE_SUBCAPTURE)    fprintf(stderr, "- SUBCAPTURE\n");
	if(cap.type & VID_TYPE_MJPEG_ENCODER) fprintf(stderr, "- MJPEG_ENCODER\n");
	fprintf(stderr, "cap.maxwidth=%d\n", cap.maxwidth);
	fprintf(stderr, "cap.maxheight=%d\n", cap.maxheight);
	fprintf(stderr, "cap.minwidth=%d\n", cap.minwidth);
	fprintf(stderr, "cap.minheight=%d\n", cap.minheight);

    if (ioctl(fd, VIDIOCGWIN, &win) < 0) {
        perror("VIDIOCGWIN");
        close(fd);
        exit(1);
    }

    if (ioctl(fd, VIDIOCGPICT, &vpic) < 0) {
        perror("VIDIOCGPICT");
        close(fd);
        exit(1);
    }


    if (cap.type & VID_TYPE_MONOCHROME) {
        vpic.depth=8;
        vpic.palette=VIDEO_PALETTE_GREY;	/* 8bit grey */
        if(ioctl(fd, VIDIOCSPICT, &vpic) < 0) {
            vpic.depth=6;
            if(ioctl(fd, VIDIOCSPICT, &vpic) < 0) {
	            vpic.depth=4;
	            if(ioctl(fd, VIDIOCSPICT, &vpic) < 0) {
	                fprintf(stderr, "Unable to find a supported capture format.\n");
	                close(fd);
	                exit(1);
	            }
            }
        }
    } else {
        vpic.depth=24;
        vpic.palette=VIDEO_PALETTE_RGB24;
    
        if(ioctl(fd, VIDIOCSPICT, &vpic) < 0) {
            vpic.palette=VIDEO_PALETTE_RGB565;
            vpic.depth=16;
            if(ioctl(fd, VIDIOCSPICT, &vpic)==-1) {
	            vpic.palette=VIDEO_PALETTE_RGB555;
	            vpic.depth=15;
	            if(ioctl(fd, VIDIOCSPICT, &vpic)==-1) {
	                fprintf(stderr, "Unable to find a supported capture format.\n");
	                exit(1);
	            }
            }
        }
    }

    buffer = (unsigned char*) malloc(win.width * win.height * bpp);
    if (!buffer) {
        fprintf(stderr, "Out of memory.\n");
        exit(1);
    }
}  

int* CaptureV4L1::getImageSize() {
    int* imageSize = new int[2];
    imageSize[0] = win.width;
    imageSize[1] = win.height;
    return imageSize;
}

void CaptureV4L1::start_capturing() {
    int f;
    int count = 10;
    unsigned int i, src_depth;
    do {
        int newbright;
        read(fd, buffer, win.width * win.height * bpp);
        f = get_brightness_adj(buffer, win.width * win.height, &newbright);
        if (f) {
            vpic.brightness += (newbright << 8);
            fprintf(stderr, "%d\n", vpic.brightness);
            if(ioctl(fd, VIDIOCSPICT, &vpic)==-1) {
	            perror("VIDIOSPICT");
	            break;
            }
        }
    } while (f && count-- > 0);

//    while(true) {
//        read(fd, buffer, win.width * win.height * bpp);
//        processor.process_image(buffer, win.width * win.height, 0, win.width, win.height);
//    }
}

unsigned char* CaptureV4L1::read_frame(int* bufferSize, int* palette, int* width, int* height) {
    read(fd, buffer, win.width * win.height * bpp);
    *bufferSize = win.width * win.height;
    *width = win.width;
    *height = win.height;
    return buffer;
}


