# --- ENGLISH ---

# Practice 2: Binary Files and Information Hiding

This practice implements a C++ library for reading, converting and saving BMP images (`BITMAPV5HEADER`, 16/24/32 bit), extending the `BMPReader` library provided in class.
It also hides arbitrary data inside the pixels of an image (steganography) and recovers it later, verifying the result with `diff`.

---

In this exercise you are asked to put into practice the knowledge acquired about programming with binary files based on the BMP format. Not all parts are mandatory, but it is recommended to complete most of them as preparation. The grade for this exercise will be decided in a separate exam after the submission date; you are only asked to submit your proposed solution in order to be eligible to take the exam.

---

## Exercise

You are asked to implement a library for manipulating BMP image files that can perform the following operations:

- Two images in `BITMAPV5HEADER` format are provided, for which the student must find out how to implement the header.
  - The first file is in 16-bit color format, and the second one in 32-bit color format.
- The code used in class is also provided as support for the practice.

---

## Part 1: File Formats

Create a modification of the supplied `BMPReader` library that supports the following new functionalities:

### Loading and saving
- Loading of `BITMAPV5HEADER` files:
  - Loading of 16-bit data
  - Loading of 32-bit data
- Saving data to 16 or 32-bit color formats, depending on the type of file/header loaded

### Color format conversion
Create functions that, regardless of the original format of the file, can transform the pixels from one color format to another:

```cpp
void convertImageTo16bpp();
void convertImageTo24bpp();
void convertImageTo32bpp();
```

### Main program
- Create a `main` program that reads the data from the supplied files (`pagoda16bit.bmp` and `pagoda32bit.bmp`) into two variables of type `bmpReader`.
- Save the data of both images into two "copy" files. To demonstrate that loading and saving have not altered the content, check that the copies are identical to the originals using the `diff` command on Linux.
- Convert the file `pagoda32bit.bmp` to 24-bit and 16-bit color formats, and save it to a new file for each format change. Check that the image has not been corrupted and verify with a hex editor that the headers are indeed `BITMAPV5HEADER` of the selected color format.

---

## Part 2: Information Hiding in Images

Based on the code seen in class, create the following functions:

### `hideRawData`

```cpp
void hideRawData(std::vector<unsigned char>& data);
```

- Receives as a parameter a data vector that will be mixed with the image data in order to hide that information.
- The image can be in any of the color formats covered: 16, 24 or 32 bit.
- The "high" color values must be preserved to avoid too much loss of information in the image.
- Since this information must be recovered later, it is advisable to store somewhere the metadata of the byte vector, such as its size.

### `hideRawData` — Optional part

Since the data vector may be larger than the number of pixels in the image, the image can be "enlarged":

- Calculate a new image width/height with enough pixels to store the information in the data vector.
- Resize the image to the new size:
  - Make height and width sizes that are multiples of the original size.
  - For each pixel of the original image, repeat it horizontally and vertically to stretch the image without losing information.
- Since the 32-bit format is the one that can hide the most information, it would be advisable to convert the image to this format.
- Update the headers according to the new image size.

### `recoverHiddenRawData`

```cpp
std::vector<unsigned char> recoverHiddenRawData();
```

- Performs the opposite process of `hideRawData`.
- Given a structure with a loaded image, it recovers the hidden information and returns it in a byte vector.
- It must be able to work with the same image data types supported by `hideRawData`.

### Main program

The program will receive through `argc/argv` the names of two files and the usage mode:

- **First file:** BMP image file that will be used as the base to hide information.
- **Second file:** name of the file that contains the data to hide. It can be an image, an executable, a text document...
- **Usage mode:** can be `1-hide` or `2-recover`.

Depending on the usage mode:

**`hide` mode:**
1. Load the image file into a `bmpReader` structure and the content of the second file into a vector of type `unsigned char`.
2. Call `hideRawData` to hide the data of the second file.
3. Save the image to a new BMP file with the prefix `hiddenData` in its name.

**`recover` mode:**
1. Load the image file into a `bmpReader` structure, which must contain hidden data.
2. Call `recoverHiddenRawData`, which will return the data of the original file stored along with the image.
3. Save the recovered data to the file name passed as the second file.

You must ensure that the recovered data is correct by using the `diff` command to check that the recovered file is identical to the original.

---

## Submission

Submit the developed code on Blackboard before the indicated date. The submitted code files must include the student's name in a comment within the code. The submitted file must follow this naming convention:

```
PBN_PR2_NAME_SURNAME.zip
```

> Any submitted file that does not comply with these submission rules may be ignored.

# --- SPANISH ---

# Práctica 2: Archivos binarios y ocultación de información

En este ejercicio se pide poner en práctica el conocimiento adquirido sobre programación con archivos binarios basados en formato BMP. No todas las partes son obligatorias, pero se recomienda realizar la mayor parte de ellas para prepararse. La nota de este ejercicio se decidirá en un examen aparte después de la fecha de entrega, sólo se pide entregar la solución propuesta por el alumno para poder optar a realizar el examen.

---

## Ejercicio

Se pide implementar una librería de manipulación de archivos de imagen BMP que podrá realizar las siguientes operaciones:

