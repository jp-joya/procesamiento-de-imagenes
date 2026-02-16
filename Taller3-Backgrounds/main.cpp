#include "background_models.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

static cv::Size getScreenSizeOrDefault() {
    const char* envWidth = std::getenv("SCREEN_WIDTH");
    const char* envHeight = std::getenv("SCREEN_HEIGHT");

    int width = 1920;
    int height = 1080;

    if (envWidth != nullptr) {
        width = std::max(1, std::atoi(envWidth));
    }
    if (envHeight != nullptr) {
        height = std::max(1, std::atoi(envHeight));
    }

    return cv::Size(width, height);
}

static void setupCenteredGridWindows(const cv::Size& frameSize) {
    const std::vector<std::string> windowNames = {
        "Frame - Diferencia absoluta manual",
        "Frame - Mascara de movimiento manual",
        "Running Avg - Diferencia absoluta manual",
        "Running Avg - Mascara de movimiento manual"
    };

    const cv::Size screen = getScreenSizeOrDefault();
    const int gap = 12;
    const int usableW = std::max(1, screen.width - (3 * gap));
    const int usableH = std::max(1, screen.height - (3 * gap));

    const int cellMaxW = std::max(1, usableW / 2);
    const int cellMaxH = std::max(1, usableH / 2);

    const double scaleW = static_cast<double>(cellMaxW) / static_cast<double>(frameSize.width);
    const double scaleH = static_cast<double>(cellMaxH) / static_cast<double>(frameSize.height);
    const double scale = std::min(1.0, std::min(scaleW, scaleH));

    const int cellW = std::max(1, static_cast<int>(frameSize.width * scale));
    const int cellH = std::max(1, static_cast<int>(frameSize.height * scale));

    const int gridW = (2 * cellW) + gap;
    const int gridH = (2 * cellH) + gap;
    const int startX = std::max(0, (screen.width - gridW) / 2);
    const int startY = std::max(0, (screen.height - gridH) / 2);

    for (int i = 0; i < static_cast<int>(windowNames.size()); ++i) {
        const int row = i / 2;
        const int col = i % 2;
        const int x = startX + col * (cellW + gap);
        const int y = startY + row * (cellH + gap);

        cv::namedWindow(windowNames[i], cv::WINDOW_NORMAL);
        cv::resizeWindow(windowNames[i], cellW, cellH);
        cv::moveWindow(windowNames[i], x, y);
    }
}

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

    cv::Mat frame, prevGray, currGray;
    cv::Mat diffFrame, maskFrame;
    cv::Mat diffRunning, maskRunning, backgroundFloat;

    if (!cap.read(frame) || frame.empty()) {
        std::cerr << "No se pudo leer el primer frame.\n";
        return 1;
    }

    cv::cvtColor(frame, prevGray, cv::COLOR_BGR2GRAY);
    if (blurKernel > 1) {
        cv::GaussianBlur(prevGray, prevGray, cv::Size(blurKernel, blurKernel), 0.0);
    }
    prevGray.convertTo(backgroundFloat, CV_32FC1);

    setupCenteredGridWindows(prevGray.size());

    std::cout
        << "[main] Controles: q/ESC salir, +/- umbral, [/ ] alpha\n";

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
            diffFrame,
            maskFrame,
            static_cast<unsigned char>(thresholdValue)
        );

        runningAverageUpdateManual(
            currGray,
            backgroundFloat,
            diffRunning,
            maskRunning,
            static_cast<unsigned char>(thresholdValue),
            alpha
        );
        cv::imshow("Frame - Diferencia absoluta manual", diffFrame);
        cv::imshow("Frame - Mascara de movimiento manual", maskFrame);
        cv::imshow("Running Avg - Diferencia absoluta manual", diffRunning);
        cv::imshow("Running Avg - Mascara de movimiento manual", maskRunning);

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

        currGray.copyTo(prevGray);
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
