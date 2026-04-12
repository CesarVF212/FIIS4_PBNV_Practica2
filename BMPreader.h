#pragma once
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <string>
#include <string.h>

#include "BMPHeader.h"
#include "DIBHeaders.h"
#include "Pixels.h"

using namespace std;


class BMPReader{
public:

    typedef struct __attribute__ ((packed)){
        unsigned char format[2];
        unsigned int fileSize;
        unsigned char optional[4];
        unsigned int offset;
    }BMP_Header;

    vector<char>* rawData;  // Datos brutos leídos de fichero.
    
    // Cabecera BMP.
    BMP_Header* bmpHeader;

    // Cabecera DIB.
    //DIB40_Header* dibHeader; 
    BITMAPV5_Header* dibHeader;

    pixel_t pixelData; // Array de pixeles de imagen

    vector<unsigned char> hiddenData;
    bool isDataHidden = false;

    vector<char>* readFile(string file){
        ifstream fIn( file, ifstream::binary );
        vector<char>* bytes=new vector<char>(istreambuf_iterator<char>(fIn), {});
        fIn.close();
        return bytes;
    }

    BMPReader(string fileName, string dipHeader){
        //abrir fichero, leer datos
        auto rawData=readFile(fileName);

        // Parsear cabecera BMP.
        bmpHeader=(BMP_Header*)rawData->data();

        // Parsear cabecera DIP.
        dibHeader=(BITMAPV5_Header*)(rawData->data() + sizeof(BMP_Header));
        printBITMAPV5_Header(dibHeader);
        //dibHeader=(DIB40_Header*)(rawData->data()+sizeof(BMP_Header));
            
        // Apuntar datos de imagen
        pixelData.type=dibHeader->bpp;
        pixelData.rawPixels=( char*)(rawData->data()+bmpHeader->offset);

    }


    void writeBMP(string newFileName)
    {

        ofstream fout( newFileName, ofstream::binary ); //crear fichero en modo escritura

        fout.write((char*)bmpHeader,sizeof(BMP_Header));
        fout.write((char*)dibHeader,dibHeader->headerSize); 


        size_t imageSize=(dibHeader->bpp/8)*dibHeader->width *dibHeader->height;

        fout.write((char*)pixelData.rawPixels, imageSize );
        fout.close();//salvar datos

    }


    static inline const float bilinearFilter[3][3]={
    {  1.0f/9.0f, 1.0f/9.0f, 1.0f/9.0f },
    {  1.0f/9.0f, 1.0f/9.0f, 1.0f/9.0f },
    {  1.0f/9.0f, 1.0f/9.0f, 1.0f/9.0f }};

    static inline const float edgeVerticalDetector[3][3]={
        {1, 0, -1},
        {1, 0, -1},
        {1, 0, -1}};
    static inline const float edgeHorizontalDetector[3][3]={{1, 1, 1},
                                        {0, 0, 0},
                                        {-1, -1, -1}};


    void applyFilter3x3(const float filter[3][3])
    {

        //por cada fila
        pixel24b* newPixels=new pixel24b[dibHeader->width*dibHeader->height];

        for(int y=0;y<dibHeader->height;y++)
            for(int x=0;x<dibHeader->width;x++) //por cada columna
            {

                float r=0, g=0,b=0;
                //pixeles alrededor
                for(int i=-1;i<=1;i++) //los superiores e inferiores
                    for(int j=-1;j<=1;j++) //los izquierdos a derechos
                    {
                        int accessX=((x+j)+dibHeader->width)%dibHeader->width;
                        int accessY=((y+i)+dibHeader->height)%dibHeader->height;

                        r+=(pixelData.pixels24[accessY*dibHeader->width+ accessX].r)* filter[i+1][j+1];
                        g+=(pixelData.pixels24[accessY*dibHeader->width+ accessX].g)* filter[i+1][j+1];
                        b+=(pixelData.pixels24[accessY*dibHeader->width+ accessX].b)* filter[i+1][j+1];

                    }
                newPixels[y*dibHeader->width+x]={(unsigned char)r,(unsigned char)g,(unsigned char)b};
            }

        //copiar resultados
        size_t pixelSize=pixelData.type/8;
        memcpy(pixelData.rawPixels,newPixels, pixelSize*dibHeader->width*dibHeader->height);

    }


    // --- CONVERSORES DE BYTE-RATE --- //

