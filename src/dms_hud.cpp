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
    void DMSHUD::draw(cv::Mat& targetFrame, const cv::Mat& cameraFrame, const DriverState& state)
    {
        cv::Rect rightPanel(targetFrame.cols / 2, 0, targetFrame.cols / 2, targetFrame.rows);
        cv::Mat panel = targetFrame(rightPanel);

        cv::Mat resized;

        cv::resize(cameraFrame, resized, panel.size());

        resized.copyTo(panel);

        if (state.m_face_detected)
        {
            float sx = static_cast<float>(panel.cols) / cameraFrame.cols;

            float sy = static_cast<float>(panel.rows) / cameraFrame.rows;

            cv::Rect face(
                static_cast<int>(state.m_face_rect.x * sx),
                static_cast<int>(state.m_face_rect.y * sy),
                static_cast<int>(state.m_face_rect.width * sx),
                static_cast<int>(state.m_face_rect.height * sy));

            drawFaceCorners(panel, face, cv::Scalar(0,255,0));
        }

        drawStatusLine(panel, 30, "Face", state.m_face_detected);
        drawStatusLine(panel, 60, "Eyes", state.m_eyes_open);
        drawStatusLine(panel, 90, "Forward", state.m_looking_forward);

        cv::putText(panel, "Eye openness: " +  std::to_string(state.m_eye_openness),
            {20,130}, cv::FONT_HERSHEY_SIMPLEX, 0.6, {255,255,255}, 2);

        cv::putText(panel, "Head: " + std::to_string(static_cast<int>(state.m_head_turn_deg)) + " deg",
            {20,160}, cv::FONT_HERSHEY_SIMPLEX, 0.6, {255,255,255}, 2);

        if (state.m_alert_drowsy)
            drawDrowsinessAlert(panel);

        if (state.m_alert_distracted)
            drawDistractionAlert(panel);
    }

    void DMSHUD::drawFaceCorners(cv::Mat& frame, const cv::Rect& r, const cv::Scalar& color)
    {
        int l = 20;

        cv::line(frame,{r.x,r.y},{r.x+l,r.y},color,3);
        cv::line(frame,{r.x,r.y},{r.x,r.y+l},color,3);

        cv::line(frame,{r.br().x,r.y},{r.br().x-l,r.y},color,3);
        cv::line(frame,{r.br().x,r.y},{r.br().x,r.y+l},color,3);

        cv::line(frame,{r.x,r.br().y},{r.x+l,r.br().y},color,3);
        cv::line(frame,{r.x,r.br().y},{r.x,r.br().y-l},color,3);

        cv::line(frame,{r.br().x,r.br().y},{r.br().x-l,r.br().y},color,3);
        cv::line(frame,{r.br().x,r.br().y},{r.br().x,r.br().y-l},color,3);
    }

    void DMSHUD::drawStatusLine(cv::Mat& frame, int y, const std::string& text, bool value)
    {
        cv::Scalar color = value ? cv::Scalar(0,255,0) : cv::Scalar(0,0,255);
        cv::circle(frame, {20,y-5}, 8, color, cv::FILLED);
        cv::putText(frame, text, {40,y}, cv::FONT_HERSHEY_SIMPLEX, 0.6, {255,255,255}, 2);
    }

    void DMSHUD::drawDrowsinessAlert(cv::Mat& frame)
    {
        cv::rectangle(frame, {0,0,frame.cols,frame.rows}, cv::Scalar(0,165,255), 8);
        cv::putText(frame, "DROWSINESS ALERT", {60,frame.rows/2}, cv::FONT_HERSHEY_DUPLEX, 1.0, cv::Scalar(0,165,255), 3);
    }

    void DMSHUD::drawDistractionAlert(
        cv::Mat& frame)
    {
        cv::rectangle(frame, {0,frame.rows-50,frame.cols,50}, cv::Scalar(0,0,255), cv::FILLED);
        cv::putText(frame, "DISTRACTION",  {20,frame.rows-15}, cv::FONT_HERSHEY_DUPLEX, 0.8, cv::Scalar(255,255,255), 2);
    }
}