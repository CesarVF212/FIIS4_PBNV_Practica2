#pragma once

typedef struct __attribute__ ((packed))
{
    unsigned char format[2];
    unsigned int fileSize;
    unsigned char optional[4];
    unsigned int offset;
}
BMP_Header;