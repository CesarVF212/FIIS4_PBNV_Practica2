#pragma once
using namespace std;

// --- CABECERAS DIP --- //
    // unsigned int = 4 bytes.
    // unsigned short = 2bytes.


typedef struct __attribute__ ((packed)) DIB40_Header
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
}
DIB40_Header;


typedef struct __attribute__ ((packed)) BITMAPV5_Header
{
    unsigned int headerSize;
    unsigned int width;
    unsigned int height;
    unsigned short numColorPlanes;
    unsigned short bpp;
    unsigned int compressMethod;
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
}
BITMAPV5_Header;


// --- FUNCIÓN DEBUG: Imprimir las cabeceras completas --- //
void printDIB40_Header(DIB40_Header* header)
{
    cout << "--- DIB40 Header ---" << endl;
    cout << "Header size: "       << header->headerSize << endl;
    cout << "Width: "             << header->width << endl;
    cout << "Height: "            << header->height << endl;
    cout << "Num layers: "        << header->numLayers << endl;
    cout << "BPP: "               << header->bpp << endl;
    cout << "Compress method: "   << header->compressMethod << endl;
    cout << "Image size: "        << header->imageSize << endl;
    cout << "H depth: "           << header->hDepth << endl;
    cout << "V depth: "           << header->vDepth << endl;
    cout << "Palette colors: "    << header->paletteColors << endl;
    cout << "Important colors: "  << header->importantColors << endl;
}

void printBITMAPV5_Header(BITMAPV5_Header* header)
{
    cout << "--- BITMAPV5 Header ---" << endl;
    cout << "Header size: "           << header->headerSize << endl;
    cout << "Width: "                 << header->width << endl;
    cout << "Height: "                << header->height << endl;
    cout << "Num color planes: "      << header->numColorPlanes << endl;
    cout << "BPP: "                   << header->bpp << endl;
    cout << "Compression method: "    << header->compressMethod << endl;
    cout << "Raw data size: "         << header->rawDataSizeInBytes << endl;
    cout << "H pels per meter: "      << header->hPelsPerMeter << endl;
    cout << "V pels per meter: "      << header->vPelsPerMeter << endl;
    cout << "Num colors in palette: " << header->numColorsInPalette << endl;
    cout << "Important colors: "      << header->importantColors << endl;
    cout << "Red channel mask: "      << header->redChannelBitMask << endl;
    cout << "Green channel mask: "    << header->greenChannelBitMask << endl;
    cout << "Blue channel mask: "     << header->blueChannelBitMask << endl;
    cout << "Alpha channel mask: "    << header->alphaChannelBitMask << endl;
    cout << "Color space type: "      << header->colorSpaceType << endl;
    cout << "Gamma red: "             << header->gammaRed << endl;
    cout << "Gamma green: "           << header->gammaGreen << endl;
    cout << "Gamma blue: "            << header->gammaBlue << endl;
    cout << "Intent: "                << header->intent << endl;
    cout << "Profile data: "          << header->profileData << endl;
    cout << "Profile size: "          << header->profileSize << endl;
    cout << "Reserved: "              << header->reserved << endl;
}