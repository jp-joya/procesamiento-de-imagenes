#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <limits>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

// ==============================
// Utilidades compartidas
// ==============================
static uchar clampToU8(double value) {
    if (value < 0.0) {
        return 0;
    }
    if (value > 255.0) {
        return 255;
    }
    return static_cast<uchar>(value + 0.5);
}

static int clampInt(int value, int low, int high) {
    if (value < low) {
        return low;
    }
    if (value > high) {
        return high;
    }
    return value;
}

// ==============================
// Conversiones manuales (base de Ejercicio 1 y 2)
// ==============================
static cv::Vec3b bgrAHsvPixelManual(const cv::Vec3b& pixel_bgr) {
    const double b = pixel_bgr[0] / 255.0;
    const double g = pixel_bgr[1] / 255.0;
    const double r = pixel_bgr[2] / 255.0;

    const double cmax = std::max(r, std::max(g, b));
    const double cmin = std::min(r, std::min(g, b));
    const double delta = cmax - cmin;

    double h = 0.0;
    if (delta > 1e-12) {
        if (cmax == r) {
            h = 60.0 * std::fmod((g - b) / delta, 6.0);
        } else if (cmax == g) {
            h = 60.0 * (((b - r) / delta) + 2.0);
        } else {
            h = 60.0 * (((r - g) / delta) + 4.0);
        }
    }
    if (h < 0.0) {
        h += 360.0;
    }

    const double s = (cmax <= 1e-12) ? 0.0 : (delta / cmax);
    const double v = cmax;

    int h_opencv = static_cast<int>(h / 2.0 + 0.5);
    h_opencv = clampInt(h_opencv, 0, 179);

    return cv::Vec3b(
        static_cast<uchar>(h_opencv),
        clampToU8(s * 255.0),
        clampToU8(v * 255.0)
    );
}

static cv::Vec3b hsvABgrPixelManual(const cv::Vec3b& pixel_hsv) {
    double h = pixel_hsv[0] * 2.0;
    const double s = pixel_hsv[1] / 255.0;
    const double v = pixel_hsv[2] / 255.0;

    if (h >= 360.0) {
        h = 0.0;
    }

    const double c = v * s;
    const double x = c * (1.0 - std::abs(std::fmod(h / 60.0, 2.0) - 1.0));
    const double m = v - c;

    double rp = 0.0;
    double gp = 0.0;
    double bp = 0.0;

    if (h < 60.0) {
        rp = c;
        gp = x;
        bp = 0.0;
    } else if (h < 120.0) {
        rp = x;
        gp = c;
        bp = 0.0;
    } else if (h < 180.0) {
        rp = 0.0;
        gp = c;
        bp = x;
    } else if (h < 240.0) {
        rp = 0.0;
        gp = x;
        bp = c;
    } else if (h < 300.0) {
        rp = x;
        gp = 0.0;
        bp = c;
    } else {
        rp = c;
        gp = 0.0;
        bp = x;
    }

    const double r = (rp + m) * 255.0;
    const double g = (gp + m) * 255.0;
    const double b = (bp + m) * 255.0;

    return cv::Vec3b(clampToU8(b), clampToU8(g), clampToU8(r));
}

static cv::Mat convertirBgrAHsvManual(const cv::Mat& img_bgr) {
    cv::Mat img_hsv(img_bgr.rows, img_bgr.cols, CV_8UC3);
    for (int i = 0; i < img_bgr.rows; ++i) {
        for (int j = 0; j < img_bgr.cols; ++j) {
            const cv::Vec3b pixel_bgr = img_bgr.at<cv::Vec3b>(i, j);
            img_hsv.at<cv::Vec3b>(i, j) = bgrAHsvPixelManual(pixel_bgr);
        }
    }
    return img_hsv;
}

