#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "command.h"
#include "coder.h"

int main(int argc, char **argv)
{
    char example1[] = "encode";
    char example2[] = "decode";
    if ( argc != 4 || ( strcmp(argv[1], example1) != 0 && strcmp(argv[1], example2) != 0) )
    {
        printf("Usage:\ncoder encode <in-file-name> <out-file-name>\n");
        return -1;
    }
    if ( !strcmp(argv[1], example1) )
        encode_file(argv[2], argv[3]);
    else
        decode_file(argv[2], argv[3]);
    return 0;
}
