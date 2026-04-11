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

    vector<char>* rawData;//datos en burto leídos de fichero
    BMP_Header* bmpHeader; //primera cabecera BMP
    DIB40_Header* dibHeader; //segunda cabecera BMP
    pixel_t pixelData; //array de pixeles de imagen

    vector<char>* readFile(string file){
        ifstream fIn( file, ifstream::binary );
        vector<char>* bytes=new vector<char>(istreambuf_iterator<char>(fIn), {});
        fIn.close();
        return bytes;
    }

    BMPReader(string fileName){
        //abrir fichero, leer datos
        auto rawData=readFile(fileName);
        //parsear cabecera
        bmpHeader=(BMP_Header*)rawData->data();
        dibHeader=(DIB40_Header*)(rawData->data()+sizeof(BMP_Header));

       //apuntar datos de imagen
        pixelData.type=dibHeader->bpp;
        pixelData.rawPixels=( char*)(rawData->data()+bmpHeader->offset);

    }


    void writeBMP(string newFileName)
    {

        ofstream fout( newFileName, ofstream::binary ); //crear fichero en modo escritura

        fout.write((char*)bmpHeader,sizeof(BMP_Header));
        fout.write((char*)dibHeader,dibHeader->headerSize); //

        size_t imageSize=(dibHeader->bpp/8)*dibHeader->width*dibHeader->height;

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


    pixel16b to16Bpp(pixel24b pixel)
    {

        pixel16b res={0};
        unsigned short r5=(pixel.r>>3)&0x00FF;
        unsigned short g6=(pixel.g>>2)&0x00FF;
        unsigned short b5=(pixel.b>>3)&0x00FF;

        res.rawColor16=(r5<<11) | (g6<<5) | b5;

        return res;

    }

    void convertImageTo16bpp(){
        //por cada pixel
        //pasar a 16bpp
        pixel16b* newPixels=new pixel16b[dibHeader->width*dibHeader->height];

        for(int y=0;y<dibHeader->height;y++)
            for(int x=0;x<dibHeader->width;x++) //por cada columna
            {
                newPixels[y*dibHeader->width+x]=to16Bpp(pixelData.pixels24[y*dibHeader->width+x]);
            }

        //copiar resultados
        size_t pixelSize=pixelData.type/8;
        memcpy(pixelData.rawPixels,newPixels, pixelSize*dibHeader->width*dibHeader->height);
        bmpHeader->fileSize=dibHeader->headerSize+sizeof(bmpHeader)+pixelSize*dibHeader->width*dibHeader->height;
        dibHeader->bpp=16;
        pixelData.type=16;
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


};











































