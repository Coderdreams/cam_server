#include <asm/types.h>
#include <sys/types.h>
#include <stdint.h>
#include <time.h>
#include <string>
#include <vector>
#include <gd.h>

#ifndef ImageProcessor_H
#define ImageProcessor_H

#define CLIP(val, min, max) (val > max) ? max : ((val < min) ? min : val)

#define ALIGN_LEFT   (0)
#define ALIGN_CENTER (1)
#define ALIGN_RIGHT  (2)

#define NO_BANNER     (0)
#define TOP_BANNER    (1)
#define BOTTOM_BANNER (2)

#define FORMAT_JPEG (0)
#define FORMAT_PNG  (1)

using namespace std;

struct ImageProcessorOptions {
    bool drawBanner;
    char *info;
    char* title;
    char* subtitle;
    char* positionedText;
    uint32_t bg_colour;
    uint32_t bl_colour;
    uint32_t fg_colour;
    char *font;
    int fontsize;
    char shadow;
    char banner;

    ImageProcessorOptions() {
        drawBanner = false;
        title = NULL;
        info = NULL;
        subtitle = NULL;
        positionedText = NULL;
        bg_colour = 0x40263A93;
        bl_colour = 0x00FF0000;
        fg_colour = 0x00FFFFFF;
        font = strdup("/usr/share/fonts/truetype/ttf-xfree86-nonfree/luxisr.ttf");
        fontsize = 10;
        shadow = 1;
        banner = BOTTOM_BANNER;
    }
};


class ImageProcessor
{
  public:
    ImageProcessor(ImageProcessorOptions options);
    ~ImageProcessor();
    char* process_image(const void *p, ssize_t size, int pixel_format, unsigned int width, unsigned int height, int* resultSize);
    void setPositionedText(char* text);
    void clearPositionedText();
    
  private:
    char* my_strftime(char *dst, size_t max, char *src, time_t timestamp, int gmt);
    char* strduptime(char *src, time_t timestamp, int gmt);
    int draw_banner(gdImage *image);
    int draw_positioned_text(gdImage *image, vector<string> text);
    int add_image_yuyv(const unsigned char *src, unsigned int width, unsigned int height, uint32_t *abitmap);
    int add_image_rgb32(unsigned char *img, unsigned int width, unsigned int height, uint32_t *abitmap);
    int add_image_rgb24(const unsigned char *img, unsigned int width, unsigned int height, uint32_t *abitmap);
    void DrawText(gdImagePtr im, char *font, double size, int x, int y, char align, uint32_t colour, char shadow, char *text);
    vector<string> getPositionedText();
    char *timestampFmt;
    char format;
    char compression;
    char *underlay;
    char *overlay;
    vector<string> positionedText;
    /* Capture start time. */
    time_t start;
    int gmt;
    boost::mutex m_mutex;
    ImageProcessorOptions options;
};

#endif // (ImageProcessor_H)