# Taller 2 - Procesamiento de Imagenes (OpenCV C++)

Proyecto con un menu en consola para ejecutar los ejercicios 1 a 7 desde un solo ejecutable.

## Requisitos

- CMake >= 3.10
- Compilador C++ (g++ recomendado)
- OpenCV instalado en el sistema

En Ubuntu/Debian:

```bash
sudo apt update
sudo apt install -y g++ cmake libopencv-dev
```

## Compilacion

Desde la raiz de `Taller2`:

```bash
cmake -S . -B build
cmake --build build
```

Ejecutable generado:

- `build/taller`

## Uso del programa

Ejecuta:

```bash
./build/taller
```

Flujo:

1. El menu te pide opcion (`1` a `7`, o `0` para salir).
2. El programa pide la ruta de imagen.
3. Si dejas Enter vacio, usa `imagen.jpg`.
4. Se ejecuta el ejercicio seleccionado.

## Ejercicios implementados

## Ejercicio 1 (resuelto)

Implementado en `main.cpp` dentro de `ejercicio1(...)` con conversion totalmente manual por pixel:

- `BGR -> HSV`:
  - Normalizacion de canales a `[0,1]`.
  - Calculo de `Cmax`, `Cmin`, `Delta`.
  - Calculo de `H`, `S`, `V` con las formulas del enunciado.
  - Ajuste a formato OpenCV: `H/2`, `S*255`, `V*255`.
- `HSV -> BGR`:
  - Recuperacion de `H`, `S`, `V` en rango real.
  - Calculo de `C`, `X`, `m`.
  - Seleccion de sector de `H` para `(r', g', b')`.
  - Conversion final a `[0,255]`.

Notas:

- No se usa `cvtColor()`.
- Se muestra:
  - imagen original,
  - imagen HSV (valores de canales),
  - imagen BGR reconstruida desde HSV manual.

## Ejercicio 2 (resuelto)

Implementado en `main.cpp` dentro de `ejercicio2(...)`:

- Convierte `BGR -> HSV` con la funcion manual del ejercicio 1.
- Multiplica `S` por `1.5` y limita a `[0,255]`.
- Convierte `HSV -> BGR` de forma manual.

No se usa `cvtColor()`.

## Ejercicio 3 (resuelto)

Implementado en `main.cpp` dentro de `ejercicio3(...)`:

- Redimensiona imagen para acelerar.
- Crea vector de pixeles RGB.
- Inicializa centroides y ejecuta iteraciones de K-Means:
  - asignacion por distancia euclidiana,
  - recalculo de centroides por promedio.
- Construye imagen cuantizada y paleta de colores.

No se usa `kmeans()`.

## Ejercicio 4 (resuelto)

Implementado en `main.cpp` dentro de `ejercicio4(...)`:

- Calcula promedio por canal.
- Calcula promedio gris.
- Obtiene factores de escala por canal.
- Corrige cada pixel con saturacion a `255`.

## Ejercicio 5 (resuelto)

Implementado en `main.cpp` dentro de `ejercicio5(...)`:

- Suavizado gaussiano manual con kernel `5x5`:

```text
1  4  6  4 1
4 16 24 16 4
6 24 36 24 6
4 16 24 16 4
1  4  6  4 1
```

- Normalizacion por suma `256`.
- Manejo de bordes por clamping de indices.

No se usa `GaussianBlur()`.

## Ejercicio 6 (resuelto)

Implementado en `main.cpp` dentro de `ejercicio6(...)`:

- Crea tabla de 256 valores con:
  - `pixel_out = 255 * (pixel_in / 255)^gamma`
- Aplica la tabla manualmente en bucle a cada canal de cada pixel.

No se usa `LUT()`.

## Ejercicio 7 (resuelto)

Implementado en `main.cpp` dentro de `ejercicio7(...)`:

- Calcula centro de imagen y distancia maxima.
- Para cada pixel aplica:
  - `d = distancia al centro`,
  - `d_norm = d / d_max`,
  - `f = 1 / (1 - k * d_norm^2)`,
  - `pixel_corregido = min(pixel_original * f, 255)`.

## Pruebas realizadas

```bash
cmake --build build
```

Inicio de menu validado:

```bash
./build/taller
```
