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
    
    // cout<<"Tamaño en bytes del fichero: " << reader -> bmpHeader -> fileSize <<endl;
    // cout<<"Tamaño de estructura BMPheader: " << sizeof(BMPReader::BMP_Header) <<endl;
    // cout<<"Tamaño de estructura DIPHeader: " << sizeof(BITMAPV5_Header) <<endl;
    // cout<<"Tamaño de Imagen BMP "<<reader->dibHeader->width<<"x"<<reader->dibHeader->height<<endl;

    // reader->applyFilter3x3(BMPReader::edgeHorizontalDetector);
    // reader2->applyFilter3x3(BMPReader::edgeVerticalDetector);

    // reader->mixImage(reader2);
    // reader->toGrayScale();
    //reader->convertImageTo16bpp();

    //reader->hideFile("mensaje.txt");

    // Escribir sobre los readers.

    reader16_24->writeBMP("pagoda16_24.bmp");
    reader16_32->writeBMP("pagoda16_32.bmp");
    reader32_16->writeBMP("pagoda32_16.bmp");
    reader32_24->writeBMP("pagoda32_24.bmp");

    // Eliminar reader;
    delete reader16_24;
    delete reader16_32;
    delete reader32_16;
    delete reader32_24;
}
