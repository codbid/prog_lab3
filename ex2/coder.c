#include <stdio.h>
#include <inttypes.h>
#include <stdio.h>

#include "utf_units.h"

int encode(uint32_t code_point, CodeUnits *code_units)
{
    int nLen = 0;
    if (code_point < 0b100000000)
    {
        code_units->lenght = 1;
        code_units->code[0] = code_point;
    }
    else if (code_point < 0b100000000000) 
    {
        code_units->lenght = 2;
        code_units->code[0] = 0b11000000 | (code_point >> 6);
        code_units->code[1] = 0b10000000 | (0b111111 & code_point);
    }
    else if (code_point < 0b10000000000000000)
    {
        code_units->lenght = 3;
        code_units->code[0] = 0b11100000 | (code_point >> 12);
        code_units->code[1] = 0b10000000 | (0b111111 & (code_point >> 6));
        code_units->code[2] = 0b10000000 | (0b111111 & code_point);
    }
    else if (code_point < 0b1000000000000000000000)
    {
        code_units->lenght = 4;
        code_units->code[0] = 0b11110000 | (code_point >> 18);
        code_units->code[1] = 0b10000000 | (0b111111 & (code_point >> 12));
        code_units->code[2] = 0b10000000 | (0b111111 & (code_point >> 6));
        code_units->code[3] = 0b10000000 | (0b111111 & code_point);
    }
    else
        return -1;
    return 0;
}

uint32_t decode(const CodeUnits *code_units)
{
    uint32_t temp = 0;
    if ( code_units->lenght == 1 )
    {
        return code_units->code[0];
    }
    else if ( code_units->lenght == 2 )
    {
        temp = 0b11111 & code_units->code[0];
        temp = (temp << 6) | ( 0b111111 & code_units->code[1] );
    }
    else if ( code_units->lenght == 3 )
    {
        temp = 0b1111 & code_units->code[0];
        temp = (temp << 6) | (0b111111 & code_units->code[1]);
        temp = (temp << 6) | (0b111111 & code_units->code[2]);
    }
    else if ( code_units->lenght == 4 )
    {
        temp = 0b111 & code_units->code[0];
        temp = (temp << 6) | (0b111111 & code_units->code[1]);
        temp = (temp << 6) | (0b111111 & code_units->code[2]);
        temp = (temp << 6) | (0b111111 & code_units->code[3]);
    }
    return temp;
}

int write_code_unit(FILE *out, const CodeUnits *code_units)
{
    if (code_units == NULL)
        return -1;


    if (code_units->lenght < 1 || code_units->lenght > 4)
        return -1;


    if (fwrite(code_units->code, sizeof(uint8_t), code_units->lenght, out) != code_units->lenght)
        return -1;

    return 0;
}

int read_next_code_unit(FILE *in, CodeUnits *code_units) 
{
    uint8_t temp;
    size_t length = 0;

    while ( !length )
    {
        if (fread(&temp, sizeof(uint8_t), 1, in) != 1) 
        {
            return -1;  // Ошибка чтения
        }

        if ((temp & 0b10000000) == 0b00000000) 
        {
            length = 1;
            code_units->code[0] = temp;
        } else if ((temp & 0b11100000) == 0b11000000) 
        {
            length = 2;
            code_units->code[0] = temp;
        } else if ((temp & 0b11110000) == 0b11100000) 
        {
            length = 3;
            code_units->code[0] = temp;
        } else if ((temp & 0b11111000) == 0b11110000) 
        {
            length = 4;
            code_units->code[0] = temp;
        }
    }
    code_units->lenght = length;
    code_units->code[0] = temp;
    for ( int i = 1; i < length; i++ )
    {
        if (fread(&temp, sizeof(uint8_t), 1, in) != 1) 
        {
            return -1;  // Ошибка чтения
        }

        if ( (temp & 0b11000000) != 0b10000000 )
        {
            return -1; // Некорректный байт
        }

        code_units->code[i] = temp;
    }
    return 0;
}