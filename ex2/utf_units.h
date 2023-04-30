#pragma once

enum {
    MaxCodeLength = 4
};

typedef struct
{
    uint8_t code[MaxCodeLength];
    size_t lenght;
} CodeUnits;
