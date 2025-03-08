
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



pattern_desc_t conway =
{
    .width  = 26,
    .height = 5,
    .name   = "Conway",
    .pattern =
    {// 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6
        _,X,X,_,_,X,_,_,X,_,_,X,_,X,_,_,_,X,_,_,X,_,_,X,_,X,
        X,_,_,_,X,_,X,_,X,X,_,X,_,X,_,_,_,X,_,X,_,X,_,X,_,X,
        X,_,_,_,X,_,X,_,X,_,X,X,_,X,_,_,_,X,_,X,X,X,_,_,X,_,
        X,_,_,_,X,_,X,_,X,_,_,X,_,X,_,X,_,X,_,X,_,X,_,_,X,_,
        _,X,X,_,_,X,_,_,X,_,_,X,_,_,X,_,X,_,_,X,_,X,_,_,X,_
    }
};

pattern_desc_t conway_full =
{
    .width  = 44,
    .height = 11,
    .name   = "Conway",
    .pattern =
    {// 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2
        _,_,_,_,_,_,_,X,X,_,_,X,_,_,X,_,_,X,_,X,_,_,_,X,_,_,X,_,_,X,_,X,_,X,_,X,X,X,_,_,_,_,_,_,
        _,_,_,_,_,_,X,_,_,_,X,_,X,_,X,X,_,X,_,X,_,_,_,X,_,X,_,X,_,X,_,X,_,X,_,X,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,X,_,_,_,X,_,X,_,X,_,X,X,_,X,_,_,_,X,_,X,X,X,_,_,X,_,_,_,_,X,X,X,_,_,_,_,_,_,
        _,_,_,_,_,_,X,_,_,_,X,_,X,_,X,_,_,X,_,X,_,X,_,X,_,X,_,X,_,_,X,_,_,_,_,_,_,X,_,_,_,_,_,_,
        _,_,_,_,_,_,_,X,X,_,_,X,_,_,X,_,_,X,_,_,X,_,X,_,_,X,_,X,_,_,X,_,_,_,_,X,X,X,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,X,X,_,_,_,X,_,_,X,_,_,_,X,_,X,X,X,_,_,_,_,X,_,_,X,X,X,_,_,_,X,_,_,_,X,_,X,X,X,_,X,X,X,
        X,_,_,_,_,X,_,X,_,X,X,_,X,X,_,X,_,_,_,_,_,X,_,X,_,X,_,_,_,_,_,X,_,_,_,X,_,X,_,_,_,X,_,_,
        X,_,X,X,_,X,X,X,_,X,_,X,_,X,_,X,X,_,_,_,_,X,_,X,_,X,X,_,_,_,_,X,_,_,_,X,_,X,X,_,_,X,X,_,
        X,_,_,X,_,X,_,X,_,X,_,_,_,X,_,X,_,_,_,_,_,X,_,X,_,X,_,_,_,_,_,X,_,_,_,X,_,X,_,_,_,X,_,_,
        _,X,X,_,_,X,_,X,_,X,_,_,_,X,_,X,X,X,_,_,_,_,X,_,_,X,_,_,_,_,_,X,X,X,_,X,_,X,_,_,_,X,X,X
    }
};

// Still lifes

pattern_desc_t block =
{
    .width  = 2,
    .height = 2,
    .name   = "Block",
    .pattern =
    {// 1 2
        X,X,
        X,X
    }
};

pattern_desc_t beehive =
{
    .width  = 4,
    .height = 3,
    .name   = "Beehive",
    .pattern =
    {// 1 2 3 4
        _,X,X,_,
        X,_,_,X,
        _,X,X,_
    }
};

pattern_desc_t loaf =
{
    .width  = 4,
    .height = 4,
    .name   = "Loaf",
    .pattern =
    {// 1 2 3 4
        _,X,X,_,
        X,_,_,X,
        _,X,_,X,
        _,_,X,_
    }
};

pattern_desc_t boat =
{
    .width  = 3,
    .height = 3,
    .name   = "Boat",
    .pattern =
    {// 1 2 3
        X,X,_,
        X,_,X,
        _,X,_
    }
};

pattern_desc_t tub =
{
    .width  = 3,
    .height = 3,
    .name   = "Tub",
    .pattern =
    {// 1 2 3
        _,X,_,
        X,_,X,
        _,X,_
    }
};

