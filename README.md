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
