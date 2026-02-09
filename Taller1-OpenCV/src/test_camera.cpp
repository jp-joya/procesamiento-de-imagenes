#include <iostream>
#include <opencv2/opencv.hpp>

// Punto de entrada: abre la camara por defecto y muestra video en tiempo real.
int main() {
    cv::VideoCapture cap(0);

    if (!cap.isOpened()) {
        std::cout << "No se puede acceder a la camara." << std::endl;
        return 1;
    }

    while (true) {
        cv::Mat frame;
        if (!cap.read(frame)) {
            std::cout << "No se pudo capturar el cuadro." << std::endl;
            break;
        }

        cv::imshow("Camera", frame);
        const int key = cv::waitKey(1) & 0xFF;
        if (key == 'q' || key == 'Q') {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