    // -> 16 bpp.
    void convertImageTo16bpp(){
        
        pixel16b* newPixels=new pixel16b[dibHeader->width * dibHeader->height];

        for(int y = 0 ; y < dibHeader->height ; y++)
        {
            for(int x = 0 ; x < dibHeader->width ; x++)
            {
                int i = y * dibHeader->width + x;
                if(pixelData.type == 24)
                    newPixels[i] = to16BPP(pixelData.pixels24[i]);
                else if(pixelData.type == 32)
                    newPixels[i] = to16BPP(pixelData.pixels32[i]);
            }
        }
        pixelData.rawPixels = (char*)newPixels;
        dibHeader->bpp=16;
        pixelData.type=16;

        // Actualizamos las mascaras de bits.
        dibHeader->redChannelBitMask   = 0xF800; // 11111 000000 00000
        dibHeader->greenChannelBitMask = 0x07E0; // 00000 111111 00000
        dibHeader->blueChannelBitMask  = 0x001F; // 00000 000000 11111
        dibHeader->alphaChannelBitMask = 0x0000; // Sin transparencia
        dibHeader->compressMethod      = 3;      // BI_BITFIELDS
        dibHeader->compressMethod = 3; 
    }

    // -> 24 bpp.
    void convertImageTo24bpp(){
        
        pixel24b* newPixels=new pixel24b[dibHeader->width * dibHeader->height];

        for(int y = 0 ; y <dibHeader->height ; y++)
        {
            for(int x = 0 ; x<dibHeader->width ; x++) 
            {
                int i = y * dibHeader->width + x;
                if(pixelData.type == 16)
                    newPixels[i] = to24BPP(pixelData.pixels16b[i]);
                else if(pixelData.type == 32)
                    newPixels[i] = to24BPP(pixelData.pixels32[i]);
            }
        }
        
        pixelData.rawPixels = (char*)newPixels;
        dibHeader->bpp=24;
        pixelData.type=24;

        dibHeader->redChannelBitMask   = 0;
        dibHeader->greenChannelBitMask = 0;
        dibHeader->blueChannelBitMask  = 0;
        dibHeader->alphaChannelBitMask = 0;
        dibHeader->compressMethod = 0;
        
    }

    // -> 32 bpp.
    void convertImageTo32bpp(){
        
        pixel32b* newPixels=new pixel32b[dibHeader->width * dibHeader->height];

        for(int y = 0 ; y < dibHeader->height ; y++)
        {
            for(int x = 0 ; x < dibHeader->width ; x++)
            {
                int i = y * dibHeader->width + x;
                if(pixelData.type == 16)
                    newPixels[i] = to32BPP(pixelData.pixels16b[i]);
                else if(pixelData.type == 24)
                    newPixels[i] = to32BPP(pixelData.pixels24[i]);
            }
        }
        //copiar resultados
        pixelData.rawPixels = (char*)newPixels;
        dibHeader->bpp=32;
        pixelData.type=32;

        dibHeader->redChannelBitMask   = 0x00FF0000;
        dibHeader->greenChannelBitMask = 0x0000FF00;
        dibHeader->blueChannelBitMask  = 0x000000FF;
        dibHeader->alphaChannelBitMask = 0xFF000000;
        dibHeader->compressMethod = 3;
    }

    // --- ESCONDER TEXTO EN IMAGEN --- //

    void hideRawData(vector<unsigned char>& data)
    {
        // Trabajar con imagenes de 24bit
        if(dibHeader->bpp!=24)
        {
            std::cout << "AVISO: imagen no soportada. Se acepta formato de 24 Bits unicamente. Se convertirá primero. \n";
            this->convertImageTo24bpp();
        }

        int numPixels=dibHeader->width*dibHeader->height;
        int contador = 0;
        
        // Por cada byte en el fichero
        for(unsigned char c : data)
        {
            if(contador<numPixels){
            // Escondemos un pixel y avanzamos al siguiente.
                hideByte24bpp(pixelData.pixels24[contador] , c);
                contador++;
            }
        }

        this->isDataHidden = true;
    }

    void recoverHiddenData()
    {
        // Trabajar con imagenes de 24bit
        if(dibHeader->bpp!=24)
        {
            std::cout << "AVISO: imagen no soportada. Se acepta formato de 24 Bits unicamente. Se convertirá primero. \n";
            this->convertImageTo24bpp();
        }

        if(this->isDataHidden == false)
        {
            std::cout << "Esta imagen no tiene información oculta";
            return;
        }

        unsigned char lastCharacterRead = '\n'; // Evitamos que pueda haber información basura.
        vector<unsigned char> hiddenMessage;
        
        int numPixels=dibHeader->width*dibHeader->height;
        
        int contador = 0;
        
        // Nos aseguramos no intentar leer fuera de la memoria asignada. Pararemos de leer al detectar el caracter de final de cadena.
        while(lastCharacterRead != '\0' && contador < numPixels)
        {
            lastCharacterRead = recoverByte42bpp(pixelData.pixels24[contador]);
            hiddenMessage.push_back(lastCharacterRead);
            contador++;
        }

        std::cout << "El mensaje secreto es: \t";
        for(unsigned char c : hiddenMessage)
            std::cout << c;

    }

