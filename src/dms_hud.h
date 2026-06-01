#pragma once
#include <dms_monitor.h>

namespace dms
{
class DMSHUD
{
    public:
        void draw(cv::Mat& targetFrame, const cv::Mat& cameraFrame, const DriverState& state);
    private:
        void drawFaceCorners(cv::Mat& frame, const cv::Rect& rect, const cv::Scalar& color);
        void drawStatusLine(cv::Mat& frame, int y, const std::string& text, bool value);
        void drawDrowsinessAlert(cv::Mat& frame);
        void drawDistractionAlert(cv::Mat& frame);
    };
}