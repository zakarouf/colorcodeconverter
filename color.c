#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "color.h"


#define COLRS_HELP \
  "Commands:\n"\
  "\t-h \t help\n"\
  "\t-id\t get r,g,b\n"\
  "\t-ix\t get rgb in hex\n"\
  "\t-ih\t get h,s,v\n"



typedef struct colrs {

  unsigned short r;
  unsigned short g;
  unsigned short b;

}colrgb_t;

typedef struct colrhsv {

  float h;
  float s;
  float v;

}colhsv_t;

static float color_getffrom(char s[])
{
    float a;
    sscanf(s, "%f", &a);
    return a;
}

static void color_draw_percent_bar(const float at, const float outof, const float max, const float growthrate, char madeof)
{
    const float lim = (at/outof)*(max);
    for (float i = 0; i < lim; i += growthrate)
    {
        putchar(madeof);
    }
}

static int color_show_values(colrgb_t c, colhsv_t h)
{
    const float maxval = 1.0f;
    const float grate = 0.1f;

    colorB_set(c.r, c.g, c.b);
    printf("          ");
    color_reset();
    puts("  <== Your Color");
    // RGB Values

    printf("Red:   %4hd | 0x%2hx |", c.r, c.r);
    colorB_set(0xff, 0, 0);
    color_draw_percent_bar(c.r, c.r+c.b+c.g, maxval, grate, ' ');
    color_reset();
    putchar('\n');

    printf("Green: %4hd | 0x%2hx |", c.g, c.g);
    colorB_set(0, 0xff, 0);
    color_draw_percent_bar(c.g, c.r+c.b+c.g, maxval, grate, ' ');
    color_reset();
    putchar('\n');

    printf("Blue:  %4hd | 0x%2hx |", c.b, c.b);
    colorB_set(0, 0, 0xff);
    color_draw_percent_bar(c.b, c.r+c.b+c.g, maxval, grate, ' ');
    color_reset();
    puts("\n");

    // HSV Values

    printf("Hue:         %5.1f  |", h.h);
    color_set(100, 200, 100);
    color_draw_percent_bar(h.h, 360.0f, maxval, grate, '=');
    color_reset();
    putchar('\n');

    printf("Saturation:  %5.1f  |", h.s);
    color_set(100, 80, 10);
    color_draw_percent_bar(h.s, 100.0f, maxval, grate, '=');
    color_reset();
    putchar('\n');

    printf("Value:       %5.1f  |", h.v);
    color_set(200, 166, 144);
    color_draw_percent_bar(h.v, 100.0f, maxval, grate, '=');
    color_reset();
    putchar('\n');

    return 0;

}


static float color_max(float a, float b, float c) {
    return ((a > b)? (a > c ? a : c) : (b > c ? b : c));
}
static float color_min(float a, float b, float c) {
    return ((a < b)? (a < c ? a : c) : (b < c ? b : c));
}

static colhsv_t color_rgb_to_hsv(float r, float g, float b) {
    // R, G, B values are divided by 255
    // to change the range from 0..255 to 0..1:
    colhsv_t hsv;
    r /= 255.0f;
    g /= 255.0f;
    b /= 255.0f;
    float cmax = color_max(r, g, b); // maximum of r, g, b
    float cmin = color_min(r, g, b); // minimum of r, g, b
    float diff = cmax-cmin; // diff of cmax and cmin.

    if (cmax == cmin)
       hsv.h = 0;
    else if (cmax == r)
       hsv.h = fmod((60 * ((g - b) / diff) + 360), 360.0f);
    else if (cmax == g)
       hsv.h = fmod((60 * ((b - r) / diff) + 120), 360.0f);
    else if (cmax == b)
       hsv.h = fmod((60 * ((r - g) / diff) + 240), 360.0f);
    // if cmax equal zero
       if (cmax == 0)
          hsv.s = 0;
       else
          hsv.s = (diff / cmax) * 100;

    // compute v
    hsv.v = cmax * 100;
    return hsv;
}

static colrgb_t color_hsv_to_rgb (colhsv_t hsv)
{
    if(hsv.h>360 || hsv.h<0 || hsv.s>100 || hsv.s<0 || hsv.v>100 || hsv.v<0)
    {
        colrgb_t c;
        c.r = 0;
        c.g = 0;
        c.b = 0;

        return c;
    }

    colrgb_t c;

    float s = hsv.s/100;
    float v = hsv.v/100;
    float C = s*v;
    float X = C*(1 - fabs( fmod(hsv.h/60.0, 2)-1));
    float m = v-C;
    float r,g,b;
    if(hsv.h >= 0 && hsv.h < 60){
        r = C,g = X,b = 0;
    }
    else if(hsv.h >= 60 && hsv.h < 120){
        r = X,g = C,b = 0;
    }
    else if(hsv.h >= 120 && hsv.h < 180){
        r = 0,g = C,b = X;
    }
    else if(hsv.h >= 180 && hsv.h < 240){
        r = 0,g = X,b = C;
    }
    else if(hsv.h >= 240 && hsv.h < 300){
        r = X,g = 0,b = C;
    }
    else{
        r = C,g = 0,b = X;
    }

    c.r = (r+m)*255;
    c.g = (g+m)*255;
    c.b = (b+m)*255;

    return c;

}

static float make_lit(float v, float by)
{
    return (v+by > 100.0f ? 100.0f : v+by);
}

static void color_ligten (colhsv_t hsv, int lim, float by)
{
    printf("Lighten by %.1f%%\n", by);
    for (int i = 0; i < lim; ++i)
    {
        hsv.v = make_lit(hsv.v, by);
        colrgb_t c = color_hsv_to_rgb(hsv);
        colorB_set(c.r, c.g, c.b);
        printf("          ");
        color_reset();
        printf("#%2hx%2hx%2hx", c.r, c.g, c.b);
        printf("\n");
    }
}


static int phrasearg(char *arg[], int count, colhsv_t* h, colrgb_t* cpf)
{
    for (int i = 0; i < count; i++)
    {
        if(arg[i][0] == '-')
        {
            switch (arg[i][1])
            {
                case 'h': puts(COLRS_HELP);
                  break;
                case 'i':
                    if (arg[i][2] == 'x') {
                        sscanf(arg[i+1], "%2hx%2hx%2hx", &cpf->r, &cpf->g, &cpf->b);
                        *h = color_rgb_to_hsv((float)cpf->r, (float)cpf->g, (float)cpf->b);
                    }
                    else if(arg[i][2] == 'd'){
                        sscanf(arg[i+1], "%3hd,%3hd,%3hd", &cpf->r, &cpf->g, &cpf->b);
                        *h = color_rgb_to_hsv((float)cpf->r, (float)cpf->g, (float)cpf->b);
                    }
                    else if(arg[i][2] == 'h'){
                        sscanf(arg[i+1], "%f,%f,%f", &h->h, &h->s, &h->v);
                        *cpf = color_hsv_to_rgb(*h);
                    }
                    else
                    {
                        puts("No Color Input");
                    }

                    color_show_values(*cpf, *h);
                    putchar('\n');

                    break;
                case 'l':
                    color_ligten(*h, 10, color_getffrom(arg[i+1]));
                    color_reset();
                    break;

                default:
                    puts(COLRS_HELP); break;
            }
            color_reset();


        }
    }

  return 0;

}

int main(int argc, char *argv[]) {

  colrgb_t rgb;
  colhsv_t hsv;
  phrasearg(argv, argc, &hsv, &rgb);



  return 0;
}