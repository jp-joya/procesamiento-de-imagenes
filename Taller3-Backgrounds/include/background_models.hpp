#pragma once

#include <opencv2/opencv.hpp>

void frameDifferenceManual(
    const cv::Mat& prevGray,
    const cv::Mat& currGray,
    cv::Mat& diff,
    cv::Mat& motionMask,
    unsigned char thresholdValue
);

void runningAverageUpdateManual(
    const cv::Mat& currGray,
    cv::Mat& backgroundFloat,
    cv::Mat& diff,
    cv::Mat& motionMask,
    unsigned char thresholdValue,
    float alpha
);
