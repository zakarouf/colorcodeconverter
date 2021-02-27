/*-------------------------------------------------
    Color Code Convertor - extended (CCC) 2021
--------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "color.h"
#include "ccc_defs.h"


typedef struct colrs {

  unsigned short r;
  unsigned short g;
  unsigned short b;

}colrgb_t;

typedef struct _Vector_3_FLOAT {

    float x,y,z;

}Vf3;

typedef struct _Vector_3_INTEGER
{
    int x,y,z;
}Vi3;

typedef struct colrhsv {

  float h;
  float s;
  float v;

}colhsv_t;


typedef struct colALLEncompass
{
    colrgb_t *rgb;
    colhsv_t *hsv;

    int size;
}CCC_Color_t;


//______CCC_COMMANDS_START_______//
typedef float(*ccc_command_t)(float);
typedef float(*ccc_command3_t)(float, float, float);

//Extra Commands

static float ccc_commandsExtra_GetSelf(float self)
{
    return self;
}

#define ccc_command_MAX_COMMANDS_M 13
static const ccc_command_t gCCC_commands[ccc_command_MAX_COMMANDS_M] = {

    ccc_commandsExtra_GetSelf
    , sinf, cosf, tanf
    , asinf, acosf, atanf
    , sinhf, coshf, tanhf
    , asinhf, acoshf, atanhf
};

typedef enum _CCC_math_commands
{
     cmc_GETSELF = 0
    , cmc_SIN
    , cmc_COS
    , cmc_TAN
    , cmc_ASIN
    , cmc_ACOS
    , cmc_ATAN
    , cmc_SINH
    , cmc_COSH
    , cmc_TANH
    , cmc_ASINH
    , cmc_ACOSH
    , cmc_ATANH


}CCC_math_commands;

// CCC_Commnand_major

static float color_addOrSub_WRAP(float x, float by, float Max)
{
    x += by;
    return ((x >= 0.0f)? (fmodf(x, Max)) :Max );
}

static float color_multiply_WRAP(float x, float by, float Max)
{
    x *= by;
    return ((x >= 0.0f)? (fmodf(x, Max)) :Max );
}

static float color_divide_WRAP(float x, float by, float Max)
{
    x /= by;
    return ((x >= 0.0f)? (fmodf(x, Max)) :Max );
}

#define ccc_commandMajor_MAX_COMMANDS_M 3
static ccc_command3_t gCCC_commands_Major[ccc_commandMajor_MAX_COMMANDS_M] = {
      color_addOrSub_WRAP
    , color_multiply_WRAP
    , color_divide_WRAP

};

//________CCC_commands_END_________//

// Manupulation Args
typedef struct ccc_Manupilation_args
{
    int i,x,y,bz;
    Vi3 chr8_min, chr8_maj;
    Vf3 chVal;
    
}ccc_manArg_t;


// Malloc And Free 2d Char, Taken from Ztorg (https://github.com/zakarouf/ztorg)
static char **zse_malloc_2D_array_char (unsigned int x, unsigned int y) {

    char **arr = malloc(y * sizeof(char*));
    for (int i = 0; i < y; ++i)
    {
        arr[i] = (char*)malloc(x * sizeof(char));
    }

    return arr;

}
static void zse_free2dchar(char **mem, int size)
{
    for (int i = 0; i < size; ++i)
    {
        free(mem[i]);
    }
    free(mem);

}

// Check if Given Char is Digit
#define ccc_isDigit_MF(c)\
    (((c) < '0') ? 0 : (((c) <= '9') ? 1 : 0 ))

// Wrap up int
#define ccc_wrapInt_MF(x, MaxValue)\
    (x)%(MaxValue)

// Check if Value is in range, 1 if it is...
#define ccc_checkIfInRange(val, min, max)\
    (( (val) < min) ? 0 : (( (val) < max) ? 1 : 0 ))

static float color_getFloatFromString(char pattern[] ,char s[])
{
    float a;
    sscanf(s, pattern, &a);
    return a;
}

// See if `Char` exist in a string
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

// Draw a Bar
static void color_draw_percent_bar(const float at, const float outof, const float max, const float growthrate, char madeof)
{
    const float lim = (at/outof)*(max);
    for (float i = 0; i < lim; i += growthrate)
    {
        putc(madeof, stdout);
    }
}

// Draw a block of Char
static void color_draw_Block(int sz, char ch)
{
    for (int i = 0; i < sz; ++i)
    {
        putc(ch, stdout);
    }
}

// Change HSV Values, WRAP for looping the value at the end/start point
static float color_changeHSV_Vvalue(float v, float by)
{
    return (v+by > 100.0f ? 100.0f : v+by);
}

// Get Max Or Min Values
static float color_max(float a, float b, float c) {
    return ((a > b)? (a > c ? a : c) : (b > c ? b : c));
}
static float color_min(float a, float b, float c) {
    return ((a < b)? (a < c ? a : c) : (b < c ? b : c));
}

// Convert RGB Color Value to HSV Values
static colhsv_t color_rgb_to_hsv(float r, float g, float b) {

    colhsv_t hsv;

    r /= 255.0f;
    g /= 255.0f;
    b /= 255.0f;

    float cmax = color_max(r, g, b);
    float cmin = color_min(r, g, b);

    float diff = cmax-cmin;

    if (cmax == cmin)
       hsv.h = 0;
    else if (cmax == r)
       hsv.h = fmod((60 * ((g - b) / diff) + 360), 360.0f);
    else if (cmax == g)
       hsv.h = fmod((60 * ((b - r) / diff) + 120), 360.0f);
    else if (cmax == b)
       hsv.h = fmod((60 * ((r - g) / diff) + 240), 360.0f);
    
       if (cmax == 0)
          hsv.s = 0;
       else
          hsv.s = (diff / cmax) * 100;

    
    hsv.v = cmax * 100;

    return hsv;
}

// Convert HSV Color Value to RGB Values
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

static void color_dk_getvals(char * arg, float *by, int *lim)
{
    char *token = strtok(arg, ",");

    while (token != NULL)
    {
        if (token[0] == 'v')
        {
            sscanf(token, "v=%f", by);
        }
        else if (token[0] == 'i')
        {
            sscanf(token, "i=%d", lim);
        }

        token = strtok(NULL, ",");
        
    }
}

// Color Dark/Lit
static void color_darklit (char * arg1, char * arg2, colhsv_t *hsv)
{
    float by = 0;
    int lim = 5;

    color_dk_getvals(arg2, &by, &lim);

    if(by > 0.0f)
    {
        printf("\nLighten up by %.1f%%\n", by);
    }
    else {
        printf("\nDarken up by %.1f%%\n", by);
    } 
    
    for (int i = 0; i < lim; ++i)
    {
        hsv->v = color_changeHSV_Vvalue(hsv->v, by);
        colrgb_t c = color_hsv_to_rgb(*hsv);
        colorB_set(c.r, c.g, c.b);
        color_draw_Block(10, ' ');
        color_reset();
        
        printf("#%02hx%02hx%02hx | %.1f%%", c.r, c.g, c.b, by*(i+1));

        printf("\n");
        if(hsv->v > 100.0f)return;
        if(hsv->v < 0.0f)return;
    }
}

// Sub Light/Dark Modifier
static void color_subdarklit_mod(char *arg, colrgb_t *r, colhsv_t *h, colrgb_t r_ori, colhsv_t h_ori)
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
            //color_show_values(*r, *h);
        }
    }
}

// Sub Light/Dark
static void color_subdarklit (char * arg1, char * arg2, colhsv_t *hsv)
{
    float by = 0;
    int lim = 5;
    color_dk_getvals(arg2, &by, &lim);

    colrgb_t c = color_hsv_to_rgb(*hsv);
    if(by > 0.0f)
    {
        printf("\nLighten up by %.1f%%\n", by);
    }
    else {
        printf("\nDarken up by %.1f%%\n", by);
    }

    colhsv_t tmpHSV = *hsv;

    for (int i = 0; i < lim; ++i)
    {
        hsv->v = color_changeHSV_Vvalue(hsv->v, by/lim);
        if(hsv->v > 100.0f && hsv->v < 0.0f) // if out of bounds then return
        {
            break;
            colorB_set(c.r, c.g, c.b);
            printf("     ");
            color_reset();
            
            printf("#%02hx%02hx%02hx\n", c.r, c.g, c.b);
            if(color_findCharInStr(arg1, 4 ,'r'))hsv[0]=tmpHSV;
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

    printf("#%02hx%02hx%02hx\n", c.r, c.g, c.b);

    if(color_findCharInStr(arg1, 4 ,'r'))hsv[0]=tmpHSV;
}

// __Mainupulation for HSV Values
static void ccc_multiManupilation_HSV(colhsv_t *hsv, colrgb_t *rgb ,Vf3 changeValue, Vi3 changeRate_com, Vi3 changeRate_major_com)
{      
    if(    (hsv->h <= 360.0f && hsv->h >= 0.0f)
        && (hsv->s <= 100.0f && hsv->s >= 0.0f)
        && (hsv->v <= 100.0f && hsv->v >= 0.0f)  )
    {
        hsv->h =   gCCC_commands_Major[changeRate_major_com.x](hsv->h, gCCC_commands[changeRate_com.x](changeValue.x), 360.0f) ;
        hsv->s =   gCCC_commands_Major[changeRate_major_com.y](hsv->s, gCCC_commands[changeRate_com.y](changeValue.y), 100.0f) ;
        hsv->v =   gCCC_commands_Major[changeRate_major_com.z](hsv->v, gCCC_commands[changeRate_com.z](changeValue.z), 100.0f) ;
        *rgb = color_hsv_to_rgb(*hsv);

    }
    else
    {
        return;
    }
}

// __Mainupulation for RGB Values
static void ccc_multiManupilation_RGB(colhsv_t *hsv , colrgb_t *rgb, Vf3 changeValue, Vi3 changeRate_com, Vi3 changeRate_major_com)
{
    if(    (rgb->r < 256 && rgb->r > 0) 
        && (rgb->g < 256 && rgb->g > 0) 
        && (rgb->b < 256 && rgb->b > 0))
    {
        rgb->r = gCCC_commands_Major[changeRate_major_com.x](rgb->r, gCCC_commands[changeRate_com.x](changeValue.x), 255.0f) ;
        rgb->g = gCCC_commands_Major[changeRate_major_com.y](rgb->g, gCCC_commands[changeRate_com.y](changeValue.y), 255.0f) ;
        rgb->b = gCCC_commands_Major[changeRate_major_com.z](rgb->b, gCCC_commands[changeRate_com.z](changeValue.z), 255.0f) ;
        *hsv = color_rgb_to_hsv(rgb->r, rgb->g, rgb->b);
    }
    else
    {
        return;
    }
}

// Manupilation Calls
static void ccc_Manupilation_makeCalls(char calltype ,ccc_manArg_t arg, CCC_Color_t color ,char *msg, int at)
{
    void (*call)(colhsv_t*, colrgb_t*, Vf3, Vi3, Vi3);

    if (calltype == 'h')
    {
        call = ccc_multiManupilation_HSV;
    }
    else if (calltype == 'd' || calltype == 'x')
    {
        call = ccc_multiManupilation_RGB;
    }
    else
    {
        return;
    }

    int y = 0;
    for (int i = 1; i < arg.i+1; ++i)
    {
        call(color.hsv, color.rgb, arg.chVal, arg.chr8_min, arg.chr8_maj);

        colorB_set(color.rgb->r, color.rgb->g, color.rgb->b);
        color_draw_Block(arg.bz, ' ');
        color_reset();
        if (!(i % arg.x))
        {
            y++;
            if (y > arg.y)
            {
                printf("%s" ,msg);
                return;
            }
            else
            {
                printf("\n");
            }
        }

    }
    printf("%s" ,msg);
}

// Color Manupulation
static void ccc_Manupilation(char *arg[], int at, int count, colrgb_t *rgb, colhsv_t *hsv)
{
    // Setting Up Defaults
    const int TOTAL_VALUES = 10;
    ccc_manArg_t CCC_m_arg =
    {
        .x = 5,
        .y = 5,
        .i = 25,
        .bz = 2,

        .chr8_min = {0},
        .chr8_maj = {0},
        .chVal    = {0},

    };

    char linebreakAtend[2] = "";

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
            sscanf(arg[at+1] ,"%d,%d,%d,%d,%f,%f,%f,%d,%d,%d,%d,%d,%d"
                    , &CCC_m_arg.i, &CCC_m_arg.x, &CCC_m_arg.y, &CCC_m_arg.bz
                    , &CCC_m_arg.chVal.x, &CCC_m_arg.chVal.y, &CCC_m_arg.chVal.z
                    , &CCC_m_arg.chr8_min.x, &CCC_m_arg.chr8_min.y, &CCC_m_arg.chr8_min.z
                    , &CCC_m_arg.chr8_maj.x, &CCC_m_arg.chr8_maj.y, &CCC_m_arg.chr8_maj.z
                );
        }
        else
        {
            if (arg[at + 1] == NULL)
                return;

            char *token = strtok(arg[at+1], ",");
    
            /* Token types (char = actual variable it is representing)
                i = it
                x = x
                y = y
                b = bz
                1 = ch_rate.x
                2 = ch_rate.y
                3 = ch_rate.z
                a = ch_rate_modify.x
                b = ch_rate_modify.y
                c = ch_rate_modify.z
                A =
                B =
                C =
            */
            while (token != NULL)
            {
                if (token[0] == 'i')
                {
                    sscanf(token, "i=%d", &CCC_m_arg.i);
                }

                else if(token[0] == 'x')
                {
                    sscanf(token, "x=%d", &CCC_m_arg.x);
                }
    
                else if(token[0] == 'y')
                {
                    sscanf(token, "y=%d", &CCC_m_arg.y);
                }
    
                else if(token[0] == 'b')
                {
                    sscanf(token, "b=%d", &CCC_m_arg.bz);
                }
    
                else if(token[0] == '1')
                {
                    sscanf(token, "1=%f", &CCC_m_arg.chVal.x);
                }
                else if(token[0] == '2')
                {
                    sscanf(token, "2=%f", &CCC_m_arg.chVal.y);
                }
                else if(token[0] == '3')
                {
                    sscanf(token, "3=%f", &CCC_m_arg.chVal.z);
                }
    
                else if(token[0] == 'a')
                {
                    sscanf(token, "a=%d", &CCC_m_arg.chr8_min.x);
                }
                else if(token[0] == 'b')
                {
                    sscanf(token, "b=%d", &CCC_m_arg.chr8_min.y);
                }
                else if(token[0] == 'c')
                {
                    sscanf(token, "c=%d", &CCC_m_arg.chr8_min.z);
                }
                else if(token[0] == 'A')
                {
                    sscanf(token, "A=%d", &CCC_m_arg.chr8_maj.x);
                }
                else if(token[0] == 'B')
                {
                    sscanf(token, "B=%d", &CCC_m_arg.chr8_maj.y);
                }
                else if(token[0] == 'C')
                {
                    sscanf(token, "C=%d", &CCC_m_arg.chr8_maj.z);
                }
                else if(token[0] == 'm')
                {
                    linebreakAtend[0] = '\n';
                }
    
                token = strtok(NULL, ",");
    
            }
        }

    }

    colrgb_t tmpRGB = *rgb;
    colhsv_t tmpHSV = *hsv;


    ccc_Manupilation_makeCalls(arg[at][2], CCC_m_arg, (CCC_Color_t){rgb, hsv, 1}, linebreakAtend, 10);


    if (color_findCharInStr(arg[at], 5, 'r'))
    {
            *rgb = tmpRGB;
            *hsv = tmpHSV;
    }

}

