#pragma once
#include <opencv2/opencv.hpp>

namespace dms
{
    struct DriverState
    {
        bool m_face_detected = false;
        bool m_eyes_open = false;
        bool m_looking_forward = false;

        float m_eye_openness = 0.0;
        float m_head_turn_deg = 0.0;

        bool m_alert_drowsy = false;
        bool m_alert_distracted = false;
        cv::Rect m_face_rect;
    };

    class DMSMonitor
    {
    public:
        bool loadModels(const std::string& deploy_path, 
                    const std::string& dnn_face_detect_path, 
                    const std::string& haar_cascade_path);
        DriverState analyze(const cv::Mat& frame);
    private:
        const static size_t m_frameCount = 15;
        const static size_t m_frameLimit = 10;
        std::deque<bool> m_lastFrames;
        cv::dnn::Net m_faceDetector;
        cv::CascadeClassifier m_eyeCascade;

        cv::Rect detectFace(const cv::Mat& frame, float& outConfidence);
        float estimateEyeOpenness(const cv::Mat& faceRoi);
        float estimateHeadTurn(const cv::Rect& faceRect, const cv::Size& frameSize);
    };
}