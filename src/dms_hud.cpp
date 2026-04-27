#include <dms_hud.h>

namespace colors
{
    const cv::Scalar OK = cv::Scalar{0, 255, 0};
    const cv::Scalar WARNING = cv::Scalar{0, 165, 255};
    const cv::Scalar DANGER = cv::Scalar{0, 0, 255};
    const cv::Scalar TEXT = cv::Scalar{255, 255, 255};
    const cv::Scalar BACKGROUND = cv::Scalar{30, 30, 30};
}

namespace dms
{
    void DMSHUD::render(cv::Mat& frame, const DriverState& state)
    {
        if (frame.empty()) 
            return;

        if (state.m_face_detected) 
            drawFaceBox(frame, state.m_face_rect);

        drawStatusIndicators(frame, state);

        if (state.m_alert_drowsy) 
            drawDrowsyAlert(frame);
        
        if (state.m_alert_distracted)
            drawDistractedAlert(frame);
    }

    void DMSHUD::drawFaceBox(cv::Mat& frame, const cv::Rect& rect)
    {
        const int minDim = std::max(20, std::min(rect.width, rect.height));
        const int lineLen = minDim / 4;
        const int thickness = 2;

        auto drawCorner = [&](cv::Point p, bool right, bool down) 
        {
            cv::line(frame, p, p + cv::Point(right ? lineLen : -lineLen, 0), colors::OK, thickness);
            cv::line(frame, p, p + cv::Point(0, down ? lineLen : -lineLen), colors::OK, thickness);
        };

        drawCorner(rect.tl(), true, true);                               // Верх-Лево
        drawCorner(cv::Point(rect.x + rect.width, rect.y), false, true); // Верх-Право
        drawCorner(cv::Point(rect.x, rect.y + rect.height), true, false);// Низ-Лево
        drawCorner(rect.br(), false, false);                             // Низ-Право
    }

    void DMSHUD::drawStatusIndicators(cv::Mat& frame, const DriverState& state)
    {
        cv::Point base(20, 40);
        const int lineStep = 32;
        const int fontFace = cv::FONT_HERSHEY_SIMPLEX;
        const double fontScale = 0.7;
        const int thickness = 2;

        cv::circle(frame, base + cv::Point(0, 10), 6, state.m_eyes_open ? colors::OK : colors::WARNING, -1);
        cv::putText(frame, state.m_eyes_open ? "Eyes is open" : "Eyes is closed",
                    base + cv::Point(22, 10), fontFace, fontScale, colors::TEXT, thickness);

        cv::Point headPos = base + cv::Point(0, lineStep);
        cv::circle(frame, headPos + cv::Point(0, 10), 6, state.m_looking_forward ? colors::OK : colors::WARNING, -1);
        cv::putText(frame, state.m_looking_forward ? "Head straight ahead" : "Head is turned",
                    headPos + cv::Point(22, 10), fontFace, fontScale, colors::TEXT, thickness);

        cv::Point anglePos = headPos + cv::Point(0, lineStep);
        std::ostringstream oss;
        oss << "Turn angle: " << std::fixed << std::setprecision(1) << state.m_head_turn_deg << "°";
        cv::putText(frame, oss.str(), anglePos + cv::Point(10, 10), fontFace, 0.6, colors::TEXT, 1);
    }

    void DMSHUD::drawDrowsyAlert(cv::Mat& frame)
    {
        const int margin = 50;
        const int boxH = 70;
        cv::Rect alertBox(margin, (frame.rows - boxH) / 2, frame.rows - margin * 2, boxH);

        cv::Mat roi = frame(alertBox);
        roi = colors::BACKGROUND * 0.7 + roi * 0.3;

        cv::rectangle(frame, alertBox, colors::WARNING, 3);
        cv::putText(frame, "DROWSINESS ALERT", cv::Point(alertBox.x + 25, alertBox.y + 42),
                    cv::FONT_HERSHEY_SIMPLEX, 1.0, colors::WARNING, 2);
    }

    void DMSHUD::drawDistractedAlert(cv::Mat& frame)
    {
        const int barH = 45;
        cv::Rect bar(0, frame.rows - barH, frame.rows, barH);

        cv::rectangle(frame, bar, colors::DANGER, cv::FILLED);
        cv::putText(frame, "DISTRACTION", cv::Point(25, frame.rows - 12),
                    cv::FONT_HERSHEY_SIMPLEX, 0.9, cv::Scalar(255, 255, 255), 2);
    }
}