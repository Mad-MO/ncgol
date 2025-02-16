
#include "patterns.h"

#define X 1
#define _ 0

typedef struct
{
    const uint16_t width;
    const uint16_t height;
    const char* name;
    const uint8_t pattern[];
} pattern_desc_t;



pattern_desc_t beacon =
{
    .width  = 4,
    .height = 4,
    .name   = "Beacon",
    .pattern =
    {// 1 2 3 4
        X,X,_,_,
        X,_,_,_,
        _,_,_,X,
        _,_,X,X,
    }
};

pattern_desc_t blinker =
{
    .width  = 3,
    .height = 1,
    .name   = "Blinker",
    .pattern =
    {// 1 2 3
        X,X,X,
    }
};

pattern_desc_t block =
{
    .width  = 2,
    .height = 2,
    .name   = "Block",
    .pattern =
    {// 1 2
        X,X,
        X,X,
    }
};

pattern_desc_t conway =
{
    .width  = 26,
    .height = 5,
    .name   = "Conway",
    .pattern =
    {// 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6
        _,X,X,_,_,X,_,_,X,_,_,X,_,X,_,_,_,X,_,_,X,_,_,X,_,X,// 1
        X,_,_,_,X,_,X,_,X,X,_,X,_,X,_,_,_,X,_,X,_,X,_,X,_,X,// 2
        X,_,_,_,X,_,X,_,X,_,X,X,_,X,_,_,_,X,_,X,X,X,_,_,X,_,// 3
        X,_,_,_,X,_,X,_,X,_,_,X,_,X,_,X,_,X,_,X,_,X,_,_,X,_,// 4
        _,X,X,_,_,X,_,_,X,_,_,X,_,_,X,_,X,_,_,X,_,X,_,_,X,_,// 5
    }
};

pattern_desc_t glider =
{
    .width  = 3,
    .height = 3,
    .name   = "Glider",
    .pattern =
    {// 1 2 3
        _,X,_,
        _,_,X,
        X,X,X,
    }
};



pattern_desc_t *pattern_list[] =
{
    &beacon,
    &blinker,
    &block,
    &conway,
    &glider
};
