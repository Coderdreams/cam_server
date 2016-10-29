
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>             /* getopt_long() */

#include "CameraServer.hxx"

/**
 * main
 *
 */
static void usage(FILE * fp, int argc, char **argv)
{
    fprintf(fp,
        "Usage: %s [options]\n\n"
        "Options:\n"
        "-d | --device name   Video device name [/dev/video]\n"
        "-p | --port number   Port number to listen on [10100]\n"
//          "-f | --format FORMAT yuv (YUV420P), yuyv or raw(native)\n"
        "-h | --help          Print this message\n"
//          "-m | --mmap          Use memory mapped buffers\n"
//          "-r | --read          Use read() calls\n"
        "-v | --version       Specify V4L version 1 or 2\n"
//          "-s | --size   WxH    WIDTHxHEIGHT\n"
//          "-u | --userp         Use application allocated buffers\n"
        "-b | --banner       Display banner [true]\n"
        "-t | --title       Specify title\n"
        "-S | --subtitle       Specify subtitle\n"
        "-l | --positioned       Text with coordinates in the form x|y|text\n"
    "", argv[0]);
}

static const char short_options[] = "v:t:b:l:S:s:d:p:f:hmrs:u";

static const struct option long_options[] = {
  {"device", required_argument, NULL, 'd'},
  {"port", required_argument, NULL, 'p'},
  {"format", required_argument, NULL, 'f'},
  {"help", no_argument, NULL, 'h'},
  {"mmap", no_argument, NULL, 'm'},
  {"read", no_argument, NULL, 'r'},
  {"size", required_argument, NULL, 's'},
  {"version", required_argument, NULL, 'v'},
  {"userp", no_argument, NULL, 'u'},
  {"banner", no_argument, NULL, 'b'},
  {"title", required_argument, NULL, 't'},
  {"subtitle", required_argument, NULL, 'S'},
  {"positioned", required_argument, NULL, 'l'},
  {0, 0, 0, 0}
};

int main( int argc , char* argv[] )
{
    char* dev_name = strdup("/dev/video0");
    char* title = strdup("");
    char* subtitle = strdup("");
    char* positionedText = strdup("");
    int width, height;
    bool banner = false;
    unsigned int version = 1;
    int count = 10;
    int portNumber = 10100;

    for (;;)
    {
        int index;
        int c;

        c = getopt_long(argc, argv, short_options, long_options, &index);

        if (-1 == c)
            break;

        switch (c)
        {
            case 0:         // getopt_long() flag
                break;
            case 'v':
                if (sscanf(optarg, "%u", &version) != 1) {
                    printf("Bad v4l version specified: {%s}\n", optarg);
                    usage(stdout, argc, argv);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'p':
                if (sscanf(optarg, "%u", &portNumber) < 0) {
                    printf("Bad port number specified: {%s}\n", optarg);
                    usage(stdout, argc, argv);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'l':
                positionedText = strdup(optarg);
                break;
            case 'd':
                dev_name = strdup(optarg);
                break;
            case 't':
                title = strdup(optarg);
                break;
            case 'b':
                banner = true;
                break;
            case 'S':
                subtitle = strdup(optarg);
                break;
            case 'f':
/*
                if (strcmp(optarg, "yuv") == 0)
                    pixel_format = V4L2_PIX_FMT_YUV420;
                else if (strcmp(optarg, "yuyv") == 0)
                    pixel_format = V4L2_PIX_FMT_YUYV;
                else if (strcmp(optarg, "raw") == 0)
                    pixel_format = V4L2_PIX_FMT_PWC2;
                else {
                    printf ("Bad format specified {%s} "
                    "can be either 'yuv' or 'raw'\n", optarg);
                    usage(stdout, argc, argv);
                    exit(EXIT_FAILURE);
                }
*/
                break;

            case 'h':
                usage(stdout, argc, argv);
                exit(EXIT_SUCCESS);
            case 'm':
//        io = IO_METHOD_MMAP;
                break;
            case 'r':
//        io = IO_METHOD_READ;
                break;

            case 's':
                if (sscanf(optarg, "%dx%d", &width, &height) != 2)
                {
                    printf("Bad size specified {%s}\n", optarg);
                    usage(stdout, argc, argv);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'u':
//        io = IO_METHOD_USERPTR;
                break;
            default:
                usage(stderr, argc, argv);
                exit(EXIT_FAILURE);
        }
    }

    CameraServer* cam_serv = new CameraServer(dev_name, version, banner, title, subtitle, positionedText, portNumber);
    cam_serv->startThread();

    int i = 0;
    char* countStr;
    if ((countStr = (char*) malloc(100)) == NULL) {
        exit(1);
    }
    while (true) {
        int* imageSize = cam_serv->getImageSize();
        printf("got %d %d\n", imageSize[0], imageSize[1]);
        free(imageSize);
        cam_serv->clearPositionedText();
        memset(countStr, 0, 100);
        sprintf(countStr, "10|20|RUN COUNT: %d", i++);
        cam_serv->setPositionedText(countStr);
        memset(countStr, 0, 100);
        sprintf(countStr, "10|40|MAKE SOME NOISE", i++);
        cam_serv->setPositionedText(countStr);
        sleep(1);
    }
//    delete cam_serv;

//    cam_serv = new CameraServer(dev_name, title, subtitle, version, portNumber);
//    cam_serv->start();

    printf("Exit program\n");
    exit(EXIT_SUCCESS);
}