static cv::Mat convertirHsvABgrManual(const cv::Mat& img_hsv) {
    cv::Mat img_bgr(img_hsv.rows, img_hsv.cols, CV_8UC3);
    for (int i = 0; i < img_hsv.rows; ++i) {
        for (int j = 0; j < img_hsv.cols; ++j) {
            const cv::Vec3b pixel_hsv = img_hsv.at<cv::Vec3b>(i, j);
            img_bgr.at<cv::Vec3b>(i, j) = hsvABgrPixelManual(pixel_hsv);
        }
    }
    return img_bgr;
}

// ==============================
// Entrada de imagen por consola
// ==============================
static cv::Mat cargarImagenDesdeConsola() {
    std::string ruta;
    std::cout << "Ruta de imagen (Enter para usar \"gato.jpeg\"): ";
    if (!std::getline(std::cin, ruta)) {
        return cv::Mat();
    }
    if (ruta.empty()) {
        ruta = "gato.jpeg";
    }

    cv::Mat img = cv::imread(ruta, cv::IMREAD_COLOR);
    if (img.empty()) {
        std::cerr << "Error: no se pudo cargar la imagen en \"" << ruta << "\"." << std::endl;
    }
    return img;
}

// ==============================
// Ejercicio 1
// ==============================
static void ejercicio1(const cv::Mat& img_bgr) {
    const cv::Mat img_hsv = convertirBgrAHsvManual(img_bgr);
    const cv::Mat img_bgr_reconstruida = convertirHsvABgrManual(img_hsv);

    cv::imshow("Ejercicio 1 - Original BGR", img_bgr);
    cv::imshow("Ejercicio 1 - HSV (canales)", img_hsv);
    cv::imshow("Ejercicio 1 - BGR reconstruida", img_bgr_reconstruida);
    cv::waitKey(0);
    cv::destroyAllWindows();
}

// ==============================
// Ejercicio 2
// ==============================
static void ejercicio2(const cv::Mat& img_bgr, double factor_s = 1.5) {
    cv::Mat img_hsv = convertirBgrAHsvManual(img_bgr);

    for (int i = 0; i < img_hsv.rows; ++i) {
        for (int j = 0; j < img_hsv.cols; ++j) {
            cv::Vec3b pixel_hsv = img_hsv.at<cv::Vec3b>(i, j);
            const double s_nueva = pixel_hsv[1] * factor_s;
            pixel_hsv[1] = clampToU8(s_nueva);
            img_hsv.at<cv::Vec3b>(i, j) = pixel_hsv;
        }
    }

    const cv::Mat img_resultado = convertirHsvABgrManual(img_hsv);

    cv::imshow("Ejercicio 2 - Original", img_bgr);
    cv::imshow("Ejercicio 2 - Saturacion aumentada", img_resultado);
    cv::waitKey(0);
    cv::destroyAllWindows();
}

// ==============================
// Estructuras y utilidades para Ejercicio 3
// ==============================
struct PixelRGB {
    double r;
    double g;
    double b;
    PixelRGB() : r(0.0), g(0.0), b(0.0) {}
    PixelRGB(double r_, double g_, double b_) : r(r_), g(g_), b(b_) {}
};

static double distanciaEuclidiana(const PixelRGB& p1, const PixelRGB& p2) {
    const double dr = p1.r - p2.r;
    const double dg = p1.g - p2.g;
    const double db = p1.b - p2.b;
    return std::sqrt(dr * dr + dg * dg + db * db);
}

