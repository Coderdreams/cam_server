
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <stdint.h>

#include <fcntl.h>		/* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <boost/thread/mutex.hpp>

#include <gd.h>

#include <asm/types.h>		/* for videodev2.h */
#include <linux/videodev2.h>

#include "ImageProcessor.hxx"

using namespace std;

ImageProcessor::ImageProcessor(ImageProcessorOptions _options) {
    timestampFmt = strdup("%Y-%m-%d %H:%M (%Z)");
    underlay     = NULL;
    overlay      = NULL;
    format       = FORMAT_JPEG;
    options      = _options;
    compression  = -1;
    positionedText = vector<string>(255);
    if (options.positionedText) {
       positionedText.push_back(string(options.positionedText));
    }
    start = time(NULL);
    gmt = -1;
}

ImageProcessor::~ImageProcessor() {
}

int ImageProcessor::add_image_yuyv(const unsigned char *img, unsigned int width, unsigned int height, uint32_t *abitmap)
{
    uint8_t *ptr;
    uint32_t x, y, z;

    /* YUYV and UYVY are very similar and so  *
     * are both handled by this one function. */

    ptr = (uint8_t *) img;
    z = 0;

    for(y = 0; y < height; y++)
    {
        for(x = 0; x < width; x++)
        {
            int r, g, b;
            int y, u, v;
/*
            if(src->palette == SRC_PAL_UYVY)
            {
                if(!z) y = ptr[1] << 8;
                else   y = ptr[3] << 8;

                u = ptr[0] - 128;
                v = ptr[2] - 128;
            }
            else // SRC_PAL_YUYV 
            {
*/
                if(!z) y = ptr[0] << 8;
                else   y = ptr[2] << 8;

                u = ptr[1] - 128;
                v = ptr[3] - 128;
//          }

            r = (y + (359 * v)) >> 8;
            g = (y - (88 * u) - (183 * v)) >> 8;
            b = (y + (454 * u)) >> 8;

            *(abitmap++) += CLIP(r, 0x00, 0xFF);
            *(abitmap++) += CLIP(g, 0x00, 0xFF);
            *(abitmap++) += CLIP(b, 0x00, 0xFF);

            if(z++)
            {
                z = 0;
                ptr += 4;
            }
        }
    }

    return(0);
}

int ImageProcessor::add_image_rgb32(unsigned char *img, unsigned int width, unsigned int height, uint32_t *abitmap)
{
    uint32_t i = width * height;

    while(i-- > 0)
    {
        *(abitmap++) += *(img++);
        *(abitmap++) += *(img++);
        *(abitmap++) += *(img++);
        img++;
    }

    return(0);
}

int ImageProcessor::add_image_rgb24(const unsigned char *img, unsigned int width, unsigned int height, uint32_t *abitmap)
{
    uint32_t i = width * height * 3;

    while(i-- > 0) *(abitmap++) += *(img++);

    return(0);
}

int ImageProcessor::draw_banner(gdImage *image)
{
    char timestamp[200];
    int w, h;
    int height;
    int spacing;
    int top;
    int y;

    w = gdImageSX(image);
    h = gdImageSY(image);

    /* Create the timestamp text. */
    my_strftime(timestamp, 200, timestampFmt,
        start, gmt);

    /* Calculate the position and height of the banner. */
    spacing = 4;
    height = options.fontsize + (spacing * 2);

    if(options.subtitle || options.info)
        height += options.fontsize * 0.8 + spacing;

    top = 0;
    if(options.banner == BOTTOM_BANNER) top = h - height;

    /* Draw the banner line. */
    if(options.banner == TOP_BANNER)
    {
        gdImageFilledRectangle(image,
            0, height + 1,
            w, height + 2,
            options.bl_colour);
    }
    else
    {
        gdImageFilledRectangle(image,
            0, top - 2,
            w, top - 1,
            options.bl_colour);
    }

    /* Draw the background box. */
    gdImageFilledRectangle(image,
        0, top,
        w, top + height,
        options.bg_colour);

    y = top + spacing + options.fontsize;

    /* Draw the title. */
    DrawText(image, options.font, options.fontsize,
        spacing, y, ALIGN_LEFT,
        options.fg_colour, options.shadow, options.title);

    /* Draw the timestamp. */
    DrawText(image, options.font, options.fontsize * 0.8,
        w - spacing, y, ALIGN_RIGHT,
        options.fg_colour, options.shadow, timestamp);

    y += spacing + options.fontsize * 0.8;

    /* Draw the sub-title. */
    DrawText(image, options.font, options.fontsize * 0.8,
        spacing, y, ALIGN_LEFT,
        options.fg_colour, options.shadow, options.subtitle);

    /* Draw the info text. */
    DrawText(image, options.font, options.fontsize * 0.7,
        w - spacing, y, ALIGN_RIGHT,
        options.fg_colour, options.shadow, options.info);

    return(0);
}

vector<string> ImageProcessor::getPositionedText() 
{
    boost::mutex::scoped_lock l(m_mutex);
    return vector<string>(positionedText);
}

void ImageProcessor::clearPositionedText() 
{
    boost::mutex::scoped_lock l(m_mutex);
    positionedText.clear();
}

void ImageProcessor::setPositionedText(char* text) 
{
    boost::mutex::scoped_lock l(m_mutex);
    positionedText.push_back(string(text));
}

