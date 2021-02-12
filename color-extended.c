#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "color.h"

#define ZAKAROUF_CCC_VERSION "0.1.1"
#define ZAKAROUF_CCC_YEAR    "2021-2022"

#define COLRS_HELP \
  "Usage:\n"\
  \
  "  ccc -i[dxh] [OPTIONS...]\n\n"\
  "  ccc (Color Code Converter) by Zakarouf\n\n"\
  \
  "Options:\n"\
  "  -h,              help\n\n"\
  "  -I[0-9[dxh]] [VALUE],\n"\
  "       0-9         Assign an ID, if unassigned ID is 0\n"\
  "       d [x,y,z]   Get r,g,b\n"\
  "       x [xyz]     Get rgb in hex\n"\
  "       h [x,y,z]   Get h,s,v\n"\
  "\n"\
  "  -s[0-9]          Show Color Values\n"\
  "  -S[0-9]          Select Color Value\n"\
  "\n"\
  "  -c[rs],          Make color brighter/darker\n"\
  "  -v[rs],          Make color brighter/darker | smooth\n"\
  "  -r[rs],          Make color brighter/darker | recursive\n"\
  "  -t,              Modifier for -v,-r\n"\
  "\n"\
  "  -M[[hdx]sr] [VALUES..],            \n"\
  "         h         Take Modifier for HSV\n"\
  "         d,x       Take Modifier for RGB\n"\
  "         s         Specify Config Values\n"\
  "         r         Retain The Orginal Value\n"\
  "  -s,              Show Current Color Values\n"\
  "Examples:\n"\
  \
  "  $ ccc -I0 d=25,163,136 -c 14\n"\
  "  $ ccc -I2 h=230,60,80 -Mhrs x=10,b=1,y=20,1=1.1 -s2\n"\
  "  $ ccc -I x=FF0211 -r 10 -t 14\n"\

#define ZAKAROUF_CCC_INTRODUCTION\
  \
  "Welcome to the CCC (to quit type `\\q`, for Help type `-h`).\n"\
  "CCC "ZAKAROUF_CCC_VERSION" by Zakarouf "ZAKAROUF_CCC_YEAR"\n"


int hex[16] = {   '0', '1', '2', '3'
                , '4', '5', '6', '7'
                , '8', '9', 'A', 'B'
                , 'C', 'D', 'E', 'F'};

typedef struct colrs {

  unsigned short r;
  unsigned short g;
  unsigned short b;

}colrgb_t;

typedef struct Vector {

    float x,y,z;

}Vector3;

typedef struct colrhsv {

  float h;
  float s;
  float v;

}colhsv_t;


#define ccc_isDigit_M(c)\
    ((c < '0') ? 0 : ((c <= '9') ? 1 : 0 ))

static float color_getFloatFromString(char pattern[] ,char s[])
{
    float a;
    sscanf(s, pattern, &a);
    return a;
}

static int color_findCharInStr(char *str, int sz, char c)
{
    for (int i = 0; i < sz; ++i)
    {
        if(*str == c)
            return 1;
        str++;
    }
    return 0;
}

static void color_draw_percent_bar(const float at, const float outof, const float max, const float growthrate, char madeof)
{
    const float lim = (at/outof)*(max);
    for (float i = 0; i < lim; i += growthrate)
    {
        putchar(madeof);
    }
}

static void color_draw_Block(int sz, char ch)
{
    for (int i = 0; i < sz; ++i)
    {
        putchar(ch);
    }
}


static float make_darklit(float v, float by)
{
    return (v+by > 100.0f ? 100.0f : v+by);
}

