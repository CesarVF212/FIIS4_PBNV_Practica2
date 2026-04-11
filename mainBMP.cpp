#include "BMPreader.h"
using namespace std;

int main(int argc, char** argv)
{
    BMPReader* reader=new BMPReader("FLAG_B24.BMP");
    BMPReader* reader2=new BMPReader("FLAG_B24.BMP");

    cout<<"Tamaño en bytes del fichero: "<<reader->bmpHeader->fileSize<<endl;
    cout<<"Tamaño de estructura BMPheader: "<<sizeof(BMPReader::BMP_Header)<<endl;
    cout<<"Tamaño de Imagen BMP "<<reader->dibHeader->width<<"x"<<reader->dibHeader->height<<endl;

    reader->applyFilter3x3(BMPReader::edgeHorizontalDetector);
    reader2->applyFilter3x3(BMPReader::edgeVerticalDetector);

    reader->mixImage(reader2);
    reader->toGrayScale();
    //reader->convertImageTo16bpp();

    //reader->hideFile("mensaje.txt");

    reader->writeBMP("nuevoBMP.bmp");

    delete reader;


}
