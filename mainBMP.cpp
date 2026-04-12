#include "BMPreader.h"

int main(int argc, char** argv)
{

    // --- PARTE 1 --- //
    // Pruebas de cambiado de formato.

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
    // Encriptación mendiante vector de caracteres.

    // Damos estos datos a un vector.
    string message = "En un lugar de la Mancha, de cuyo nombre no quiero acordarme, no ha mucho tiempo que vivía un hidalgo de los de lanza en astillero, adarga antigua, rocín flaco y galgo corredor";
    vector <unsigned char> messageVector(message.begin(), message.end()); 
    
    // Añadimos al final un /0 para saber cuando parar de leer.
    messageVector.push_back('\0');
    reader32_24->hideRawData(messageVector);

    // Escalado de imagenes.
    unsigned short scaleFactor = 2;
    reader16_32->scaleImage(scaleFactor);
    reader16_32->writeBMP("scaledImage.bmp");

    // reader32_24->recoverHiddenData();


    // --- PARTE 2.1 --- //

    // Programa tipo main para encriptar texto en imagen.
    
    bool end = false;
    char lastRead = 0;

    // Nos aseguramos que haya el número de argumentos correcto.
    if(argc < 3)
    {
        std::cout << "No se han introducido los argumentos correctos. Se deben de especificar dos nombres de archivo [1: Archivo de Imagen | 2: Archivo de Texto]";
        return 1;
    }

    while(end != true){

        std::cout << "\n1 - Hide \n";
        std::cout << "2 - Recover \n";
        std::cout << "E - End \n";

        std::cin >> lastRead;

        if(lastRead == '1')
        {
            BMPReader* hideReader = new BMPReader(argv[1], "BITMAPV5_Header");

            hideReader->hideFile(argv[2]);
            std::cout << "\nInformacion guardada en la imagen.\n";

            // Escribimos la imagen. De forma que sea la que queda guardada.
            hideReader->writeBMP(argv[1]);
            delete hideReader;
        }

        if(lastRead == '2')
        {
            BMPReader* recoverReader = new BMPReader(argv[1], "BITMAPV5_Header");
            recoverReader->recoverHiddenData();
            delete recoverReader;
        }
        
        if(lastRead == 'E' || lastRead == 'e')
            end = true;
        
    }

    // Eliminar reader;
    delete reader16_24;
    delete reader16_32;
    delete reader32_16;
    delete reader32_24;

    return 0;
}
