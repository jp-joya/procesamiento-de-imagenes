#include "background_models.hpp"

#include <algorithm>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

int main() {
    int cameraIndex = 0;
    int thresholdValue = 30;
    int blurKernel = 5; // impar y >= 1

    thresholdValue = std::clamp(thresholdValue, 0, 255);
    if (blurKernel < 1) {
        blurKernel = 1;
    }
    if (blurKernel % 2 == 0) {
        blurKernel += 1;
    }

    cv::VideoCapture cap(cameraIndex);
    if (!cap.isOpened()) {
        std::cerr << "No se pudo abrir la camara con indice " << cameraIndex << '\n';
        return 1;
    }

    cv::Mat frame, prevGray, currGray, diff, motionMask;
    if (!cap.read(frame) || frame.empty()) {
        std::cerr << "No se pudo leer el primer frame.\n";
        return 1;
    }

    cv::cvtColor(frame, prevGray, cv::COLOR_BGR2GRAY);
    if (blurKernel > 1) {
        cv::GaussianBlur(prevGray, prevGray, cv::Size(blurKernel, blurKernel), 0.0);
    }

    std::cout << "[frame] Controles: q/ESC salir, +/- ajustar umbral\n";

    while (true) {
        if (!cap.read(frame) || frame.empty()) {
            std::cerr << "No se pudo leer un frame de la camara.\n";
            break;
        }

        cv::cvtColor(frame, currGray, cv::COLOR_BGR2GRAY);
        if (blurKernel > 1) {
            cv::GaussianBlur(currGray, currGray, cv::Size(blurKernel, blurKernel), 0.0);
        }

        frameDifferenceManual(
            prevGray,
            currGray,
            diff,
            motionMask,
            static_cast<unsigned char>(thresholdValue)
        );

        const int movingPixels = cv::countNonZero(motionMask);
        cv::Mat view = frame.clone();
        cv::putText(
            view,
            "Metodo: Frame diff | Umbral: " + std::to_string(thresholdValue) +
                " | Pixeles en movimiento: " + std::to_string(movingPixels),
            cv::Point(10, 30),
            cv::FONT_HERSHEY_SIMPLEX,
            0.6,
            cv::Scalar(0, 255, 0),
            2
        );

        cv::imshow("Camara", view);
        cv::imshow("Frame - Diferencia absoluta manual", diff);
        cv::imshow("Frame - Mascara de movimiento manual", motionMask);

        const int key = cv::waitKey(1);
        if (key == 'q' || key == 27) {
            break;
        }
        if (key == '+' || key == '=') {
            thresholdValue = std::min(255, thresholdValue + 1);
        } else if (key == '-' || key == '_') {
            thresholdValue = std::max(0, thresholdValue - 1);
        }

        currGray.copyTo(prevGray);
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