static float color_changeHSV_Hvalue_WRAP(float hue, float by)
{
    float rVal = hue + by;
    return ((rVal >= 0.0f)? (fmodf(rVal, 360.0f)) : 360.f );
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

static int color_show_values(colrgb_t c, colhsv_t h)
{
    const float maxval = 10.0f;
    const float grate = 1.0f;

    colorB_set(c.r, c.g, c.b);
    printf("          ");
    color_reset();
    puts("  <== Your Color");


    // RGB Values
    printf("Red:   %4hd | 0x%02hx |", c.r, c.r);
    colorB_set(0xff, 0, 0);
    color_draw_percent_bar(c.r, c.r+c.b+c.g, maxval, grate, ' ');
    color_reset();
    putchar('\n');

    printf("Green: %4hd | 0x%02hx |", c.g, c.g);
    colorB_set(0, 0xff, 0);
    color_draw_percent_bar(c.g, c.r+c.b+c.g, maxval, grate, ' ');
    color_reset();
    putchar('\n');

    printf("Blue:  %4hd | 0x%02hx |", c.b, c.b);
    colorB_set(0, 0, 0xff);
    color_draw_percent_bar(c.b, c.r+c.b+c.g, maxval, grate, ' ');
    color_reset();
    puts("\n");

    // HSV Values

    printf("Hue:         %5.1f |", h.h);
    color_set(100, 200, 100);
    color_draw_percent_bar(h.h, 360.0f, maxval, grate, '=');
    color_reset();
    putchar('\n');

    printf("Saturation:  %5.1f |", h.s);
    color_set(100, 80, 10);
    color_draw_percent_bar(h.s, 100.0f, maxval, grate, '=');
    color_reset();
    putchar('\n');

    printf("Value:       %5.1f |", h.v);
    color_set(200, 166, 144);
    color_draw_percent_bar(h.v, 100.0f, maxval, grate, '=');
    color_reset();
    putchar('\n');

    return 0;

}

static void color_darklit (colhsv_t *hsv, int lim, float by)
{
    if(by > 0.0f)
    {
        printf("Lighten up by %.1f%%\n", by);
    }
    else {
        printf("Darken up by %.1f%%\n", by);
    } 
    
    for (int i = 0; i < lim; ++i)
    {
        hsv->v = make_darklit(hsv->v, by);
        colrgb_t c = color_hsv_to_rgb(*hsv);
        colorB_set(c.r, c.g, c.b);
        color_draw_Block(10, ' ');
        color_reset();
        
        printf("#%0hx%0hx%0hx | %.1f%%", c.r, c.g, c.b, by*(i+1));

        printf("\n");
        if(hsv->v >= 99.0f)return;
        if(hsv->v <= 0.0f)return;
    }
}

static void color_darklit_mod(char *arg, colrgb_t *r, colhsv_t *h, colrgb_t r_ori, colhsv_t h_ori)
{
    for (int i = 2; i < 4; ++i)
    {
        if (arg[i] == 'r')
        {
            *r = r_ori;
            *h = h_ori;
        }
        else if (arg[i] == 's')
        {
            color_show_values(*r, *h);
        }
    }
}

static void color_subdarklit (char *arg[],int at, int argc, colhsv_t *hsv , int lim, float by)
{

    colrgb_t c;
    if(by > 0.0f)
    {
        c = color_hsv_to_rgb(*hsv);
        printf("Lighten up by %.1f%%\n", by);
    }
    else {
        c = color_hsv_to_rgb(*hsv);
        printf("Darken up by %.1f%%\n", by);
    }

    colhsv_t tmpHSV = *hsv;
    colrgb_t tmpRGB = c;

    for (int i = 0; i < lim; ++i)
    {
        hsv->v = make_darklit(hsv->v, by/lim);
        if(hsv->v > 100.0f && hsv->v < 0.0f) // if out of bounds then return
        {
            colorB_set(c.r, c.g, c.b);
            printf("     ");
            color_reset();
            
            printf("#%0hx%0hx%0hx\n", c.r, c.g, c.b);
            color_darklit_mod(arg[at], &c, hsv, tmpRGB, tmpHSV);
            return;
        }
        
        c = color_hsv_to_rgb(*hsv);
        colorB_set(c.r, c.g, c.b);
        printf(" ");
        color_reset();
    }
    colorB_set(c.r, c.g, c.b);
    printf("          ");
    color_reset();

    printf("#%0hx%0hx%0hx\n", c.r, c.g, c.b);

    color_darklit_mod(arg[at], &c, hsv, tmpRGB, tmpHSV);
}

static void ccc_multiManupilation_HSV(const int x, const int y, const int bsize , colhsv_t *hsv, Vector3 changeRate, Vector3 changeRateModify)
{
    colrgb_t rgb = color_hsv_to_rgb(*hsv);

    for (int i = 0; i < y*x; ++i)
    {
        if(!(i%x))
        {
            printf("\n");
        }
        
        if(    (hsv->h <= 360.0f && hsv->h >= 0.0f)
            && (hsv->s <= 100.0f && hsv->s >= 0.0f)
            && (hsv->v <= 100.0f && hsv->v >= 0.0f)  )
        {
            hsv->h = (color_changeHSV_Hvalue_WRAP(hsv->h, changeRate.x)) * changeRateModify.x;
            hsv->s += (changeRate.y) * changeRateModify.y;
            hsv->v += (changeRate.z) * changeRateModify.z;

            rgb = color_hsv_to_rgb(*hsv);
            

            colorB_set(rgb.r, rgb.g, rgb.b);
            color_draw_Block(bsize, ' ');
            color_reset();

        }
        else
        {
            printf("\n");
            return;
        }

    }
    printf("\n");
}
static void ccc_multiManupilation_RGB(const int x, const int y, const int bsize , colrgb_t *rgb, Vector3 changeRate, Vector3 changeRateModify)
{

    for (int i = 0; i < y*x; ++i)
    {
        if(!(i%x))
        {
            printf("\n");
        }
        if(    (rgb->r < 256 && rgb->r > 0) 
            && (rgb->g < 256 && rgb->g > 0) 
            && (rgb->b < 256 && rgb->b > 0))

        {
            rgb->r += (changeRate.x) * changeRateModify.x;
            rgb->g += (changeRate.y) * changeRateModify.y;
            rgb->b += (changeRate.z) * changeRateModify.z;


            colorB_set(rgb->r, rgb->g, rgb->b);
            color_draw_Block(bsize, ' ');
            color_reset();



        }
        else
        {
            printf("\n");
            return;
        }

    }
    printf("\n");
}

static int ccc_getDARKLIT_Config(char *arg[], int count, void *CONFIG)
{
    for (int i = 0; i < count; ++i)
    {
        if(arg[i][0] == '-')
        {
            switch(arg[i][1])
            {
                case 't':
                    return color_getFloatFromString("%f", arg[i + 1]);
                break;

                default:
                    break;
            }
        }
    }

    return 10;
}

static void ccc_Manupilation(char *arg[], int at, int count, colrgb_t *rgb, colhsv_t *hsv)
{
    // Setting Up Defaults
    const int TOTAL_VALUES = 9;
    int bz = 2
        , x = 5
        , y = 5;

    Vector3 ch_rate = {0.0f, 0.0f, 0.0f}
            , ch_rate_mod = {1.0f, 1.0f, 1.0f};



    // Check if all the values are given
    if(arg[at+1] != NULL)
    {
        int check = 0;
        int i = 0;
        while(check != TOTAL_VALUES-1)
        {
            if(arg[at+1][i] == ',')
            {
                check++;
            }
            i++;
            if(i > 37)
            {
                break;
            }
        }

        if (check == TOTAL_VALUES-1 && !color_findCharInStr(arg[at], 4, 's'))
        {
            sscanf(arg[at+1] ,"%d,%d,%d,%f,%f,%f,%f,%f,%f"
                    , &x, &y, &bz
                    , &ch_rate.x, &ch_rate.y, &ch_rate.z
                    , &ch_rate_mod.x, &ch_rate_mod.y, &ch_rate_mod.z);
        }
        else
        {
    
            char *token = arg[at + 1];
    
            /* Token types (char = actual variable it is representing)
                x = x
                y = y
                b = bz
                1 = ch_rate.x
                2 = ch_rate.y
                3 = ch_rate.z
                a = ch_rate_modify.x
                s = ch_rate_modify.y
                d = ch_rate_modify.z
            */
            for (int i = 0; i < TOTAL_VALUES && token != NULL; i++)
            {
                if(token[0] == 'x')
                    sscanf(token, "x=%d", &x);
    
                else if(token[0] == 'y')
                    sscanf(token, "y=%d", &y);
    
                else if(token[0] == 'b')
                    sscanf(token, "b=%d", &bz);
    
                else if(token[0] == '1')
                    sscanf(token, "1=%f", &ch_rate.x);
                else if(token[0] == '2')
                    sscanf(token, "2=%f", &ch_rate.y);
                else if(token[0] == '3')
                    sscanf(token, "3=%f", &ch_rate.z);
    
                else if(token[0] == 'a')
                    sscanf(token, "a=%f", &ch_rate_mod.x);
                else if(token[0] == 's')
                    sscanf(token, "s=%f", &ch_rate_mod.y);
                else if(token[0] == 'd')
                    sscanf(token, "d=%f", &ch_rate_mod.z);
    
                if (i == 0)
                    token = strtok(arg[at+1], ",");
                else
                    token = strtok(NULL, ",");
    
            }
        }

    }

    colrgb_t tmpRGB = *rgb;
    colhsv_t tmpHSV = *hsv;

    if (arg[at][2] == 'd' || arg[at][2] == 'x')
    {
        ccc_multiManupilation_RGB(x, y, bz ,rgb, ch_rate, ch_rate_mod);
        if (color_findCharInStr(arg[at], 4, 'r'))
        {
            *rgb = tmpRGB;
            *hsv = tmpHSV;
        }
        else {
            *hsv = color_rgb_to_hsv(rgb->r, rgb->g, rgb->b);
        }

    }
    else if (arg[at][2] == 'h')
    {
        ccc_multiManupilation_HSV(x, y, bz ,hsv, ch_rate, ch_rate_mod);
        if (color_findCharInStr(arg[at], 4, 'r'))
        {
            *rgb = tmpRGB;
            *hsv = tmpHSV;
        }
        else {
            *rgb = color_hsv_to_rgb(*hsv);
        }
    }

}

static int ccc_getCOLOR(char *arg, char *arg2 ,colhsv_t* h, colrgb_t* cpf, int *AT_)
{
    int AT_color = *AT_;
    if (ccc_isDigit_M(arg[2]))
    {
        AT_color = arg[2] - '0';
        *AT_ = AT_color;
        
    }

        
    if (arg2[0] == 'x') {
        sscanf(arg2, "x=%2hx%2hx%2hx", &cpf[AT_color].r, &cpf[AT_color].g, &cpf[AT_color].b);
        h[AT_color] = color_rgb_to_hsv((float)cpf[AT_color].r, (float)cpf[AT_color].g, (float)cpf[AT_color].b);
    }
    else if(arg2[0] == 'd'){
        sscanf(arg2, "d=%hd,%hd,%hd", &cpf[AT_color].r, &cpf[AT_color].g, &cpf[AT_color].b);
        h[AT_color] = color_rgb_to_hsv((float)cpf[AT_color].r, (float)cpf[AT_color].g, (float)cpf[AT_color].b);
    }
    else if(arg2[0] == 'h'){
        sscanf(arg2, "h=%f,%f,%f", &h[AT_color].h, &h[AT_color].s, &h[AT_color].v);
        cpf[AT_color] = color_hsv_to_rgb(h[AT_color]);
    }
    else
    {
        return -1;
    }

    return AT_color;
}

static int phrasearg(char *arg[], int count, colhsv_t* hsv, colrgb_t* rgb)
{

    static colrgb_t tmpRGB[10] = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
    };
    static colhsv_t tmpHSV[10] = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
    };

    static int AT_color = 0;
    int tmp_int;

    for (int i = 0; i < count; i++)
    {
        if(arg[i][0] == '-')
        {
            switch (arg[i][1])
            {
                case 'h': puts(COLRS_HELP);
                  break;
                case 'I':

                    if((ccc_getCOLOR(arg[i], arg[i+1] ,hsv, rgb, &AT_color)) > 9)
                    {
                        return 1;
                    }

                    color_show_values(rgb[AT_color], hsv[AT_color]);
                    printf("%d\n", count);
                    tmpRGB[AT_color] = rgb[AT_color];
                    tmpHSV[AT_color] = hsv[AT_color];
                    putchar('\n');

                    break;

                case 'r':

                    color_darklit(&hsv[AT_color], ccc_getDARKLIT_Config(arg, count, NULL), color_getFloatFromString("%f", (arg[i + 1] == NULL) ? "0":arg[i+1] ));
                    rgb[AT_color] = color_hsv_to_rgb(hsv[AT_color]);

                    color_reset();

                    break;
                case 'c':
                    color_subdarklit(arg, i, count, &hsv[AT_color], ccc_getDARKLIT_Config(arg, count, NULL), color_getFloatFromString("%f", (arg[i + 1] == NULL) ? "0":arg[i+1]));
                    rgb[AT_color] = color_hsv_to_rgb(hsv[AT_color]);

                    
                    break;
                case 'v':
                    color_subdarklit(arg, i, count, &hsv[AT_color], fabs(color_getFloatFromString("%f", arg[i + 1])), color_getFloatFromString("%f", (arg[i + 1] == NULL) ? "0":arg[i+1]));
                    rgb[AT_color] = color_hsv_to_rgb(hsv[AT_color]);

                    break;

                case 'M':
                    ccc_Manupilation(arg, i, count, &rgb[AT_color], &hsv[AT_color]);
                    printf("\n");
                    break;

                case 'O':
                    rgb[AT_color] = tmpRGB[AT_color];
                    hsv[AT_color] = tmpHSV[AT_color];
                    break;
                
                case 's':
                    if (ccc_isDigit_M(arg[i][2]))
                    {
                        tmp_int = arg[i][2] - '0' ;
                        color_show_values(rgb[tmp_int], hsv[tmp_int]);
                    }
                    else {
                        color_show_values(rgb[AT_color], hsv[AT_color]);
                    }
                    break;
                case 'S':
                    if(ccc_isDigit_M(arg[i][2]))
                    {
                        AT_color = arg[i][2] - '0';
                    }



                default:
                    break;
            }

            
            color_reset();
        }
        //color_set(250, 10, 10);
        //printf("%s\n", arg[i]);
    }

  return AT_color;

}