// Oscillators

pattern_desc_t blinker =
{
    .width  = 3,
    .height = 1,
    .name   = "Blinker",
    .pattern =
    {// 1 2 3
        X,X,X
    }
};

pattern_desc_t toad =
{
    .width  = 4,
    .height = 2,
    .name   = "Toad",
    .pattern =
    {// 1 2 3 4
        _,X,X,X,
        X,X,X,_
    }
};

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
        _,_,X,X
    }
};

pattern_desc_t pulsar =
{
    .width  = 13,
    .height = 13,
    .name   = "Pulsar",
    .pattern =
    {// 1 2 3 4 5 6 7 8 9 0 1 2 3
        _,_,X,X,X,_,_,_,X,X,X,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,
        X,_,_,_,_,X,_,X,_,_,_,_,X,
        X,_,_,_,_,X,_,X,_,_,_,_,X,
        X,_,_,_,_,X,_,X,_,_,_,_,X,
        _,_,X,X,X,_,_,_,X,X,X,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,X,X,X,_,_,_,X,X,X,_,_,
        X,_,_,_,_,X,_,X,_,_,_,_,X,
        X,_,_,_,_,X,_,X,_,_,_,_,X,
        X,_,_,_,_,X,_,X,_,_,_,_,X,
        _,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,X,X,X,_,_,_,X,X,X,_,_
    }
};

pattern_desc_t penta_decathlon =
{
    .width  = 10,
    .height = 3,
    .name   = "Penta-decathlon",
    .pattern =
    {// 1 2 3 4 5 6 7 8 9 0
        _,_,X,_,_,_,_,X,_,_,
        X,X,_,X,X,X,X,_,X,X,
        _,_,X,_,_,_,_,X,_,_
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
        X,X,X
    }
};

// Spaceships

pattern_desc_t lwss =
{
    .width  = 5,
    .height = 4,
    .name   = "Lightweight spaceship",
    .pattern =
    {// 1 2 3 4 5
        X,_,_,X,_,
        _,_,_,_,X,
        X,_,_,_,X,
        _,X,X,X,X
    }
};

pattern_desc_t mwss =
{
    .width  = 6,
    .height = 5,
    .name   = "Middleweight spaceship",
    .pattern =
    {// 1 2 3 4 5 6
        _,_,X,_,_,_,
        X,_,_,_,X,_,
        _,_,_,_,_,X,
        X,_,_,_,_,X,
        _,X,X,X,X,X
    }
};

pattern_desc_t hwss =
{
    .width  = 7,
    .height = 5,
    .name   = "Heavyweight spaceship",
    .pattern =
    {// 1 2 3 4 5 6 7
        _,_,X,X,_,_,_,
        X,_,_,_,_,X,_,
        _,_,_,_,_,_,X,
        X,_,_,_,_,_,X,
        _,X,X,X,X,X,X
    }
};

pattern_desc_t gosperglidergun =
{
    .width = 36,
    .height = 9,
    .name = "Gosper Glider gun",
    .pattern =
    {// 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,X,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,X,_,X,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,X,X,_,_,_,_,_,_,X,X,_,_,_,_,_,_,_,_,_,_,_,_,X,X,
        _,_,_,_,_,_,_,_,_,_,_,X,_,_,_,X,_,_,_,_,X,X,_,_,_,_,_,_,_,_,_,_,_,_,X,X,
        X,X,_,_,_,_,_,_,_,_,X,_,_,_,_,_,X,_,_,_,X,X,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        X,X,_,_,_,_,_,_,_,_,X,_,_,_,X,_,X,X,_,_,_,_,X,_,X,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,X,_,_,_,_,_,X,_,_,_,_,_,_,_,X,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,X,_,_,_,X,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,X,X,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_
    }
};

pattern_desc_t simkinglidergun =
{
    .width = 33,
    .height =14,
    .name = "Simkin Glider gun",
    .pattern =
    {// 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3
        X,X,_,_,_,_,_,X,X,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        X,X,_,_,_,_,_,X,X,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,X,X,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,X,X,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,X,X,_,X,X,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,X,_,_,_,_,_,X,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,X,_,_,_,_,_,_,X,_,_,X,X,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,X,X,X,_,_,_,X,_,_,_,X,X,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,X,_,_,_,_,_,_
    }
};