- Se entregan dos imágenes en formato `BITMAPV5HEADER`, el cual el alumno debe buscar cómo implementar su cabecera.
  - El primer archivo está en formato 16 bit de color, y el segundo en formato 32 bit de color.
- También se entrega el código usado en clase para apoyo de la práctica.

---

## Parte 1: Formatos de fichero

Crear una modificación de la librería `BMPReader` suministrada, la cual soporte las siguientes funcionalidades nuevas:

### Carga y guardado
- Carga de ficheros `BITMAPV5HEADER`:
  - Carga de datos de 16 bit
  - Carga de datos de 32 bit
- Guardado de datos a formatos de color 16 o 32 bit, en función del tipo de fichero/cabecera cargada

### Conversión de formato de color
Crear funciones que, independientemente del formato original del archivo, puedan transformar los píxeles de un formato de color a otro:

```cpp
void convertImageTo16bpp();
void convertImageTo24bpp();
void convertImageTo32bpp();
```

### Programa main
- Crear un programa `main` que lea los datos de los ficheros suministrados (`pagoda16bit.bmp` y `pagoda32bit.bmp`) en dos variables de tipo `bmpReader`.
- Guardar los datos de sendas imágenes en dos archivos "copia". Para demostrar que la carga y guardado no han alterado el contenido, comprobar que las copias son iguales a los originales usando el comando `diff` en Linux.
- Pasar el archivo `pagoda32bit.bmp` a formato de color 24 bit y 16 bit, y guardarla a un nuevo fichero con cada cambio de formato. Comprobar que la imagen no se ha corrompido y verificar con un editor hexadecimal que efectivamente son cabeceras `BITMAPV5HEADER` del formato de color seleccionado.

---

## Parte 2: Ocultación de información en imágenes

En base al código visto en clase, crear las siguientes funciones:

### `hideRawData`

```cpp
void hideRawData(std::vector<unsigned char>& data);
```

- Recibe por parámetros un vector de datos que mezclará con los datos de la imagen para ocultar esa información.
- La imagen puede estar en cualquiera de los formatos de color vistos: 16, 24 ó 32 bit.
- Se deben mantener los valores "altos" de color para evitar demasiada pérdida de información en la imagen.
- Dado que se debe recuperar más adelante esta información, es conveniente guardar en algún sitio los metadatos del vector de bytes, como el tamaño del mismo.

### `hideRawData` — Parte opcional

Dado que el vector de datos puede ser mayor que el número de píxeles de la imagen, se puede "ampliar" la imagen:

- Calcular un nuevo ancho/alto de imagen que tenga suficientes píxeles para guardar la información del vector de datos.
- Redimensionar la imagen al nuevo tamaño:
  - Hacer tamaños de alto y ancho que sean múltiplos del tamaño original.
  - Por cada píxel de la imagen original, repetirlo en horizontal y vertical para estirar la imagen sin perder información.
- Dado que el formato de 32 bit es el que más información puede ocultar, sería conveniente pasar la imagen a este formato.
- Actualizar las cabeceras en función del nuevo tamaño de la imagen.

### `recoverHiddenRawData`

```cpp
std::vector<unsigned char> recoverHiddenRawData();
```

- Realiza el proceso contrario a `hideRawData`.
- Dada una estructura con una imagen cargada, recupera la información oculta y la devuelve en un vector de bytes.
- Debe ser capaz de operar con los mismos tipos de datos de imagen que soporte `hideRawData`.

### Programa main

El programa recibirá por parámetros `argc/argv` los nombres de dos archivos y el modo de uso:

- **Primer archivo:** archivo de imagen BMP que se usará como base para ocultar información.
- **Segundo archivo:** nombre de archivo que contendrá los datos a ocultar. Puede ser una imagen, un ejecutable, un documento de texto...
- **Modo de uso:** puede ser `1-hide` o `2-recover`.

En función del modo de uso:

**Modo `hide`:**
1. Cargar el archivo de imagen en una estructura `bmpReader` y el contenido del segundo fichero en un vector de tipo `unsigned char`.
2. Llamar a `hideRawData` para ocultar los datos del segundo archivo.
3. Guardar la imagen en un nuevo archivo BMP con el prefijo `hiddenData` en el nombre.

**Modo `recover`:**
1. Cargar el archivo de imagen en una estructura `bmpReader`, el cual debe contener datos ocultos.
2. Llamar a `recoverHiddenRawData`, que devolverá los datos del fichero original guardado junto a la imagen.
3. Guardar los datos recuperados en el nombre de fichero pasado como segundo archivo.

Se debe asegurar que los datos recuperados son los correctos usando el comando `diff` para comprobar que el fichero recuperado es igual al original.

---

## Entrega

Entregar el código desarrollado en Blackboard antes de la fecha indicada. Los archivos de código entregados deberán tener el nombre del alumno en un comentario dentro del código. El archivo entregado deberá seguir la siguiente nomenclatura:

```
PBN_PR2_NOMBRE_APELLIDO.zip
```

> Cualquier archivo entregado que no se atenga a estas reglas de entrega podrá ser ignorado.