char **zse_malloc_2D_array_char (unsigned int x, unsigned int y) {

    char **arr = malloc(y * sizeof(char*));
    for (int i = 0; i < y; ++i)
    {
        arr[i] = (char*)malloc(x * sizeof(char));
    }

    return arr;

}

void zse_free2dchar(char **mem, int size)
{
    for (int i = 0; i < size; ++i)
    {
        free(mem[i]);
    }
    free(mem);

}

// Start Interactive Mode if No arguments given
static void ccc_interactive_mode (colrgb_t *rgb, colhsv_t *hsv)
{
    printf("Starting Interactive Mode...\n"
        ZAKAROUF_CCC_INTRODUCTION);
    int At = 0
        , quit = 0
        , buffXsize = 100
        , buffYsize = 100
        , argc = 0;

    
    char ** buffer_FINAL = zse_malloc_2D_array_char(buffXsize, buffYsize);
    char * buffer = malloc(sizeof(char) * buffYsize * buffXsize);
    char * token;

    while (!quit)
    {
        color_set(255, 0, 0); printf("%d", At);
        color_set(0, 255, 0); printf(">");
        color_set(0, 0, 255); printf("> ");
        color_reset();
        
        
        scanf("%[^\n]%*c", buffer);
        
        if (buffer[0] == '\\' && buffer[1] == 'q')
        {
            quit = 1;
        }

        token = strtok(buffer, " ");

        
        for (int i = 0; i < buffXsize && token != NULL; ++i)
        {

            sprintf(buffer_FINAL[i], "%s", token);
            token = strtok(NULL, " ");
            argc ++;


        }

        At = phrasearg(buffer_FINAL, argc, hsv, rgb);
        argc = 0;

    }

    zse_free2dchar(buffer_FINAL, buffYsize);
    free(buffer);
}


int main(int argc, char *argv[]) {

    colrgb_t rgb[10] = { 0 };
    colhsv_t hsv[10] = { 0 };

    if (argc >= 1)
    {
        phrasearg(argv, argc, hsv, rgb);
    }

    if (argc <= 1 || (argv[1][0] == '-'  &&  argv[1][1] == 'i') )
    {
        ccc_interactive_mode(rgb, hsv);
    }

    return 0;
}

/*
*/