// ==============================
// Ejercicio 3
// ==============================
static void ejercicio3(const cv::Mat& img_bgr, int K = 5) {
    if (K < 2) {
        K = 2;
    }

    cv::Mat img_small;
    cv::resize(img_bgr, img_small, cv::Size(160, 120));

    const int rows = img_small.rows;
    const int cols = img_small.cols;
    const int total_pixels = rows * cols;

    std::vector<PixelRGB> pixeles(total_pixels);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            const cv::Vec3b px = img_small.at<cv::Vec3b>(i, j);
            pixeles[i * cols + j] = PixelRGB(
                static_cast<double>(px[2]),
                static_cast<double>(px[1]),
                static_cast<double>(px[0])
            );
        }
    }

    std::vector<PixelRGB> centroides(K);
    for (int c = 0; c < K; ++c) {
        const int idx = (c * total_pixels) / K;
        centroides[c] = pixeles[idx];
    }

    std::vector<int> asignaciones(total_pixels, 0);
    const int max_iteraciones = 20;
    for (int iter = 0; iter < max_iteraciones; ++iter) {
        bool hubo_cambio = false;

        for (int idx = 0; idx < total_pixels; ++idx) {
            double mejor_dist = std::numeric_limits<double>::max();
            int mejor_cluster = 0;

            for (int c = 0; c < K; ++c) {
                const double d = distanciaEuclidiana(pixeles[idx], centroides[c]);
                if (d < mejor_dist) {
                    mejor_dist = d;
                    mejor_cluster = c;
                }
            }

            if (asignaciones[idx] != mejor_cluster) {
                asignaciones[idx] = mejor_cluster;
                hubo_cambio = true;
            }
        }

        std::vector<double> suma_r(K, 0.0);
        std::vector<double> suma_g(K, 0.0);
        std::vector<double> suma_b(K, 0.0);
        std::vector<int> conteo(K, 0);

        for (int idx = 0; idx < total_pixels; ++idx) {
            const int c = asignaciones[idx];
            suma_r[c] += pixeles[idx].r;
            suma_g[c] += pixeles[idx].g;
            suma_b[c] += pixeles[idx].b;
            conteo[c] += 1;
        }

        for (int c = 0; c < K; ++c) {
            if (conteo[c] > 0) {
                centroides[c].r = suma_r[c] / conteo[c];
                centroides[c].g = suma_g[c] / conteo[c];
                centroides[c].b = suma_b[c] / conteo[c];
            } else {
                const int idx = (iter + c) % total_pixels;
                centroides[c] = pixeles[idx];
            }
        }

        if (!hubo_cambio) {
            break;
        }
    }

    cv::Mat img_quantizada(rows, cols, CV_8UC3);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            const int idx = i * cols + j;
            const PixelRGB& c = centroides[asignaciones[idx]];
            img_quantizada.at<cv::Vec3b>(i, j) =
                cv::Vec3b(clampToU8(c.b), clampToU8(c.g), clampToU8(c.r));
        }
    }

    const int paleta_h = 60;
    const int paleta_w = 60;
    cv::Mat paleta(paleta_h, paleta_w * K, CV_8UC3);
    for (int c = 0; c < K; ++c) {
        for (int y = 0; y < paleta_h; ++y) {
            for (int x = c * paleta_w; x < (c + 1) * paleta_w; ++x) {
                paleta.at<cv::Vec3b>(y, x) = cv::Vec3b(
                    clampToU8(centroides[c].b),
                    clampToU8(centroides[c].g),
                    clampToU8(centroides[c].r)
                );
            }
        }
    }

    cv::imshow("Ejercicio 3 - Original (redimensionada)", img_small);
    cv::imshow("Ejercicio 3 - KMeans Manual", img_quantizada);
    cv::imshow("Ejercicio 3 - Paleta", paleta);
    cv::waitKey(0);
    cv::destroyAllWindows();
}

