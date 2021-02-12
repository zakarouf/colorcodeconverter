#ifndef ZAKAROUF_COLORCODECONVERTER_H
#define ZAKAROUF_COLORCODECONVERTER_H

#define color_reset() \
    printf("\x1b[0m")

#define color_set(r, g, b) \
    printf("\x1b[38;2;%d;%d;%dm", r, g, b)

#define colorB_set(br, bg, bb) \
    printf("\x1b[48;2;%d;%d;%dm", br, bg, bb)

#define colorAll_set(r, g, b, br, bg, bb) \
    printf("\x1b[38;2;%d;%d;%d;48;2;%d;%d;%dm", r, g, b, br, bg, bb)


#endif
