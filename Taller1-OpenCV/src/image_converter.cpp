#include <cmath>
#include <filesystem>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

namespace fs = std::filesystem;

// Ajusta un valor decimal al rango valido de un canal de imagen (0-255).
static uchar clamp_to_u8(double value) {
    if (value < 0.0) {
        return 0;
    }
    if (value > 255.0) {
        return 255;
    }
    return static_cast<uchar>(std::round(value));
}

// Carga una imagen BGR desde disco y valida que se haya leído correctamente.
cv::Mat cargar_imagen(const std::string& ruta_imagen) {
    cv::Mat imagen = cv::imread(ruta_imagen, cv::IMREAD_COLOR);
    if (imagen.empty()) {
        std::cerr << "Error al cargar la imagen." << std::endl;
    }
    return imagen;
}

// Convierte una imagen BGR a escala de grises usando la formula luminancia.
cv::Mat convertir_a_grises(const cv::Mat& imagen) {
    cv::Mat imagen_grises(imagen.rows, imagen.cols, CV_8UC1);

    for (int y = 0; y < imagen.rows; ++y) {
        for (int x = 0; x < imagen.cols; ++x) {
            const cv::Vec3b bgr = imagen.at<cv::Vec3b>(y, x);
            const double r = static_cast<double>(bgr[2]);
            const double g = static_cast<double>(bgr[1]);
            const double b = static_cast<double>(bgr[0]);
            const double gris = 0.299 * r + 0.587 * g + 0.114 * b;
            imagen_grises.at<uchar>(y, x) = clamp_to_u8(gris);
        }
    }

    return imagen_grises;
}

// Convierte una imagen BGR a YUV aplicando las formulas canal por canal.
cv::Mat convertir_a_yuv(const cv::Mat& imagen) {
    cv::Mat imagen_yuv(imagen.rows, imagen.cols, CV_8UC3);

    for (int y = 0; y < imagen.rows; ++y) {
        for (int x = 0; x < imagen.cols; ++x) {
            const cv::Vec3b bgr = imagen.at<cv::Vec3b>(y, x);
            const double r = static_cast<double>(bgr[2]);
            const double g = static_cast<double>(bgr[1]);
            const double b = static_cast<double>(bgr[0]);

            const double yy = 0.299 * r + 0.587 * g + 0.114 * b;
            const double uu = -0.14713 * r - 0.28886 * g + 0.436 * b + 128.0;
            const double vv = 0.615 * r - 0.51499 * g - 0.10001 * b + 128.0;

            imagen_yuv.at<cv::Vec3b>(y, x) = cv::Vec3b(
                clamp_to_u8(yy),
                clamp_to_u8(uu),
                clamp_to_u8(vv)
            );
        }
    }

    return imagen_yuv;
}

// Convierte una imagen BGR a HSV con calculo manual de H, S y V.
cv::Mat convertir_a_hsv(const cv::Mat& imagen) {
    cv::Mat imagen_hsv(imagen.rows, imagen.cols, CV_8UC3);

    for (int y = 0; y < imagen.rows; ++y) {
        for (int x = 0; x < imagen.cols; ++x) {
            const cv::Vec3b bgr = imagen.at<cv::Vec3b>(y, x);
            const double r = static_cast<double>(bgr[2]) / 255.0;
            const double g = static_cast<double>(bgr[1]) / 255.0;
            const double b = static_cast<double>(bgr[0]) / 255.0;

            const double max_rgb = std::max({r, g, b});
            const double min_rgb = std::min({r, g, b});
            const double delta = max_rgb - min_rgb;

            double h = 0.0;
            if (delta > 1e-10) {
                if (max_rgb == r) {
                    h = 60.0 * std::fmod(((g - b) / delta), 6.0);
                } else if (max_rgb == g) {
                    h = 60.0 * (((b - r) / delta) + 2.0);
                } else {
                    h = 60.0 * (((r - g) / delta) + 4.0);
                }
            }

            if (h < 0.0) {
                h += 360.0;
            }

            const double s = (max_rgb <= 1e-10) ? 0.0 : (delta / max_rgb);
            const double v = max_rgb;

            const uchar hh = clamp_to_u8((h / 360.0) * 255.0);
            const uchar ss = clamp_to_u8(s * 255.0);
            const uchar vv = clamp_to_u8(v * 255.0);

            imagen_hsv.at<cv::Vec3b>(y, x) = cv::Vec3b(hh, ss, vv);
        }
    }

    return imagen_hsv;
}

// Guarda una imagen en disco e informa si la escritura falla.
void guardar_imagen(const cv::Mat& imagen, const fs::path& ruta_archivo) {
    if (!cv::imwrite(ruta_archivo.string(), imagen)) {
        std::cerr << "No se pudo guardar: " << ruta_archivo << std::endl;
    }
}

// Muestra las imagenes convertidas en ventanas separadas.
void mostrar_imagenes(const cv::Mat& grises, const cv::Mat& yuv, const cv::Mat& hsv) {
    cv::imshow("Escala de Grises", grises);
    cv::imshow("YUV", yuv);
    cv::imshow("HSV", hsv);
    cv::waitKey(0);
    cv::destroyAllWindows();
}

// Punto de entrada: recibe ruta de imagen, convierte, guarda y muestra resultados.
int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Por favor, proporciona la ruta de la imagen como parametro." << std::endl;
        return 1;
    }

    const fs::path ruta_imagen(argv[1]);
    const cv::Mat imagen = cargar_imagen(ruta_imagen.string());
    if (imagen.empty()) {
        return 1;
    }

    const std::string nombre_imagen = ruta_imagen.stem().string();
    const fs::path carpeta_destino = ruta_imagen.parent_path() / (nombre_imagen + "_convertidas");
    fs::create_directories(carpeta_destino);

    const cv::Mat imagen_grises = convertir_a_grises(imagen);
    const cv::Mat imagen_yuv = convertir_a_yuv(imagen);
    const cv::Mat imagen_hsv = convertir_a_hsv(imagen);

    guardar_imagen(imagen_grises, carpeta_destino / (nombre_imagen + "_grises.jpg"));
    guardar_imagen(imagen_yuv, carpeta_destino / (nombre_imagen + "_yuv.jpg"));
    guardar_imagen(imagen_hsv, carpeta_destino / (nombre_imagen + "_hsv.jpg"));

    mostrar_imagenes(imagen_grises, imagen_yuv, imagen_hsv);
    return 0;
}
