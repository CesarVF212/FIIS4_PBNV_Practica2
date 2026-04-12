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

// --- Conversiones a 16 BPP --- //

template<typename From, typename To>
To to16BPP(From pixel);

// 24 -> 16
template<>
pixel16b to16BPP<pixel24b, pixel16b>(pixel24b pixel) 
{
    pixel16b res={0};
        
    unsigned short r5=(pixel.r>>3)&0x00FF;
    unsigned short g6=(pixel.g>>2)&0x00FF;
    unsigned short b5=(pixel.b>>3)&0x00FF;

    res.rawColor16=(r5<<11) | (g6<<5) | b5;

    return res;
}

// 32 -> 16
template<>
pixel16b to16BPP<pixel32b, pixel16b>(pixel32b pixel) 
{
    pixel16b res = {0};

    unsigned short r5=(pixel.r>>3)&0x00FF;
    unsigned short g6=(pixel.g>>2)&0x00FF;
    unsigned short b5=(pixel.b>>3)&0x00FF;

    res.rawColor16=(r5<<11) | (g6<<5) | b5;

    return res;
}


// --- Conversiones a 24 BPP --- //

template<typename From, typename To>
To to24BPP(From pixel);

// 16 -> 24

pixel24b to24BPP<pixel16b, pixel24b>(pixel16b pixel) 
{   
    pixel24b res = {0};

    unsigned short r5 = (pixel.rawColor16 >> 11) & 0x1F;  // 5 Bits.
    unsigned short g6 = (pixel.rawColor16 >> 5)  & 0x3F;  // 6 Bits.
    unsigned short b5 =  pixel.rawColor16        & 0x1F;  // 5 Bits.

    // Hacemos una operación de insertado. Desplazamos los bits a la izquierda para que lleguen al valor del canal. En este caso 6 Bits, depende del canal se hace shift de 2 o de 3.
    // Luego, se rellenan los bits de abajo con el valor mas significativo. No termino de entender esto pero por lo que he visto es como la "convección". Supongo que hará mas precisa la conversión.
    // Mi idea iniciar era hacer una "regla de tres" a 8 Bits, pero esto es computacionalmente mas costoso, según estuve mirando.
    res.r = (r5 << 3) | (r5 >> 2);
    res.g = (g6 << 2) | (g6 >> 4);
    res.b = (b5 << 3) | (b5 >> 2);
    
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


// --- Conversiones a 32 BPP --- //

template<typename From, typename To>
To to32BPP(From pixel);

// 16 -> 32
pixel24b to32BPP<pixel16b, pixel32b>(pixel16b pixel) 
{   
    pixel24b res = {0};

    unsigned short r5 = (pixel.rawColor16 >> 11) & 0x1F;  // 5 Bits.
    unsigned short g6 = (pixel.rawColor16 >> 5)  & 0x3F;  // 6 Bits.
    unsigned short b5 =  pixel.rawColor16        & 0x1F;  // 5 Bits.

    res.r = (r5 << 3) | (r5 >> 2);
    res.g = (g6 << 2) | (g6 >> 4);
    res.b = (b5 << 3) | (b5 >> 2);

    // Completamente opaco.
    res.a = 255;

    return res;
}

// 24 -> 32
template<>
pixel32b to32BPP<pixel24b, pixel32b>(pixel24b pixel) 
{
    pixel32b res = {0};

    res.r = pixel.r;
    res.g = pixel.g;
    res.b = pixel.b;

    // No tenemos información del alpha, así que por defecto hacemos una conversión completamente opaca.
    res.a = 255;

    return res;
}