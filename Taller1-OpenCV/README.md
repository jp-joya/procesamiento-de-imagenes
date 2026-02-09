# OpenCV en C++

Migracion del proyecto a C++ usando OpenCV.

## Autor

- JUAN PABLO JOYA

## Archivos principales

- `src/test_camera.cpp`: prueba de camara en tiempo real.
- `src/image_converter.cpp`: convierte una imagen a Grises, YUV y HSV, guarda resultados y los muestra.
- `CMakeLists.txt`: compilacion de ambos ejecutables.

## Requisitos

- Compilador C++ con soporte C++17 (ej. `g++`)
- CMake (3.12 o superior)
- OpenCV para C++ instalado en el sistema

En Ubuntu/Debian, por ejemplo:

```bash
sudo apt update
sudo apt install -y g++ cmake libopencv-dev
```

## Compilacion

Desde la raiz del proyecto:

```bash
cmake -S . -B build
cmake --build build
```

Esto genera:

- `build/test_camera`
- `build/image_converter`

## Uso de `test_camera`

Ejecuta:

```bash
./build/test_camera
```

Comportamiento:

- Abre la camara por defecto (`0`).
- Muestra una ventana `Camera`.
- Presiona `q` para salir.

## Uso de `image_converter`

Ejecuta:

```bash
./build/image_converter <ruta_imagen>
```

Ejemplo:

```bash
./build/image_converter gato.jpeg
```

Comportamiento:

- Crea una carpeta `<nombre>_convertidas`.
- Guarda:
  - `<nombre>_grises.jpg`
  - `<nombre>_yuv.jpg`
  - `<nombre>_hsv.jpg`
- Muestra tres ventanas con las conversiones (Grises, YUV, HSV).
