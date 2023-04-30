#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define COUNT 1000000

uint32_t generate_number()
{
    const int r = rand();
    const int p = r % 100;
    if (p < 90) {
        return r % 128;
    }
    if (p < 95) {
        return r % 16384;
    }
    if (p < 99) {
        return r % 2097152;
    }
    return r % 268435455;
}


size_t encode_varint(uint32_t value, uint8_t* buf)
{
    assert(buf != NULL);
    uint8_t* cur = buf;
    while (value >= 0x80) {
        const uint8_t byte = (value & 0x7f) | 0x80;
        *cur = byte;
        value >>= 7;
        ++cur;
    }
    *cur = value;
    ++cur;
    return cur - buf;
}

uint32_t decode_varint(const uint8_t** bufp)
{
    const uint8_t* cur = *bufp;
    uint8_t byte = *cur++;
    uint32_t value = byte & 0x7f;
    size_t shift = 7;
    while (byte >= 0x80)
    {
        byte = *cur++;
        value += (byte & 0x7f) << shift;
        shift += 7;
    }
    *bufp = cur;
    return value;
}

void print_buf(uint8_t* buf, size_t size) {
    for (size_t i = 0; i < size; i++) {
        for (int j = 7; j >= 0; j--) {
            printf("%d", (buf[i] >> j) & 1);
        }
        printf(" ");
    }
    printf("\n");
}

int main()
{
    FILE *fpu = fopen("uncompressed.dat", "wb");
    FILE *fpc = fopen("compressed.bin", "wb");
    if (fpu == NULL || fpc == NULL) {
        printf("Failed to open file.\n");
        return -1;
    }
    for (int i = 0; i < COUNT; i++) {
        uint32_t r = generate_number();
        uint8_t buf[10];
        size_t encode_size = encode_varint(r, buf);
        fwrite(&r, sizeof(r), 1, fpu);
        fwrite(buf, sizeof(uint8_t), encode_size, fpc);

    }
    printf("\n");
    fclose(fpu); fclose(fpc);
    fpu = fopen("uncompressed.dat", "rb");
    fpc = fopen("compressed.bin", "rb");
    for (int i = 0; i < COUNT; i++) {
        uint32_t num_u;
        fread(&num_u, sizeof(uint32_t), 1, fpu);
        uint8_t buffer[4];
        for (int j = 0; j < 4; j++)
        {
            fread(&buffer[j], sizeof(uint8_t), 1, fpc);
            if ((buffer[j] >> 7) == 0)
                break;
            
        }
        const uint8_t *temp[1];
        temp[0] = buffer;
        uint32_t num_c = decode_varint(&temp[0]);
        if ( num_c != num_u )
            printf("The numbers don't match: %d != %d\n", num_u, num_c);

    }
    fclose(fpu); fclose(fpc);
    return 0;
}