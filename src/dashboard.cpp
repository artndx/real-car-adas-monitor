#include <dashboard.h>
#include <algorithm>
#include <cmath>

namespace dashboard
{
    cv::Mat Dashboard::draw(const OBDRecord& record) 
    {
        cv::Mat frame = cv::Mat::zeros(480, 640, CV_8UC3);
        
        cv::Mat roi = frame(cv::Rect(0, 0, 640, 480));
        roi.setTo(cv::Scalar(15, 15, 15));

        GaugeParams speed_params {0, 140, record.m_speed, 90, "SPEED", "km/h"};
        GaugeParams rpm_params {0, 6000, record.m_engine_rpm, 4500, "RPM", ""};

        drawGauge(roi, cv::Point(160, 160), 90, speed_params);
        drawGauge(roi, cv::Point(480, 160), 90, rpm_params);

        int bar_y = 300;
        int bar_w = 240;
        int bar_h = 18;

        const float cool_temp_limit = 100.0;
        const float fuel_limit = 15.0;

        GaugeParams cool_temp_params {0, 120, record.m_coolant_temp, cool_temp_limit, "TEMP", " C"};
        GaugeParams fuel_params {0, 100, record.m_fuel_level, fuel_limit, "FUEL", "%"};
        GaugeParams throttle_params {0, 100, record.m_throttle_position, 100, "THR", "%"};
        
        drawLinearGauge(roi, cv::Point(50, bar_y), cv::Size(bar_w, bar_h), cool_temp_params);
        drawLinearGauge(roi, cv::Point(350, bar_y), cv::Size(bar_w, bar_h), fuel_params, true);
        drawLinearGauge(roi, cv::Point(50, bar_y + 45), cv::Size(bar_w, bar_h), throttle_params);

        std::string style_txt = std::format("STYLE: {}", getLabelTypeStr(record.m_label));
        cv::Scalar style_clr;
        switch (record.m_label) 
        {
            case LabelType::NONE:
                style_clr = cv::Scalar(200, 200, 200); 
                break;
            case LabelType::SLOW: 
                style_clr = cv::Scalar(255, 200, 0); 
                break;
            case LabelType::NORMAL: 
                style_clr = cv::Scalar(0, 255, 0); 
                break;
            case LabelType::AGGRESSIVE: 
                style_clr = cv::Scalar(0, 0, 255); 
                break;
        }
        
        cv::putText(roi, style_txt, cv::Point(350, bar_y + 45 + 40), cv::FONT_HERSHEY_SIMPLEX, 0.7, style_clr, 2, cv::LINE_AA);

        int warn_y = 460;
        int offset = 0;
        if (record.m_coolant_temp > cool_temp_limit) 
        {
            drawWarning(roi, "HIGH TEMP", cv::Point(30, warn_y - offset));
            offset += 25;
        }

        if (record.m_fuel_level < fuel_limit) 
        {
            drawWarning(roi, "LOW FUEL", cv::Point(30, warn_y - offset));
            offset += 25;
        }

        return frame;
    }

    void Dashboard::drawGauge(cv::Mat& frame, const cv::Point& center, int radius, 
                                const GaugeParams& params, float warning_threshold)
    {
        cv::circle(frame, center, radius, cv::Scalar(30, 30, 30), 18, cv::LINE_AA);
        cv::circle(frame, center, radius, cv::Scalar(60, 60, 60), 12, cv::LINE_AA);

        float current_val = std::clamp(params.m_current_val, params.m_min_val, params.m_max_val);
        float ratio = (current_val - params.m_min_val) / (params.m_max_val - params.m_min_val);

        double start_ang = -135.0;
        double sweep = 270.0;
        double cur_ang = start_ang + ratio * sweep;

        cv::Scalar arc_color = cv::Scalar(0, 255, 0);
        if (warning_threshold > 0 && current_val > warning_threshold) 
            arc_color = cv::Scalar(0, 0, 255);

        cv::ellipse(frame, center, cv::Size(radius, radius), 0, start_ang, cur_ang, arc_color, 12, cv::LINE_AA);

        double rad = CV_PI * cur_ang / 180.0;
        cv::Point needle(center.x + radius * 0.85 * std::cos(rad),
                        center.y + radius * 0.85 * std::sin(rad));
        cv::line(frame, center, needle, cv::Scalar(255, 255, 255), 2, cv::LINE_AA);
        cv::circle(frame, center, 4, cv::Scalar(255, 255, 255), -1);

        std::string text = std::to_string(static_cast<int>(current_val)) + " " + params.unit;
        int base = 0;
        cv::Size sz = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.6, 1, &base);
        cv::putText(frame, text, cv::Point(center.x - sz.width / 2, center.y + sz.height / 2 + 27),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
        cv::putText(frame, params.label, cv::Point(center.x - sz.width / 2, center.y + sz.height / 2 + 42),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(180, 180, 180), 1, cv::LINE_AA);
    }

    void Dashboard::drawLinearGauge(cv::Mat& frame, const cv::Point& start, const cv::Size& size, 
                                    const GaugeParams& params, float warning_threshold, bool warn_below ) 
    {
        float current_val = std::clamp(params.m_current_val, params.m_min_val, params.m_max_val);
        float ratio = (current_val - params.m_min_val) / (params.m_max_val - params.m_min_val);

        cv::rectangle(frame, start, cv::Point(start.x + size.width, start.y + size.height), cv::Scalar(40, 40, 40), -1, cv::LINE_AA);

        cv::Scalar fill = cv::Scalar(0, 165, 255);
        if (warning_threshold > 0) 
        {
            bool is_warning = (warn_below && current_val < warning_threshold) || 
                                (!warn_below && current_val > warning_threshold);
            if (is_warning) 
                fill = cv::Scalar(0, 0, 255);
        }

        int width = static_cast<int>(size.width * ratio);
        cv::rectangle(frame, start, cv::Point(start.x + width, start.y + size.height), fill, -1, cv::LINE_AA);

        std::string text = std::format("{} : {} {}", params.label, current_val, params.unit);
        cv::putText(frame, text, cv::Point(start.x, start.y - 8),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(220, 220, 220), 1, cv::LINE_AA);
    }

    void Dashboard::drawWarning(cv::Mat& img, const std::string& msg, const cv::Point& pos) 
    {
        cv::putText(img, "Warning: " + msg, pos, cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
    }
}