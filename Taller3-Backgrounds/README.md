# Taller 3 - Background Subtraction Manual con OpenCV (C++)

# JUAN PABLO JOYA PENAGOS

Este taller incluye dos implementaciones manuales para deteccion de movimiento con camara:

- `frame`: frame differencing manual (frame actual vs frame anterior).
- `running_average_manual`: running average manual (fondo adaptativo con promedio exponencial).
- `main`: ejecuta ambos metodos en una sola aplicacion.

## Estructura

```text
Taller3-Backgrounds/
├── CMakeLists.txt
├── main.cpp
├── include/
│   └── background_models.hpp
├── src/
│   ├── background_models.cpp
│   ├── frame.cpp
│   └── running_average_manual.cpp
└── build/
```

## Requisitos

- C++17
- OpenCV 4
- CMake
- `g++`

En Ubuntu/Debian (referencia):

```bash
sudo apt update
sudo apt install -y g++ cmake libopencv-dev
```

## Compilacion

Desde `Taller3-Backgrounds/`:

```bash
cmake -S . -B build
cmake --build build
```

## Ejecucion

```bash
./build/frame
./build/running_average_manual
./build/main
```

## Formulas

Frame differencing:

- `d(x,y) = |I_t(x,y) - I_{t-1}(x,y)|`
- `M(x,y) = 255` si `d(x,y) >= T`, en otro caso `0`

Running average:

- `B_t(x,y) = (1 - alpha) * B_{t-1}(x,y) + alpha * I_t(x,y)`
- `d(x,y) = |I_t(x,y) - B_t(x,y)|`
- `M(x,y) = 255` si `d(x,y) >= T`, en otro caso `0`

## Controles

En ambos ejecutables:

- `+` o `=`: subir umbral
- `-` o `_`: bajar umbral
- `q` o `Esc`: salir

Solo en `running_average_manual`:

- `]`: subir `alpha`
- `[`: bajar `alpha`

Solo en `main`:

- Abre 4 ventanas en cuadricula centrada:
  - `Frame - Diferencia absoluta manual`
  - `Frame - Mascara de movimiento manual`
  - `Running Avg - Diferencia absoluta manual`
  - `Running Avg - Mascara de movimiento manual`
- Si tu resolucion no es 1920x1080, puedes ajustar la centrado con:
  - `SCREEN_WIDTH=<ancho> SCREEN_HEIGHT=<alto> ./build/main`