int ImageProcessor::draw_positioned_text(gdImage *image, vector<string> text)
{
    int x, y;

    //int w = gdImageSX(image);
    //int h = gdImageSY(image);
    char *line;
    for (int i = 0; i < text.size(); i++) {
        line = strdup(text[i].c_str());
        char *saveptr;
        char *pch = strtok_r(line,"|", &saveptr);
        int index = 0;
        while (pch != NULL)
        {
            if (index == 0) {
                sscanf(pch, "%i", &x);
            } else if(index == 1) {
                sscanf(pch, "%i", &y);
            } else {
                DrawText(image, options.font, options.fontsize,
                    x, y, ALIGN_LEFT,
                    options.fg_colour, options.shadow, pch);
                index = -1;
            }
            pch = strtok_r(NULL, "|", &saveptr);
            index++;
        }
        free(line);
    }

    return(0);
}

char *ImageProcessor::my_strftime(char *dst, size_t max, char *src,
                    time_t timestamp, int gmt)
{
    struct tm tm_timestamp;

    /* Set the time structure. */
    if(gmt) gmtime_r(&timestamp, &tm_timestamp);
    else localtime_r(&timestamp, &tm_timestamp);

    *dst = '\0';
    strftime(dst, max, src, &tm_timestamp);

    return(dst);
}

char *ImageProcessor::strduptime(char *src, time_t timestamp, int gmt)
{
    struct tm tm_timestamp;
    char *dst;
    size_t l;

    if(!src) return(NULL);

    /* Set the time structure. */
    if(gmt) gmtime_r(&timestamp, &tm_timestamp);
    else localtime_r(&timestamp, &tm_timestamp);

    dst = NULL;
    l = strlen(src) * 2;

    while(1)
    {
        size_t r;
        char *t = static_cast<char*>(realloc(dst, l));

        if(!t)
        {
            free(dst);
            return(NULL);
        }

        dst = t;

        *dst = 1;
        r = strftime(dst, l, src, &tm_timestamp);

        if(r > 0 && r < l) return(dst);
        if(r == 0 && *dst == '\0') return(dst);

        l *= 2;
    }

    return(NULL);
}

void ImageProcessor::DrawText(gdImagePtr im, char *font, double size,
    int x, int y, char align,
    uint32_t colour, char shadow, char *text)
{
    char *err;
    int brect[8];

    if(!text) return;

    if(shadow)
    {
        uint32_t scolour = colour & 0xFF000000;

        DrawText(im, font, size, x + 1, y,
            align, scolour, 0, text);
        DrawText(im, font, size, x, y + 1,
            align, scolour, 0, text);
        DrawText(im, font, size, x - 1, y,
            align, scolour, 0, text);
        DrawText(im, font, size, x, y - 1,
            align, scolour, 0, text);

    }

    /* Correct alpha value for GD. */
    colour = (((colour & 0xFF000000) / 2) & 0xFF000000) +
        (colour & 0xFFFFFF);

    /* Pre-render the text. We use the results during alignment. */
    err = gdImageStringFT(NULL, &brect[0], colour, font, size, 0.0, 0, 0, text);
    if(err)
    {
        printf("%s\n", err);
        return;
    }

    /* Adjust the coordinates according to the alignment. */
    switch(align)
    {
        case ALIGN_CENTER: x -= brect[4] / 2; break;
        case ALIGN_RIGHT:  x -= brect[4];     break;
    }

    /* Render the text onto the image. */
    gdImageStringFT(im, NULL, colour, font, size, 0.0, x, y, text);
}

char* ImageProcessor::process_image(const void *p, ssize_t size, int pixel_format, unsigned int width, unsigned int height, int* resultSize)
{
    static int no_image = 0;
    char filename[1024];
    FILE *f;
    uint32_t *abitmap, *pbitmap;
    gdImage *original;

    abitmap = static_cast<uint32_t*>(calloc(width * height * 3, sizeof(uint32_t)));
    if(!abitmap)
    {
        printf("Out of memory.\n");
        exit(-1);
    }

    if (pixel_format == V4L2_PIX_FMT_YUYV) {
        add_image_yuyv(static_cast<const unsigned char*>(p), width, height, abitmap);
    } else {
        add_image_rgb24(static_cast<const unsigned char*>(p), width, height, abitmap);
    }

    original = gdImageCreateTrueColor(width, height);
    if(!original)
    {
        printf("Out of memory.\n");
        free(abitmap);
        exit(-1);
    }

    pbitmap = abitmap;
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            int px = x;
            int py = y;
            int colour;

            colour  = (*(pbitmap++)) << 16;
            colour += (*(pbitmap++)) << 8;
            colour += (*(pbitmap++));

            gdImageSetPixel(original, px, py, colour);
        }
    }

    free(abitmap);

    if(options.drawBanner) draw_banner(original);

    vector<string> text = getPositionedText();
    if(text.size() > 0) draw_positioned_text(original, text);

    int buffSize;
    char* buff;
    void* imageJpg;

    imageJpg = gdImageJpegPtr(original, &buffSize, compression);
    buff = (char*) malloc(buffSize);
    memcpy(buff, imageJpg, buffSize);

    gdImageDestroy(original);
    gdFree(imageJpg);

    *resultSize = buffSize;
    return buff;
}