pattern_desc_t glider_stopper_below =
{
    .width  = 4,
    .height = 4,
    .name   = "Glider stopper below",
    .pattern =
    {// 1 2 3 4
        X,X,_,_,
        X,_,_,_,
        _,X,X,X,
        _,_,_,X
    }
};

pattern_desc_t glider_stopper_above =
{
    .width  = 4,
    .height = 4,
    .name   = "Glider stopper above",
    .pattern =
    {// 1 2 3 4
        X,_,_,_,
        X,X,X,_,
        _,_,_,X,
        _,_,X,X
    }
};

pattern_desc_t pentomino =
{
    .width  = 3,
    .height = 3,
    .name   = "Pentomino",
    .pattern =
    {// 1 2 3
        _,X,X,
        X,X,_,
        _,X,_
    }
};

pattern_desc_t diehard =
{
    .width  = 8,
    .height = 3,
    .name   = "Diehard",
    .pattern =
    {// 1 2 3 4 5 6 7 8
        _,_,_,_,_,_,X,_,
        X,X,_,_,_,_,_,_,
        _,X,_,_,_,X,X,X
    }
};

pattern_desc_t acorn =
{
    .width  = 7,
    .height = 3,
    .name   = "Acorn",
    .pattern =
    {// 1 2 3 4 5 6 7
        _,X,_,_,_,_,_,
        _,_,_,X,_,_,_,
        X,X,_,_,X,X,X
    }
};

// Engines

pattern_desc_t blockengine1 =
{
    .width  = 8,
    .height = 6,
    .name   = "Block engine 1",
    .pattern =
    {// 1 2 3 4 5 6 7 8
        _,_,_,_,_,_,X,_,
        _,_,_,_,X,_,X,X,
        _,_,_,_,X,_,X,_,
        _,_,_,_,X,_,_,_,
        _,_,X,_,_,_,_,_,
        X,_,X,_,_,_,_,_
    }
};

pattern_desc_t blockengine2 =
{
    .width  = 5,
    .height = 5,
    .name   = "Block engine 2",
    .pattern =
    {// 1 2 3 4 5
        X,X,X,_,X,
        X,_,_,_,_,
        _,_,_,X,X,
        _,X,X,_,X,
        X,_,X,_,X
    }
};

pattern_desc_t doubleblockengine =
{
    .width  = 39,
    .height = 1,
    .name   = "Double block engine",
    .pattern =
    {// 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9
        X,X,X,X,X,X,X,X,_,X,X,X,X,X,_,_,_,X,X,X,_,_,_,_,_,_,X,X,X,X,X,X,X,_,X,X,X,X,X
    }
};

pattern_desc_t ilove8bit =
{
    .width  = 27,
    .height = 5,
    .name   = "I love 8-bit",
    .pattern =
    {// 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
        X,_,_,_,X,_,X,_,_,_,X,X,X,_,_,_,_,_,X,X,_,_,X,_,X,X,X,
        X,_,_,X,X,X,X,X,_,_,X,_,X,_,_,_,_,_,X,_,X,_,X,_,_,X,_,
        X,_,_,X,X,X,X,X,_,_,X,X,X,_,X,X,X,_,X,X,_,_,X,_,_,X,_,
        X,_,_,_,X,X,X,_,_,_,X,_,X,_,_,_,_,_,X,_,X,_,X,_,_,X,_,
        X,_,_,_,_,X,_,_,_,_,X,X,X,_,_,_,_,_,X,X,_,_,X,_,_,X,_
    }
};



// Has to be with same content as enum "pattern_t" in "patterns.h"
pattern_desc_t *pattern_list[] =
{
    &conway,
    &conway_full,
    &block,
    &beehive,
    &loaf,
    &boat,
    &tub,
    &blinker,
    &toad,
    &beacon,
    &pulsar,
    &penta_decathlon,
    &glider,
    &glider_stopper_below,
    &glider_stopper_above,
    &lwss,
    &mwss,
    &hwss,
    &gosperglidergun,
    &simkinglidergun,
    &pentomino,
    &diehard,
    &acorn,
    &blockengine1,
    &blockengine2,
    &doubleblockengine,
    &ilove8bit
};
