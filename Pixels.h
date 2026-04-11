#pragma once

// -- Estructura de pixeles de 24 bits -- //
typedef struct 
{
    union
    {
        struct
        {
            unsigned char r;
            unsigned char g;
            unsigned char b;
        };
        struct
        {
            unsigned char red;
            unsigned char green;
            unsigned char blue;
        };
    }; 
}
pixel24b;


// -- Estructura de pixeles de 32 bits -- //
typedef struct 
{
    union
    {
        struct
        {
            unsigned char r;
            unsigned char g;
            unsigned char b;
            unsigned char a;
        };
        struct
        {
            unsigned char red;
            unsigned char green;
            unsigned char blue;
            unsigned char alpha;
        };
    }; 
}
pixel32b;


// -- Estructura de pixeles de 16 bits -- //
typedef struct 
{
    short int rawColor16;
}
pixel16b;


// -- Estructura de almacenamiento de datos de imagen -- //
typedef struct{
    union{
        pixel32b* pixels32;
        pixel24b* pixels24;
        pixel16b* pixels16b;
        char* rawPixels;
    };
    unsigned short type; // 8,16,24,32 bits por pixel
}pixel_t;