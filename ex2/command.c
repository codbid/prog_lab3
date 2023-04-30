#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>

#include "utf_units.h"
#include "coder.h"

int encode_file(const char *in_file_name, const char *out_file_name)
{
    FILE *in = fopen(in_file_name, "r");
    FILE *out = fopen(out_file_name, "wb");
    if ( !in || !out )
    {
        return -1; // Ошибка открытия файлов
    }

    uint32_t code_point;
    while (fscanf(in, "%" SCNx32, &code_point) >= 0)
    {
        CodeUnits *code_units = calloc(1, sizeof(CodeUnits));
        if (encode(code_point, code_units) < 0)
        {
            return -1; //Ошибка при кодировании
        }
        write_code_unit(out, code_units);
        free(code_units);
    }
    fclose(in); fclose(out);
}

int decode_file(const char *in_file_name, const char *out_file_name)
{
    FILE *in = fopen(in_file_name, "rb");
    FILE *out = fopen(out_file_name, "w");
    if ( !in || !out )
    {
        return -1; // Ошибка открытия файлов
    }

    CodeUnits *code_units = calloc(1, sizeof(CodeUnits));
    while ( !read_next_code_unit(in, code_units) )
    {
        uint32_t code_point = decode(code_units);
        if (fprintf(out, "%" PRIx32 "\n", code_point) < 1)
            return -1; // Ошибка записи
    }
    fclose(in); fclose(out);

}