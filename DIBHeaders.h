#pragma once


// --- CABECERAS DIP --- //

    // unsigned int = 4 bytes.
    // unsigned short = 2bytes.


typedef struct __attribute__ ((packed))
{
    unsigned int headerSize; 
    unsigned int width;
    unsigned int height; 
    unsigned short numLayers; // Número de capas. Usamos 1.
    unsigned short bpp; // Bits por pixel.
    unsigned int compressMethod; // Método de compresión: por defecto 0.
    unsigned int imageSize;
    unsigned int hDepth;
    unsigned int vDepth;
    unsigned int paletteColors; // Atributos de la paleta.
    unsigned int importantColors;   // Colores importantes de la paleta.

    static const unsigned short headerSizeInBytes = 40;

}
BITMAPINFO_Header;

typedef struct __attribute__ ((packed))
{
    unsigned int headerSizeInBytes;
    unsigned int width;
    unsigned int height;
    unsigned short numColorPlanes;
    unsigned short bitsPerPixel;
    unsigned int compressionMethod;
    unsigned int rawDataSizeInBytes;
    unsigned int hPelsPerMeter;
    unsigned int vPelsPerMeter;
    unsigned int numColorsInPalette;
    unsigned int importantColors;
    unsigned int redChannelBitMask;
    unsigned int greenChannelBitMask;
    unsigned int blueChannelBitMask;
    unsigned int alphaChannelBitMask;
    unsigned int colorSpaceType;
    unsigned int endpoints[9];  // Definición del espacio de color. Por defecto: LCS_Windows_Color_Space
    unsigned int gammaRed;
    unsigned int gammaGreen;
    unsigned int gammaBlue;
    unsigned int intent;        // Tipo de renderizado.
    unsigned int profileData;   // Offset o datos del perfil de color.
    unsigned int profileSize;   // Tamaño en Bytes del ICC.
    unsigned int reserved;      // Reservado, siempre 0.
    static const unsigned short headerSizeBytes = 124;
}
BITMAPV5_Header;