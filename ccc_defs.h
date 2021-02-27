#ifndef ZAKAROUF_CCC_DEFINITION
#define ZAKAROUF_CCC_DEFINITION
/*----------------------------*/

#define ZAKAROUF_CCC_VERSION "0.2.0"
#define ZAKAROUF_CCC_YEAR    "2021-2022"
#define ZAKAROUF_CCC_HOSTPAGE "github.com/zakarouf/colorcodeconverter"

#define COLRS_HELP \
  "Usage:\n"\
  \
  "  ccc -i[dxh] [OPTIONS...]\n\n"\
  "  ccc (Color Code Converter) by Zakarouf\n\n"\
  "  " ZAKAROUF_CCC_HOSTPAGE"\n\n"\
  \
  "Options:\n"\
  "  -h,              help\n\n"\
  "  -i               Start in Interactive Mode, Should be the First Arguments\n"\
  "\n"\
  "  -I[0-9] [dxh]=[VALUE],\n"\
  "       0-9         Assign an ID, if unassigned ID is 0\n"\
  "       d [x,y,z]   Get r,g,b\n"\
  "       x [xyz]     Get rgb in hex\n"\
  "       h [x,y,z]   Get h,s,v\n"\
  "\n"\
  "  -s[0-9]          Show Color Values\n"\
  "  -S[0-9]          Select Color Value\n"\
  "\n"\
  "  -c[rs] [v,i]=[],  Make color brighter/darker\n"\
  "  -v[rs] [v,i]=[],  Make color brighter/darker | smooth\n"\
  "  -r[rs] [v,i]=[],  Make color brighter/darker | recursive\n"\
  "\n"\
  "  -M[[dxh]sr] [VALUES..],            \n"\
  "         h         Take Modifier for HSV\n"\
  "         d,x       Take Modifier for RGB\n"\
  "         s         Specify Config Values\n"\
  "         r         Retain The Orginal Value\n"\
  "\n"\
  "  -R, 			        Reset Color Value To Original\n"\
  "  -C [Value],      Copy Color Value To self\n"\
  "\n"\
  "  -F [FILENAME],   Read Commnads From A File\n"\
  "\n"\
  "Examples:\n"\
  \
  "  $ ccc -I0 d=25,163,136 -c 14\n"\
  "  $ ccc -I2 h=230,60,80 -Mhrs x=10,b=1,y=20,1=1.1 -s2\n"\
  "  $ ccc -I x=FF0211 -r 10 -t 14\n"\

#define ZAKAROUF_CCC_INTRODUCTION\
  \
  "Welcome to the CCC (to quit type `\\q`, for Help type `-h`).\n"\
  "CCC " ZAKAROUF_CCC_VERSION" by Zakarouf " ZAKAROUF_CCC_YEAR"\n"


#endif