    unsigned char recoverByte42bpp(pixel24b& pixel)
{
    unsigned char redMask = 0b00000111;
    unsigned char greenMask = 0b00000011;
    unsigned char blueMask = 0b00000111;

    unsigned char res = 0;

    res |= (pixel.r & redMask);
    res |= (pixel.g & greenMask) << 3;
    res |= (pixel.b & blueMask) << 5;

    return res;
}

    void hideByte24bpp(pixel24b& pixel, unsigned char byte)
    {

        unsigned char redMask=0b11111000; //mantener 5 bits mas importantes
        pixel.r=pixel.r & redMask; //borrar 3 bits menos importantes
        unsigned char data3bitDerecha=0b00000111; //elige 3 bits de la derech
        pixel.r=pixel.r| (byte&data3bitDerecha); //guardo 3 bits de la derecha en "rojo"

        unsigned char greenMask=0b11111100; //mantener 6 bits mas importantes
        pixel.g=pixel.g & greenMask; //borrar 2 bits menos importante
        byte=byte>>3; //muevo los bits 3 y 4 a la derecha
        unsigned char data2bitDerecha=0b00000011; //elige 2 bits de la derech
        pixel.g=pixel.g| (byte&data2bitDerecha); //guardo 2 bits de la derecha en "verde"

        byte=byte>>2; //muevo los bits 5 6 7 a la derecha
        pixel.b=pixel.b & redMask; //borrar 3 bits menos importantes
        pixel.b=pixel.b| (byte&data3bitDerecha); //guardo 3 bits de la derecha en "azul"

    }


    void hideFile(string fileName)
    {
        //trabajar con imagenes de 24bit
        if(dibHeader->bpp!=24)
            cout<<"ERROR: imagen no soportada\n";

        auto fileData=readFile(fileName);

        int numPixels=dibHeader->width*dibHeader->height;
        int contador=0;
        //por cada byte en el fichero
        for(auto &b:(*fileData))
        {
            if(contador<numPixels){
            //esconder en un pixel
                hideByte24bpp(pixelData.pixels24[contador] ,b);
                //avanzar
                contador++;
            }
        }

    }


    void mixImage(BMPReader* otherImage)
    {

        //por cada cada pixel de this, sumar pixel de otherImage
        int numPixels=dibHeader->height*dibHeader->width;
        for(int i=0;i<numPixels;i++)
        {
            this->pixelData.pixels24[i].r+=otherImage->pixelData.pixels24[i].r;
            this->pixelData.pixels24[i].g+=otherImage->pixelData.pixels24[i].g;
            this->pixelData.pixels24[i].b+=otherImage->pixelData.pixels24[i].b;
        }

    }

    void toGrayScale(){

        int average=0;
        int numPixels=dibHeader->height*dibHeader->width;
        for(int i=0;i<numPixels;i++)
        {
            average=pixelData.pixels24[i].r+pixelData.pixels24[i].g+pixelData.pixels24[i].b;
            average/=3;
            if(average>1) average=255;
            else average=0;
            pixelData.pixels24[i].r=pixelData.pixels24[i].g=pixelData.pixels24[i].b=average;
        }
    }

    // Aumentar el tamaño de una imagen.
    void scaleImage(unsigned int scaleFactor) 
    {
        if (scaleFactor != 4 && scaleFactor != 2) {
            std::cout << "ERROR: Solo se permite x2 o x4." << std::endl;
            return;
        }

        if(this->dibHeader->bpp != 24)
        {
            std::cout << "AVISO: solo se permite el escalado de imagenes de 24 bits. La convertiremos \n ";
            this->convertImageTo24bpp();
        }

        // Calculamos las dimensiones de la nueva imagen.
        int oldW = dibHeader->width;
        int oldH = dibHeader->height;
        int newW = oldW * scaleFactor;
        int newH = oldH * scaleFactor;

        // Reservamos en memoria el espacio para la nueva imagen.
        pixel24b* newPixels = new pixel24b[newW * newH];

        // 4. Algoritmo de escalado: Recorremos la imagen NUEVA
        for (int y = 0; y < newH; y++) {
            for (int x = 0; x < newW; x++) {
                // Mapeamos el píxel nuevo al original (división entera)
                int originalX = x / scaleFactor;
                int originalY = y / scaleFactor;
                
                // Copiamos el color del original a la posición nueva
                newPixels[y * newW + x] = pixelData.pixels24[originalY * oldW + originalX];
            }
        }

        // Actualizamos la cabecera.
        delete[] pixelData.pixels24;          // Liberar imagen vieja
        pixelData.pixels24 = newPixels;       // Apuntar a la nueva
        dibHeader->width = newW;              // Actualizar cabecera
        dibHeader->height = newH;
        
        // El tamaño de la imagen en bytes también cambia (para 32bpp)
        dibHeader->rawDataSizeInBytes = newW * newH * 4; 
    }
};











































