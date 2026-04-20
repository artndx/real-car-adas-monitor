#pragma once
#include <opencv2/opencv.hpp>
#include <common.h>
#include <string>

namespace dashboard
{
    struct GaugeParams
    {
        float m_min_val; 
        float m_max_val;
        float m_current_val;
        float m_warning_limit;
        std::string label;
        std::string unit;
    };

    class Dashboard {
    public:
        Dashboard() = default;
        
        cv::Mat draw(const OBDRecord& record);
    private:
        void drawGauge(cv::Mat& frame, const cv::Point& center, int radius, 
                        const GaugeParams& params, float warning_threshold = -1.0f);
        void drawLinearGauge(cv::Mat& frame, const cv::Point& start, const cv::Size& size, 
                        const GaugeParams& params, float warning_threshold = -1.0f, bool warn_below = false);
        void drawWarning(cv::Mat& img, const std::string& msg, const cv::Point& pos);
    };
}