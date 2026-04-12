#pragma once

#include <type_traits>

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

// -- Conversiones de ByteRate --- //

// Vamos a usar especializaciones con templates para poder tener funciones genéricas para las conversiones.

// Declaración genérica.
template<typename From, typename To>
To convertPixel(From pixel);

// 24 -> 16
template<>
pixel16b convertPixel<pixel24b, pixel16b>(pixel24b pixel) 
{
    pixel16b res={0};
        
    unsigned short r5=(pixel.r>>3)&0x00FF;
    unsigned short g6=(pixel.g>>2)&0x00FF;
    unsigned short b5=(pixel.b>>3)&0x00FF;

    res.rawColor16=(r5<<11) | (g6<<5) | b5;

    return res;
}

// 24 -> 32
template<>
pixel32b convertPixel<pixel24b, pixel32b>(pixel24b pixel) 
{
    pixel32b res = {0};

    res.r = pixel.r;
    res.g = pixel.g;
    res.b = pixel.b;

    // No tenemos información del alpha, así que por defecto hacemos una conversión completamente opaca.
    res.a = 255;

    return res;
}

// 32 -> 24
template<>
pixel24b convertPixel<pixel32b, pixel24b>(pixel32b pixel) 
{   
    pixel24b res = {0};

    res.r = pixel.r;
    res.g = pixel.g;
    res.b = pixel.b;

    // Aquí simplemente ignoramos el canal alpha.
    return res;
}

// 32 -> 16
template<>
pixel16b convertPixel<pixel32b, pixel16b>(pixel32b pixel) 
{
    pixel16b res = {0};

    unsigned short r5=(pixel.r>>3)&0x00FF;
    unsigned short g6=(pixel.g>>2)&0x00FF;
    unsigned short b5=(pixel.b>>3)&0x00FF;

    res.rawColor16=(r5<<11) | (g6<<5) | b5;

    return res;
}