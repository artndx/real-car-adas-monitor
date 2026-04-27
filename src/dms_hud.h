#pragma once
#include <dms_monitor.h>

namespace dms
{
    class DMSHUD
    {
    public:
        void render(cv::Mat& frame, const DriverState& state);

    private:
        void drawFaceBox(cv::Mat& frame, const cv::Rect& faceRect);
        void drawStatusIndicators(cv::Mat& frame, const DriverState& state);
        void drawDrowsyAlert(cv::Mat& frame);
        void drawDistractedAlert(cv::Mat& frame);
    };
}