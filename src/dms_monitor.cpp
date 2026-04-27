#include <dms_monitor.h>

namespace dms
{
    bool DMSMonitor::loadModels(const std::string& deploy_path, 
                    const std::string& dnn_face_detect_path, 
                    const std::string& haar_cascade_path)
    {
        try 
        {
            m_faceDetector = cv::dnn::readNetFromCaffe(deploy_path, dnn_face_detect_path);
            if (m_faceDetector.empty()) 
            {
                std::cerr << std::format("Error loading deploy file: {} )", deploy_path) << std::endl;
                std::cerr << std::format("Error loading dnn model file: {} )", dnn_face_detect_path) << std::endl;
                return false;
            }

            m_faceDetector.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
            m_faceDetector.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

            if (!m_eyeCascade.load(haar_cascade_path)) 
            {
                std::cerr << std::format("Error loading haar cascade file: {} )", haar_cascade_path) << std::endl;
                return false;
            }

            m_lastFrames.clear();
            return true;
        }
        catch (const cv::Exception& e) 
        {
            std::cerr << e.what() << std::endl;
            return false;
        }
    }

    DriverState DMSMonitor::analyze(const cv::Mat& frame)
    {
        DriverState state;

        float faceConfidence = 0.0f;
        state.m_face_rect = detectFace(frame, faceConfidence);
        state.m_face_detected = (state.m_face_rect.area() > 0 && faceConfidence >= 0.5f);

        if (!state.m_face_detected) 
            return state;

        cv::Mat faceRoi = frame(state.m_face_rect);
        state.m_eye_openness = estimateEyeOpenness(faceRoi);
        state.m_eyes_open = (state.m_eye_openness >= 0.3f);  // Порог открытости

        state.m_head_turn_deg = estimateHeadTurn(state.m_face_rect, frame.size());
        state.m_looking_forward = (std::abs(state.m_head_turn_deg) <= 30.0f);  // ±30° = "прямо"

        m_lastFrames.push_back(!state.m_eyes_open);
        if (m_lastFrames.size() > m_frameCount) 
            m_lastFrames.pop_front();
        
        if (m_lastFrames.size() == m_frameCount) 
        {
            const size_t closedCount = std::count(m_lastFrames.begin(), m_lastFrames.end(), true);
            
            state.m_alert_drowsy = (closedCount >= m_frameLimit);
        }

        state.m_alert_distracted = (!state.m_looking_forward || !state.m_face_detected);

        return state;
    }

    cv::Rect DMSMonitor::detectFace(const cv::Mat& frame, float& outConfidence)
    {
        cv::Mat blob = cv::dnn::blobFromImage(frame, 
                                              1.0f, 
                                              cv::Size(300, 300),
                                              cv::Scalar(104.0, 177.0, 123.0),
                                              false,
                                              false);

        m_faceDetector.setInput(blob);
        cv::Mat detections = m_faceDetector.forward();

        cv::Rect bestRect;
        const int numDetections = detections.size[2];
        
        float bestConfidence = std::numeric_limits<float>::min();
        for (int i = 0; i < numDetections; ++i) 
        {
            const float* det = detections.ptr<float>(0, 0, i);
            float confidence = det[2];
            
            if (confidence > 0.5f && confidence >= bestConfidence) 
            {
                int x1 = static_cast<int>(det[3] * frame.cols);
                int y1 = static_cast<int>(det[4] * frame.rows);
                int x2 = static_cast<int>(det[5] * frame.cols);
                int y2 = static_cast<int>(det[6] * frame.rows);

                cv::Rect rect(x1, y1, x2 - x1, y2 - y1);

                bool validSize = (rect.width > 30 && rect.height > 30);
                bool inBounds = (rect.x >= 0 && rect.y >= 0 && 
                                rect.x + rect.width <= frame.cols && 
                                rect.y + rect.height <= frame.rows);

                if (validSize && inBounds) 
                {
                    bestRect = rect;
                    bestConfidence = confidence;
                }
            }
        }

        outConfidence = bestConfidence;
        return bestRect;
    }

    float DMSMonitor::estimateEyeOpenness(const cv::Mat& faceRoi)
    {
        if (faceRoi.empty()) 
            return 0.0f;

        cv::Rect eyesRoi(0, 0, faceRoi.cols, faceRoi.rows / 2);
        cv::Mat eyesRegion = faceRoi(eyesRoi);
        
        std::vector<cv::Rect> eyes;
        m_eyeCascade.detectMultiScale(eyesRegion, eyes, 1.1, 5, 
                                      cv::CASCADE_SCALE_IMAGE,
                                      cv::Size(20, 20));

        if (eyes.empty()) 
            return 0.0f;

        auto largestEye = std::max_element(eyes.begin(), eyes.end(),
            [](const cv::Rect& a, const cv::Rect& b) { 
                return a.area() < b.area(); 
            });

        cv::Mat eyeImg = eyesRegion(*largestEye);
        
        cv::Mat gray, blurred, thresh;
        cv::cvtColor(eyeImg, gray, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 0);
        cv::threshold(blurred, thresh, 0, 255, cv::THRESH_BINARY_INV + cv::THRESH_OTSU);

        const float darkRatio = static_cast<float>(cv::countNonZero(thresh)) 
                              / static_cast<float>(thresh.total());

        float openness = 1.0f - std::clamp(darkRatio * 1.5f, 0.0f, 1.0f);
        
        return openness;
    }

    float DMSMonitor::estimateHeadTurn(const cv::Rect& faceRect, const cv::Size& frameSize)
    {
        const float faceCenterX = faceRect.x + faceRect.width / 2.0f;
        const float frameCenterX = frameSize.width / 2.0f;

        const float normalizedOffset = (faceCenterX - frameCenterX) / (frameSize.width / 2.0f);
        
        return std::clamp(normalizedOffset * 90.0f, -90.0f, 90.0f);
    }
}