// ==============================
// Ejercicio 4
// ==============================
static void ejercicio4(const cv::Mat& img_bgr) {
    const int rows = img_bgr.rows;
    const int cols = img_bgr.cols;
    const double total_pixels = static_cast<double>(rows * cols);

    double suma_b = 0.0;
    double suma_g = 0.0;
    double suma_r = 0.0;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            const cv::Vec3b px = img_bgr.at<cv::Vec3b>(i, j);
            suma_b += px[0];
            suma_g += px[1];
            suma_r += px[2];
        }
    }

    const double avg_b = suma_b / total_pixels;
    const double avg_g = suma_g / total_pixels;
    const double avg_r = suma_r / total_pixels;
    const double gray_avg = (avg_r + avg_g + avg_b) / 3.0;

    const double scale_b = gray_avg / (avg_b + 1e-12);
    const double scale_g = gray_avg / (avg_g + 1e-12);
    const double scale_r = gray_avg / (avg_r + 1e-12);

    cv::Mat img_resultado(rows, cols, CV_8UC3);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            const cv::Vec3b px = img_bgr.at<cv::Vec3b>(i, j);
            img_resultado.at<cv::Vec3b>(i, j) = cv::Vec3b(
                clampToU8(px[0] * scale_b),
                clampToU8(px[1] * scale_g),
                clampToU8(px[2] * scale_r)
            );
        }
    }

    cv::imshow("Ejercicio 4 - Original", img_bgr);
    cv::imshow("Ejercicio 4 - Gray World", img_resultado);
    cv::waitKey(0);
    cv::destroyAllWindows();
}

// ==============================
// Ejercicio 5
// ==============================
static void ejercicio5(const cv::Mat& img_bgr) {
    const int kernel[5][5] = {
        {1, 4, 6, 4, 1},
        {4, 16, 24, 16, 4},
        {6, 24, 36, 24, 6},
        {4, 16, 24, 16, 4},
        {1, 4, 6, 4, 1}
    };
    const int kernel_sum = 256;

    const int rows = img_bgr.rows;
    const int cols = img_bgr.cols;
    cv::Mat img_suavizada(rows, cols, CV_8UC3);

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            double acc_b = 0.0;
            double acc_g = 0.0;
            double acc_r = 0.0;

            for (int ky = -2; ky <= 2; ++ky) {
                for (int kx = -2; kx <= 2; ++kx) {
                    const int yy = clampInt(y + ky, 0, rows - 1);
                    const int xx = clampInt(x + kx, 0, cols - 1);
                    const int peso = kernel[ky + 2][kx + 2];
                    const cv::Vec3b px = img_bgr.at<cv::Vec3b>(yy, xx);

                    acc_b += peso * px[0];
                    acc_g += peso * px[1];
                    acc_r += peso * px[2];
                }
            }

            img_suavizada.at<cv::Vec3b>(y, x) = cv::Vec3b(
                clampToU8(acc_b / kernel_sum),
                clampToU8(acc_g / kernel_sum),
                clampToU8(acc_r / kernel_sum)
            );
        }
    }

    cv::imshow("Ejercicio 5 - Original", img_bgr);
    cv::imshow("Ejercicio 5 - Suavizado gaussiano manual", img_suavizada);
    cv::waitKey(0);
    cv::destroyAllWindows();
}

// ==============================
// Ejercicio 6
// ==============================
static void ejercicio6(const cv::Mat& img_bgr, double gamma = 1.5) {
    if (gamma <= 0.0) {
        gamma = 1.0;
    }

    std::array<uchar, 256> tabla{};
    for (int i = 0; i < 256; ++i) {
        const double normalizado = i / 255.0;
        const double corregido = 255.0 * std::pow(normalizado, gamma);
        tabla[i] = clampToU8(corregido);
    }

    cv::Mat img_resultado(img_bgr.rows, img_bgr.cols, CV_8UC3);
    for (int i = 0; i < img_bgr.rows; ++i) {
        for (int j = 0; j < img_bgr.cols; ++j) {
            const cv::Vec3b px = img_bgr.at<cv::Vec3b>(i, j);
            img_resultado.at<cv::Vec3b>(i, j) = cv::Vec3b(
                tabla[px[0]],
                tabla[px[1]],
                tabla[px[2]]
            );
        }
    }

    cv::imshow("Ejercicio 6 - Original", img_bgr);
    cv::imshow("Ejercicio 6 - Gamma manual", img_resultado);
    cv::waitKey(0);
    cv::destroyAllWindows();
}

