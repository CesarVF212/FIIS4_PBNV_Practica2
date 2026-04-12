#include "BMPreader.h"
using namespace std;

int main(int argc, char** argv)
{
    BMPReader* reader16_24 = new BMPReader("pagoda16bit.bmp", "BITMAPV5_Header");
    BMPReader* reader16_32 = new BMPReader("pagoda16bit.bmp", "BITMAPV5_Header");

    BMPReader* reader32_16 = new BMPReader("pagoda32bit.bmp", "BITMAPV5_Header");
    BMPReader* reader32_24 = new BMPReader("pagoda32bit.bmp", "BITMAPV5_Header");

    reader16_24->convertImageTo24bpp();
    reader16_32->convertImageTo32bpp();

    reader32_16->convertImageTo16bpp();
    reader32_24->convertImageTo24bpp();

    reader16_24->writeBMP("pagoda16_24.bmp");
    reader16_32->writeBMP("pagoda16_32.bmp");
    reader32_16->writeBMP("pagoda32_16.bmp");
    reader32_24->writeBMP("pagoda32_24.bmp");

    // --- PARTE 2 --- //

    // Damos estos datos a un vector.
    string message = "Este es un mensaje secreto";
    vector <unsigned char> messageVector(message.begin(), message.end()); 
    
    // Añadimos al final un /0 para saber cuando parar de leer.
    messageVector.push_back('\0');
    reader32_24->hideRawData(messageVector);

    // Escalado de imagenes.
    unsigned short scaleFactor = 2;
    reader16_32->scaleImage(scaleFactor);
    reader16_32->writeBMP("scaledImage.bmp");

    // Eliminar reader;
    delete reader16_24;
    delete reader16_32;
    delete reader32_16;
    delete reader32_24;
}
