
#include "patterns.h"

#define X 1
#define _ 0

typedef struct
{
    const uint16_t width;
    const uint16_t height;
    const uint8_t pattern[];
} pattern_desc_t;



pattern_desc_t conway =
{
    // Conway
    .width  = 26,
    .height = 5,
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
    // Conway's Game of Life
    .width  = 44,
    .height = 11,
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
    // Block
    .width  = 2,
    .height = 2,
    .pattern =
    {// 1 2
        X,X,
        X,X
    }
};

pattern_desc_t beehive =
{
    // Beehive
    .width  = 4,
    .height = 3,
    .pattern =
    {// 1 2 3 4
        _,X,X,_,
        X,_,_,X,
        _,X,X,_
    }
};

pattern_desc_t loaf =
{
    // Loaf
    .width  = 4,
    .height = 4,
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
    // Boat
    .width  = 3,
    .height = 3,
    .pattern =
    {// 1 2 3
        X,X,_,
        X,_,X,
        _,X,_
    }
};

pattern_desc_t tub =
{
    // Tub
    .width  = 3,
    .height = 3,
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
    // Blinker
    .width  = 3,
    .height = 1,
    .pattern =
    {// 1 2 3
        X,X,X
    }
};

pattern_desc_t toad =
{
    // Toad
    .width  = 4,
    .height = 2,
    .pattern =
    {// 1 2 3 4
        _,X,X,X,
        X,X,X,_
    }
};

pattern_desc_t beacon =
{
    // Beacon
    .width  = 4,
    .height = 4,
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
    // Pulsar
    .width  = 13,
    .height = 13,
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

pattern_desc_t octagon =
{
    // Octagon
    .width  = 6,
    .height = 6,
    .pattern =
    {// 1 2 3 4 5 6
        _,X,_,_,X,_,
        X,_,X,X,_,X,
        _,X,_,_,X,_,
        _,X,_,_,X,_,
        X,_,X,X,_,X,
        _,X,_,_,X,_
    }
};

pattern_desc_t tumbler =
{
    // Tumbler
    .width  = 9,
    .height = 5,
    .pattern =
    {// 1 2 3 4 5 6 7 8 9
        _,X,_,_,_,_,_,X,_,
        X,_,X,_,_,_,X,_,X,
        X,_,_,X,_,X,_,_,X,
        _,_,X,_,_,_,X,_,_,
        _,_,X,X,_,X,X,_,_
    }
};

pattern_desc_t penta_decathlon =
{
    // Penta-decathlon
    .width  = 10,
    .height = 3,
    .pattern =
    {// 1 2 3 4 5 6 7 8 9 0
        _,_,X,_,_,_,_,X,_,_,
        X,X,_,X,X,X,X,_,X,X,
        _,_,X,_,_,_,_,X,_,_
    }
};

pattern_desc_t glider =
{
    // Glider
    .width  = 3,
    .height = 3,
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
    // Lightweight spaceship
    .width  = 5,
    .height = 4,
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
    // Middleweight spaceship
    .width  = 6,
    .height = 5,
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
    // Heavyweight spaceship
    .width  = 7,
    .height = 5,
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
    // Gosper Glider gun
    .width = 36,
    .height = 9,
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
    // Simkin Glider gun
    .width = 33,
    .height =14,
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
    // Glider stopper below
    .width  = 4,
    .height = 4,
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
    // Glider stopper above
    .width  = 4,
    .height = 4,
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
    // Pentomino
    .width  = 3,
    .height = 3,
    .pattern =
    {// 1 2 3
        _,X,X,
        X,X,_,
        _,X,_
    }
};

pattern_desc_t diehard =
{
    // Die hard
    .width  = 8,
    .height = 3,
    .pattern =
    {// 1 2 3 4 5 6 7 8
        _,_,_,_,_,_,X,_,
        X,X,_,_,_,_,_,_,
        _,X,_,_,_,X,X,X
    }
};

pattern_desc_t acorn =
{
    // Acorn
    .width  = 7,
    .height = 3,
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
    // Block engine 1
    .width  = 8,
    .height = 6,
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
    // Block engine 2
    .width  = 5,
    .height = 5,
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
    // Double block engine
    .width  = 39,
    .height = 1,
    .pattern =
    {// 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9
        X,X,X,X,X,X,X,X,_,X,X,X,X,X,_,_,_,X,X,X,_,_,_,_,_,_,X,X,X,X,X,X,X,_,X,X,X,X,X
    }
};

pattern_desc_t ilove8bit =
{
    // I love 8-bit
    .width  = 27,
    .height = 5,
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
    &octagon,
    &tumbler,
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
