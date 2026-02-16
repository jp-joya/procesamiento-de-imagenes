#include "background_models.hpp"

#include <algorithm>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

int main() {
    int cameraIndex = 0;
    int thresholdValue = 30;
    int blurKernel = 5; // impar y >= 1
    float alpha = 0.02f; // tasa de actualizacion del fondo [0,1]

    thresholdValue = std::clamp(thresholdValue, 0, 255);
    if (blurKernel < 1) {
        blurKernel = 1;
    }
    if (blurKernel % 2 == 0) {
        blurKernel += 1;
    }
    alpha = std::clamp(alpha, 0.0f, 1.0f);

    cv::VideoCapture cap(cameraIndex);
    if (!cap.isOpened()) {
        std::cerr << "No se pudo abrir la camara con indice " << cameraIndex << '\n';
        return 1;
    }

    cv::Mat frame, currGray, backgroundFloat, diff, motionMask, backgroundView;
    if (!cap.read(frame) || frame.empty()) {
        std::cerr << "No se pudo leer el primer frame.\n";
        return 1;
    }

    cv::cvtColor(frame, currGray, cv::COLOR_BGR2GRAY);
    if (blurKernel > 1) {
        cv::GaussianBlur(currGray, currGray, cv::Size(blurKernel, blurKernel), 0.0);
    }
    currGray.convertTo(backgroundFloat, CV_32FC1);

    std::cout << "[running_average_manual] Controles: q/ESC salir, +/- umbral, [/ ] alpha\n";

    while (true) {
        if (!cap.read(frame) || frame.empty()) {
            std::cerr << "No se pudo leer un frame de la camara.\n";
            break;
        }

        cv::cvtColor(frame, currGray, cv::COLOR_BGR2GRAY);
        if (blurKernel > 1) {
            cv::GaussianBlur(currGray, currGray, cv::Size(blurKernel, blurKernel), 0.0);
        }

        runningAverageUpdateManual(
            currGray,
            backgroundFloat,
            diff,
            motionMask,
            static_cast<unsigned char>(thresholdValue),
            alpha
        );

        backgroundFloat.convertTo(backgroundView, CV_8UC1);
        const int movingPixels = cv::countNonZero(motionMask);

        cv::Mat view = frame.clone();
        cv::putText(
            view,
            "Metodo: Running Avg | Umbral: " + std::to_string(thresholdValue) +
                " | Alpha: " + cv::format("%.3f", alpha) +
                " | Pixeles en movimiento: " + std::to_string(movingPixels),
            cv::Point(10, 30),
            cv::FONT_HERSHEY_SIMPLEX,
            0.55,
            cv::Scalar(0, 255, 0),
            2
        );

        cv::imshow("Camara", view);
        cv::imshow("Running Avg - Fondo estimado manual", backgroundView);
        cv::imshow("Running Avg - Diferencia absoluta manual", diff);
        cv::imshow("Running Avg - Mascara de movimiento manual", motionMask);

        const int key = cv::waitKey(1);
        if (key == 'q' || key == 27) {
            break;
        }
        if (key == '+' || key == '=') {
            thresholdValue = std::min(255, thresholdValue + 1);
        } else if (key == '-' || key == '_') {
            thresholdValue = std::max(0, thresholdValue - 1);
        } else if (key == ']') {
            alpha = std::min(1.0f, alpha + 0.001f);
        } else if (key == '[') {
            alpha = std::max(0.0f, alpha - 0.001f);
        }
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
