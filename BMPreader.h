#pragma once
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <string>
#include <string.h>

// Archivos auxiliares.
#include "DIBHeaders.h"
#include "Pixels.h"

using namespace std;

class BMPReader{

public:
    
// --- CABECERA BMP (PRIMERA) --- //
typedef struct __attribute__ ((packed))
{
    unsigned char format[2];    // Almacena el tipo de cabecera. En nuestro caso es "B", "M".
    unsigned int fileSize;      // Tamaño TOTAL del archivo.
    unsigned char optional[4];  // Reservados opcionales (no los usamos).
    unsigned int offset;        // Offset del fichero. Donde empiezan los datos.
}
BMP_Header;


BMP_Header* bmpHeader;      //Primera cabecera BMP
DIB40_Header* dibHeader;    //Segunda cabecera BMP
vector<char>* rawData;      //Datos en bruto leidos.
pixel_t pixelData;          //Array de pixeles de imagen


// Lee el fichero indicado y devuelve sus bytes en un vector.
vector<char>* readFile(string file){
    ifstream fIn(file, ifstream::binary);
    vector<char>* bytes = new vector<char> (istreambuf_iterator<char>(fIn), {});
    fIn.close();
    
    return bytes;
}

// Constructor: abre el fichero, parsea las cabeceras y apunta al inicio de los datos de imagen.
BMPReader(string fileName){
    //Abrir fichero y leer sus datos
    auto rawData = readFile(fileName);
    //Parsear cabecera BMP
    bmpHeader = (BMP_Header*)rawData->data();
    //Parsear cabecera DIB, desplazándose 14 bytes desde el inicio
    dibHeader = (DIB40_Header*)(rawData->data() + sizeof(BMP_Header));

    //Apuntar al inicio de los datos de imagen usando el offset de la cabecera BMP
    pixelData.type=dibHeader->bpp;
    pixelData.rawPixels=(char*)(rawData->data()+bmpHeader->offset);
}

// Escribe la imagen actual en un nuevo fichero BMP.
void writeBMP(string newFileName){
    ofstream fout(newFileName, ofstream::binary); //Crear fichero en modo escritura binaria
    fout.write((char*)bmpHeader, sizeof(BMP_Reader));   //Escribir cabecera BMP
    fout.write((char*)dibHeader, dibHeader->headerSize); //Escribir cabecera DIB
    
    //Calcular tamaño en bytes del área de imagen (bytes por pixel * ancho * alto)
    size_t imageSize = (dibHeader->bpp/8)*dibHeader->width*dibHeader->heigth;
    
    fout.write((char*)pixelData.rawPixels, imageSize);  //Escribir datos de imagen
    
    fout.close(); //Cerrar y salvar los datos
}

// Filtro de suavizado bilineal: todos los pesos iguales a 1/9 (media de vecinos).
static inline const float bilinearFilter[3][3]={
{ 1.0f/9.0f, 1.0f/9.0f, 1.0f/9.0f},
{ 1.0f/9.0f, 1.0f/9.0f, 1.0f/9.0f},
{ 1.0f/9.0f, 1.0f/9.0f, 1.0f/9.0f}};

// Filtro de detección de bordes verticales (kernel de Prewitt vertical).
static inline const float edgeVerticalDetector[3][3] = {
    {1,0,-1}, 
    {1,0,-1},
    {1,0,-1}};

// Filtro de detección de bordes horizontales (kernel de Prewitt horizontal).
static inline const float edgeHorizontalDetector[3][3] = {
    {1,1,1}, 
    {0,0,0},
    {-1,-1,-1}};

// Aplica una convolución 3x3 sobre la imagen con el filtro indicado.
// Los bordes se tratan con wrap-around (la imagen se considera periódica).
void applyFilter3x3(const float filter[3][3])
{
    //Reservar buffer temporal para los nuevos pixeles calculados
    pixel24b* newPixels = new pixel24b[dibHeader->witdh * dibHeader->heigth];
    //Recorrer cada fila de la imagen
    for(int y=0; y < dibHeader->height; y++){
    //Recorrer cada columna de la imagen
        for(int x=0; x < dibHeader->witdh; x++){
            float r=0,g=0,b =0;
            //Acumular la contribución de cada vecino en la ventana 3x3
            for(int i= -1; i<= 1;i++){ //Vecinos superiores e inferiores
                for(int j= -1; j<= 1;j++){ //Vecinos izquierdos y derechos
                    //Calcular coordenadas con wrap-around para evitar salirse de los límites
                    int accessX=((x+j) + dibHeader->witdh) % dibHeader->witdh;
                    int accessY=((y+i) +dibHeader->heigth) % dibHeader->heigth;
                    //Acumular cada canal multiplicado por el peso del filtro
                    r+=(pixelData.pixels24[(accessY)*dibHeader->witdh + (accessX)].r)*filter[i+1][j+1];
                    g+=(pixelData.pixels24[(accessY)*dibHeader->witdh + (accessX)].g)*filter[i+1][j+1];
                    b+=(pixelData.pixels24[(accessY)*dibHeader->witdh + (accessX)].b)*filter[i+1][j+1];
                }      
            }
            //Guardar el pixel resultante en el buffer temporal
            newPixels[(y)*dibHeader->witdh + (x)] = {(unsigned char)r,(unsigned char)g,(unsigned char)b};
        }

        //Copiar el buffer temporal sobre los datos originales de la imagen
        size_t pixelSize = pixelData.type / 8;
        memcpy(pixelData.rawPixels, newPixels, pixelSize*dibHeader->witdh * dibHeader->heigth);
    }
}

// Convierte un pixel de 24 bits (R8 G8 B8) a formato de 16 bits (R5 G6 B5).
pixel16b to16bpp(pixel24b pixel){
    pixel16b res = {0};

    //Reducir cada canal al número de bits que le corresponde en el formato 16bpp
    unsigned short r5 = (pixel.r >> 3) & 0x00FF; //5 bits para rojo
    unsigned short g6 = (pixel.g >> 2) & 0x00FF; //6 bits para verde
    unsigned short b5 = (pixel.b >> 3) & 0x00FF; //5 bits para azul

    //Empaquetar los tres canales en un único valor de 16 bits: RRRRRGGGGGGBBBBB
    res.rawColor16= (r5 < 11) | (g6 < 5) | b5;
    return res;
}

// Convierte todos los pixeles de la imagen de 24 bits a 16 bits y actualiza las cabeceras.
void convertImageTo16bpp(){
    //Reservar buffer para los nuevos pixeles de 16 bits
    pixel16b* newPixels = new pixel16b[dibHeader->witdh * dibHeader->heigth];
        //Recorrer cada fila de la imagen
        for(int y=0; y < dibHeader->height; y++){
        //Recorrer cada columna de la imagen
            for(int x=0; x < dibHeader->witdh; x++){
                //Convertir el pixel 24bpp a 16bpp y almacenarlo en el buffer
                newPixels[y*dibHeader->width+x]= to16bpp(pixelData.pixels24[y*dibHeader->width+x]);
            }
        }
        //Copiar los nuevos pixeles sobre los datos originales
        size_t pixelSize = pixelData.type / 8;
        memcpy(pixelData.rawPixels, newPixels, pixelSize*dibHeader->witdh * dibHeader->heigth);
        //Actualizar el tamaño del fichero en la cabecera BMP
        bmpHeader->fileSize=dibHeader->headerSize + sizeof(bmpHeader) + pixelSize*dibHeader->witdh * dibHeader->heigth;
        //Actualizar los bits por pixel en la cabecera DIB y en la estructura interna
        dibHeader->bpp = 16;
        pixelData.type=16;
    }

// Esconde un byte en los bits menos significativos de los canales R, G y B de un pixel.
// Distribución: 3 bits en rojo, 2 bits en verde, 3 bits en azul.
void hideByte24bpp(pixel24b& pixel,unsigned char byte){
    //Guardar los 3 bits menos significativos del byte en el canal rojo
    unsigned char redMask = 0b11111000; //Mantener los 5 bits más importantes del rojo
    pixel.r = pixel.r & redMask; //Borrar los 3 bits menos importantes del rojo
    unsigned char data3bitDerecha = 0b00000111; //Máscara para los 3 bits de la derecha
    pixel.r= pixel.r | (byte & data3bitDerecha); //Insertar los 3 bits bajos del byte en el rojo

    //Guardar los bits 3 y 4 del byte en el canal verde
    unsigned char greenMask = 0b00000011; //Mantener los 6 bits más importantes del verde
    pixel.g = pixel.g & greenMask; //Borrar los 2 bits menos importantes del verde
    byte = byte >> 3; //Desplazar el byte para acceder a los bits 3 y 4
    unsigned char data2bitDerecha = 0b00000011; //Máscara para los 2 bits de la derecha
    pixel.g= pixel.g | (byte & data2bitDerecha); //Insertar 2 bits en el canal verde

    //Guardar los bits 5, 6 y 7 del byte en el canal azul
    unsigned char blueMask = 0b00000111; //Mantener los 5 bits más importantes del azul
    pixel.b = pixel.b & blueMask; //Borrar los 3 bits menos importantes del azul
    byte = byte >> 2; //Desplazar el byte para acceder a los bits 5, 6 y 7
    pixel.b= pixel.b | (byte & data3bitDerecha); //Insertar 3 bits en el canal azul
}

// Esconde el contenido de un fichero en los bits menos significativos de la imagen (esteganografía).
// Requiere que la imagen sea de 24 bits por pixel.
void hideFile(string fileName){
    //Solo se soportan imágenes de 24 bits
    if(dibHeader->bpp != 24)
        cout << "ERROR: imagen no soportada\n";
    
    //Leer los bytes del fichero a ocultar
    auto fileData = readFile(fileName);

    int numPixels = dibHeader->width * dibHeader->weight;
    int contador=0;
    //Esconder cada byte del fichero en un pixel de la imagen
    for (auto &b:(*fileData)){
        //Comprobar que no se supera el número de pixeles disponibles
        if (numPixels > contador){
            hideByte24bpp(pixelData.pixels24[contador], b);
            contador++;
        }
    }
}

// Suma canal a canal los pixeles de otra imagen sobre la imagen actual.
// Ambas imágenes deben tener las mismas dimensiones.
void mixImage(BMPReader* otherImage){
    int numPixels = dibHeader->width * dibHeader->weight;
    
    //Acumular cada canal de color pixel a pixel
    for(int i =0; i < numPixels; i++)
    {
        this->pixelData.pixels24[i].r += otherImage->pixelData.pixels24[i].r;
        this->pixelData.pixels24[i].g += otherImage->pixelData.pixels24[i].g;
        this->pixelData.pixels24[i].b += otherImage->pixelData.pixels24[i].b;
    }
}

// Convierte la imagen a escala de grises binarizada: cada pixel pasa a blanco (255) o negro (0)
// según si la media de sus canales supera 1.
void toGrayScale(){
    int average = 0;
    int numPixels = dibHeader->width * dibHeader->weight;
    
    for(int i =0; i < numPixels; i++){
        //Calcular la media de los tres canales de color
        average = this->pixelData.pixels24[i].r +  this->pixelData.pixels24[i].g +this->pixelData.pixels24[i].b;
        average/=3;
        //Binarizar: blanco si la media supera 1, negro en caso contrario
        if(average>1) average = 255;
        else average = 0;
        //Asignar el mismo valor a los tres canales para obtener el tono gris
        this->pixelData.pixels24[i].r=this->pixelData.pixels24[i].g =this->pixelData.pixels24[i].b=average;
    }
}

};