// Get Color From String
static int ccc_getCOLOR(char *arg, char *arg2 ,colhsv_t* h, colrgb_t* cpf, int *AT_)
{
    int AT_color = *AT_;
    if (ccc_isDigit_MF(arg[2]))
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

// Read From File
static char * ccc_ReadFromFile(char *file, int * string_size)
{
    FILE *f = fopen(file, "rb");

    if (f == NULL)
        return NULL;

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

    char *string = malloc(fsize + 1);
    fread(string, 1, fsize, f);
    fclose(f);

    string[fsize] = 0;

    *string_size = fsize;
    return string;
}

// Break String into List of String (Into Tokens)
static int ccc_BreakStringInto2DString_ASTOKENS (char ** buffer2D ,int buffXsize, int buffYsize, char *buffer, char *token_breaker)
{
    char * token;
    int count = 0;

    token = strtok(buffer, token_breaker);

    for (int i = 0; i < buffXsize && token != NULL; ++i)
    {
        sprintf(buffer2D[i], "%s", token);
        token = strtok(NULL, token_breaker);
        count++;
    }

    return count;
}

// Draw Color and Show their Contents
static int color_show_values(colrgb_t c, colhsv_t h)
{
    const float maxval = 10.0f;
    const float grate = 1.0f;

    putchar('\n');
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

// Read Command From File
static int ccc_commandPhrasing(char *arg[], int count, CCC_Color_t color);
static void ccc_RunFile(char * file, CCC_Color_t color)
{
    int buffsz;
    int tokens;
    char * buff = ccc_ReadFromFile(file, &buffsz);
    if (buff == NULL)
    {
        return;
    }

    int buffXsize = 100;
    int buffYsize = 100;

    char ** buff2d = zse_malloc_2D_array_char(buffXsize, buffYsize);
    tokens = ccc_BreakStringInto2DString_ASTOKENS(buff2d, buffXsize, buffYsize, buff, " \n");

    ccc_commandPhrasing(buff2d, tokens, color);

    zse_free2dchar(buff2d, buffYsize);
    free(buff);

}

// Main Command Phrasing
static int ccc_commandPhrasing(char *arg[], int count, CCC_Color_t color)
{
    // Create Temperory for Hard Reset.
    static colrgb_t tmpRGB[10] = { 0 };
    static colhsv_t tmpHSV[10] = { 0 };

    static int AT_color = 0; // Color Cursor Id
    int tmp_int;             // Temporary Int for all kinds of stuff

    /*
        How Does Lexer Works :-
         1. Each Command starts with `-`, Why?
            At first I started creating ccc w/o
            any Interactive mode and File Read
            mode in my mind.
         2. Next is the char and pattern w/ their
            respective functions...Thats it.

         -- Use `ccc -h` for more info on Commands
    */

    // Lexer
    for (int i = 0; i < count; i++)
    {
        if(arg[i][0] == '-')
        {
            switch (arg[i][1])
            {
                // Show Help
                case 'h':
                    puts(COLRS_HELP);
                    break;

                // Take Color into Memory
                case 'I':

                    if((ccc_getCOLOR(arg[i], arg[i+1] ,color.hsv, color.rgb, &AT_color)) > 9)
                    {
                        return -1;
                    }
                    tmpRGB[AT_color] = color.rgb[AT_color];
                    tmpHSV[AT_color] = color.hsv[AT_color];

                    break;

                // Darken/Lighten Up Color (Recursive)
                case 'r':
                    color_darklit(arg[i], arg[i + 1], &color.hsv[AT_color]);
                    color.rgb[AT_color] = color_hsv_to_rgb(color.hsv[AT_color]);
                    color_reset();
                    break;

                // Darken/Lighten Up Color
                case 'c':
                    color_subdarklit(arg[i], arg[i+1], &color.hsv[AT_color]);
                    color.rgb[AT_color] = color_hsv_to_rgb(color.hsv[AT_color]);
                    break;

                // Darken/Lighten Up Color (Makes a Long Percent Bar)
                case 'v':
                    color_subdarklit(arg[i], arg[i+1], &color.hsv[AT_color]);
                    color.rgb[AT_color] = color_hsv_to_rgb(color.hsv[AT_color]);
                    break;

                // Manupilate Color (More verbose)
                case 'M':
                    ccc_Manupilation(arg, i, count, &color.rgb[AT_color], &color.hsv[AT_color]);
                    break;

                // Reset Color w/ Original input color
                case 'R':
                    color.rgb[AT_color] = tmpRGB[AT_color];
                    color.hsv[AT_color] = tmpHSV[AT_color];
                    break;
                
                // Show Selected Color Contents
                case 's':
                    if (ccc_isDigit_MF(arg[i][2]))
                    {
                        tmp_int = arg[i][2] - '0' ;
                        color_show_values(color.rgb[tmp_int], color.hsv[tmp_int]);
                    }
                    else if(arg[i][2] == '-' && arg[i][3] == '1')
                    {
                        for (int i = 0; i < color.size; ++i)
                        {
                            colorB_set(color.rgb[i].r, color.rgb[i].g, color.rgb[i].b);
                            color_draw_Block(10, ' ');
                            printf("| ID: %d | #%02hx%02hx%02hx | %f,%f,%f\n"
                                , i, color.rgb[i].r, color.rgb[i].g, color.rgb[i].b
                                , color.hsv[i].h, color.hsv[i].s, color.hsv[i].v
                            );
                            color_reset();
                        }
                    }
                    else {
                        color_show_values(color.rgb[AT_color], color.hsv[AT_color]);
                    }
                    break;

                // Select Cursor
                case 'S':
                    if(ccc_isDigit_MF(arg[i][2]))
                    {
                        AT_color = arg[i][2] - '0';
                    }
                    break;

                // Read Comands From File
                case 'F':
                    if (arg[i+1] != NULL)
                    {
                        ccc_RunFile(arg[i+1], color);
                    }
                    break;
                case 'C':
                    if (arg[i+1] != NULL)
                    {
                        sscanf(arg[i+1], "%d", &tmp_int);
                        if (ccc_checkIfInRange(tmp_int, 0, color.size))
                        {
                            memcpy(&color.rgb[AT_color], &color.rgb[tmp_int], sizeof(colrgb_t));
                            memcpy(&color.hsv[AT_color], &color.hsv[tmp_int], sizeof(colhsv_t));
                        }
                    }
                    break;


                // Do Nothing
                default:
                    break;
            }
   
            color_reset();
        }
    }

  return AT_color;

}

// Start Interactive Mode if No arguments given or Specified Otherwise
static void ccc_interactive_mode (CCC_Color_t color)
{
    printf("Starting Interactive Mode...\n"
        ZAKAROUF_CCC_INTRODUCTION);


    int   At = 0               // Cursor   
        , quit = 0             // Quit (0 to not quit)
        , buffXsize = 100      // Maximum String Size for Single Tokens
        , buffYsize = 100      // Maximum Size For List of Tokens
        , argumentCount = 0;   // Number Tokens/Commands

    
    // Creating buffer...
    char ** buffer_FINAL = zse_malloc_2D_array_char(buffXsize, buffYsize);
    char * buffer = malloc(sizeof(char) * buffYsize * buffXsize);

    while (!quit)
    {
        color_set(255, 0, 0); printf("%d", At);
        color_set(0, 255, 0); printf(">");
        color_set(0, 0, 255); printf("> ");
        color_reset();
        
        // Read From User, `%[^\n]%*c` for Taking in Space-seperated Tokens
        scanf("%[^\n]%*c", buffer);
        
        // Check if User typed `\q` to quit
        if (buffer[0] == '\\' && buffer[1] == 'q')
        {
            quit = 1;
        }

        // Read from user-input buffer, breaks into token convert's in Array of String. Returns nos. of Token read.
        argumentCount = ccc_BreakStringInto2DString_ASTOKENS (buffer_FINAL , buffXsize,  buffYsize, buffer, " \n");

        // Phrase Array of String as Commands. Returns the color cursor ID.
        At = ccc_commandPhrasing(buffer_FINAL, argumentCount, color);

    }

    zse_free2dchar(buffer_FINAL, buffYsize);
    free(buffer);
}

CCC_Color_t color_init_empty(int size)
{
    return (CCC_Color_t) {
        .size = size,
        .rgb = malloc(sizeof(colrgb_t) * size),
        .hsv = malloc(sizeof(colhsv_t) * size)
    };
}

void color_delete(CCC_Color_t *color)
{
    free(color->rgb);
    free(color->hsv);
    color->size = 0;
}

/*--------------------------------------*/
int main(int argc, char *argv[]) {

    // Main Color Holder...
    CCC_Color_t color = color_init_empty(10);

    if (argc >= 1)
    {
        // Main Lexer(..?) type command interface
        ccc_commandPhrasing(argv, argc, color);
    }

    if (argc <= 1 || (argv[1][0] == '-'  &&  argv[1][1] == 'i') )
    {
        // Interactive Mode
        ccc_interactive_mode(color);
    }

    return 0;
}