// ==============================
// Ejercicio 7
// ==============================
static void ejercicio7(const cv::Mat& img_bgr, double k = 0.4) {
    const int rows = img_bgr.rows;
    const int cols = img_bgr.cols;

    const double cx = cols / 2.0;
    const double cy = rows / 2.0;
    const double d_max = std::sqrt(cx * cx + cy * cy);

    cv::Mat img_resultado(rows, cols, CV_8UC3);
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            const cv::Vec3b px = img_bgr.at<cv::Vec3b>(y, x);

            const double dx = x - cx;
            const double dy = y - cy;
            const double d = std::sqrt(dx * dx + dy * dy);
            const double d_norm = d / (d_max + 1e-12);

            double denom = 1.0 - k * d_norm * d_norm;
            if (denom < 1e-6) {
                denom = 1e-6;
            }
            const double f = 1.0 / denom;

            img_resultado.at<cv::Vec3b>(y, x) = cv::Vec3b(
                clampToU8(px[0] * f),
                clampToU8(px[1] * f),
                clampToU8(px[2] * f)
            );
        }
    }

    cv::imshow("Ejercicio 7 - Original", img_bgr);
    cv::imshow("Ejercicio 7 - Vignette corregido", img_resultado);
    cv::waitKey(0);
    cv::destroyAllWindows();
}

// ==============================
// Menu y flujo principal
// ==============================
static void imprimirMenu() {
    std::cout << "\n=== Taller 2 - Menu de Ejercicios ===\n";
    std::cout << "1. RGB a HSV (manual)\n";
    std::cout << "2. Modificar saturacion\n";
    std::cout << "3. K-Means manual\n";
    std::cout << "4. Gray World\n";
    std::cout << "5. Suavizado Gaussiano\n";
    std::cout << "6. Correccion Gamma\n";
    std::cout << "7. Correccion Vignette\n";
    std::cout << "0. Salir\n";
    std::cout << "Opcion: ";
}

int main() {
    while (true) {
        imprimirMenu();

        int opcion = -1;
        if (!(std::cin >> opcion)) {
            if (std::cin.eof()) {
                std::cout << "\nEntrada cerrada. Saliendo..." << std::endl;
                break;
            }
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cerr << "Entrada invalida. Intenta de nuevo." << std::endl;
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (opcion == 0) {
            std::cout << "Saliendo..." << std::endl;
            break;
        }

        cv::Mat imagen = cargarImagenDesdeConsola();
        if (imagen.empty()) {
            continue;
        }

        switch (opcion) {
            case 1:
                ejercicio1(imagen);
                break;
            case 2:
                ejercicio2(imagen);
                break;
            case 3: {
                int k = 5;
                std::cout << "Valor de K (default 5): ";
                if (!(std::cin >> k)) {
                    if (std::cin.eof()) {
                        std::cout << "\nEntrada cerrada. Saliendo..." << std::endl;
                        return 0;
                    }
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    k = 5;
                }
                ejercicio3(imagen, k);
                break;
            }
            case 4:
                ejercicio4(imagen);
                break;
            case 5:
                ejercicio5(imagen);
                break;
            case 6: {
                double gamma = 1.5;
                std::cout << "Gamma (default 1.5): ";
                if (!(std::cin >> gamma)) {
                    if (std::cin.eof()) {
                        std::cout << "\nEntrada cerrada. Saliendo..." << std::endl;
                        return 0;
                    }
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    gamma = 1.5;
                }
                ejercicio6(imagen, gamma);
                break;
            }
            case 7: {
                double k = 0.4;
                std::cout << "k de correccion (default 0.4): ";
                if (!(std::cin >> k)) {
                    if (std::cin.eof()) {
                        std::cout << "\nEntrada cerrada. Saliendo..." << std::endl;
                        return 0;
                    }
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    k = 0.4;
                }
                ejercicio7(imagen, k);
                break;
            }
            default:
                std::cerr << "Opcion invalida. Elige entre 0 y 7." << std::endl;
                break;
        }
    }

    return 0;
}
