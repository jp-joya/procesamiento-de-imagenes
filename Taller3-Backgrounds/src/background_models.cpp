#include "background_models.hpp"

#include <algorithm>

void frameDifferenceManual(
    const cv::Mat& prevGray,
    const cv::Mat& currGray,
    cv::Mat& diff,
    cv::Mat& motionMask,
    unsigned char thresholdValue
) {
    CV_Assert(prevGray.type() == CV_8UC1 && currGray.type() == CV_8UC1);
    CV_Assert(prevGray.size() == currGray.size());

    diff.create(currGray.size(), CV_8UC1);
    motionMask.create(currGray.size(), CV_8UC1);

    for (int y = 0; y < currGray.rows; ++y) {
        const unsigned char* pPrev = prevGray.ptr<unsigned char>(y);
        const unsigned char* pCurr = currGray.ptr<unsigned char>(y);
        unsigned char* pDiff = diff.ptr<unsigned char>(y);
        unsigned char* pMask = motionMask.ptr<unsigned char>(y);

        for (int x = 0; x < currGray.cols; ++x) {
            int delta = static_cast<int>(pCurr[x]) - static_cast<int>(pPrev[x]);
            if (delta < 0) {
                delta = -delta;
            }

            pDiff[x] = static_cast<unsigned char>(delta);
            pMask[x] = (delta >= thresholdValue) ? 255 : 0;
        }
    }
}

void runningAverageUpdateManual(
    const cv::Mat& currGray,
    cv::Mat& backgroundFloat,
    cv::Mat& diff,
    cv::Mat& motionMask,
    unsigned char thresholdValue,
    float alpha
) {
    CV_Assert(currGray.type() == CV_8UC1);
    CV_Assert(backgroundFloat.type() == CV_32FC1);
    CV_Assert(currGray.size() == backgroundFloat.size());

    alpha = std::clamp(alpha, 0.0f, 1.0f);

    diff.create(currGray.size(), CV_8UC1);
    motionMask.create(currGray.size(), CV_8UC1);

    for (int y = 0; y < currGray.rows; ++y) {
        const unsigned char* pCurr = currGray.ptr<unsigned char>(y);
        float* pBg = backgroundFloat.ptr<float>(y);
        unsigned char* pDiff = diff.ptr<unsigned char>(y);
        unsigned char* pMask = motionMask.ptr<unsigned char>(y);

        for (int x = 0; x < currGray.cols; ++x) {
            const float updatedBg = (1.0f - alpha) * pBg[x] + alpha * static_cast<float>(pCurr[x]);
            pBg[x] = updatedBg;

            int bgValue = static_cast<int>(updatedBg + 0.5f);
            if (bgValue < 0) {
                bgValue = 0;
            } else if (bgValue > 255) {
                bgValue = 255;
            }

            int delta = static_cast<int>(pCurr[x]) - bgValue;
            if (delta < 0) {
                delta = -delta;
            }

            pDiff[x] = static_cast<unsigned char>(delta);
            pMask[x] = (delta >= thresholdValue) ? 255 : 0;
        }
    }
}
