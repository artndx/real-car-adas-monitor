#include <dms_monitor.h>
#include <env.h>

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
            m_isLoaded = true;
            return true;
        }
        catch (const cv::Exception& e)
        {
            std::cerr << e.what() << std::endl;
            return false;
        }
    }

    bool DMSMonitor::isLoaded() const
    {
        return m_isLoaded;
    }

    DriverState DMSMonitor::analyze(const cv::Mat& frame)
    {
        DriverState state;
        if (!isLoaded())
            throw "Models is not loaded";

        if (frame.empty())
            return state;

        state.m_face_rect = detectFace(frame);

        if (state.m_face_rect.area() <= 0)
            return state;

        state.m_face_detected = true;
#ifdef PRINT_FRAMES
        std::string frame_file = FRAMES_PATH / "frame.png";
        cv::imwrite(frame_file, frame);
        std::string face_file = FRAMES_PATH / "face.png";
        cv::imwrite(face_file, frame(state.m_face_rect));
#endif
        EyesFragment eyes = estimateEyeOpenness(frame, state.m_face_rect);
        float leftOpenness = eyes.first.m_openness;
        float rightOpenness = eyes.second.m_openness;

        float avgOpenness = 0.0f;
        int validCount = 0;

        if (leftOpenness > 0.06f) {
            avgOpenness += leftOpenness;
            validCount++;
        }
        if (rightOpenness > 0.06f) {
            avgOpenness += rightOpenness;
            validCount++;
        }

        if (validCount > 0) {
            avgOpenness /= validCount;
        }

        state.m_eye_openness = avgOpenness;
        state.m_eyes_open = (state.m_eye_openness > 0.5f);
        state.m_head_turn_deg = estimateHeadTurn(state.m_face_rect, frame.size());
        state.m_looking_forward = std::abs(state.m_head_turn_deg) < 15.0f;
        updateEyeHistory(state.m_eyes_open);
        state.m_alert_drowsy = computeDrowsiness();
        state.m_alert_distracted = !state.m_looking_forward;

        return state;
    }

    cv::Rect DMSMonitor::detectFace(const cv::Mat& frame)
    {
        cv::Mat blob = cv::dnn::blobFromImage(frame, 1.0, cv::Size(300, 300), cv::Scalar(104, 177, 123));

        m_faceDetector.setInput(blob);

        cv::Mat detections = m_faceDetector.forward();

        cv::Mat detMat(
            detections.size[2],
            detections.size[3],
            CV_32F,
            detections.ptr<float>());

        float bestConfidence = 0.5f;
        cv::Rect bestFace;

        for (int i = 0; i < detMat.rows; ++i)
        {
            float confidence = detMat.at<float>(i, 2);

            if (confidence < bestConfidence)
                continue;

            int x1 = static_cast<int>(detMat.at<float>(i, 3) * frame.cols);
            int y1 = static_cast<int>(detMat.at<float>(i, 4) * frame.rows);
            int x2 = static_cast<int>(detMat.at<float>(i, 5) * frame.cols);
            int y2 = static_cast<int>(detMat.at<float>(i, 6) * frame.rows);

            cv::Rect rect(cv::Point(x1, y1), cv::Point(x2, y2));

            rect &= cv::Rect(0, 0, frame.cols, frame.rows);

            bestConfidence = confidence;
            bestFace = rect;
        }

        return bestFace;
    }

    EyesFragment DMSMonitor::estimateEyeOpenness(const cv::Mat& frame, const cv::Rect& faceRect)
    {
        EyesFragment result;

        int eyeHeight = static_cast<int>(faceRect.height * 0.4);
        int eyeY = faceRect.y + static_cast<int>(faceRect.height * 0.15);
        cv::Rect eyesRoi(faceRect.x, eyeY, faceRect.width, eyeHeight);
        eyesRoi &= cv::Rect(0, 0, frame.cols, frame.rows);

        if (eyesRoi.area() <= 0)
            return result;

        cv::Mat roiGray;
        cv::cvtColor(frame(eyesRoi), roiGray, cv::COLOR_BGR2GRAY);
        cv::equalizeHist(roiGray, roiGray);

        std::vector<cv::Rect> eyes;
        m_eyeCascade.detectMultiScale(roiGray, eyes, 1.1, 3, 0, cv::Size(20, 20));

        if (eyes.empty())
            return result;


        std::sort(eyes.begin(), eyes.end(),
            [](const cv::Rect& a, const cv::Rect& b) {
                return a.x < b.x;
            });

        size_t numEyes = std::min(eyes.size(), (size_t)2);
        for (size_t i = 0; i < numEyes; ++i) {
            cv::Rect eyeRect = eyes[i];
            float openness = calculateEyeOpenness(roiGray(eyeRect));
#ifdef PRINT_FRAMES
            cv::Mat copy;
            roiGray.copyTo(copy);

            cv::Mat eyeImg = copy(eyeRect).clone();

            cv::putText(
                eyeImg,
                std::format("{:.2f}", openness),
                cv::Point(3, 15),
                cv::FONT_HERSHEY_SIMPLEX,
                0.4,
                cv::Scalar(255),
                1
            );
            cv::imwrite(FRAMES_PATH / "eye.png", eyeImg);
#endif

            if (i == 0)
            {
                result.first.m_openness = openness;
                result.first.m_rect = eyeRect;
            }
            else
            {
                result.second.m_openness = openness;
                result.second.m_rect = eyeRect;
            }
        }

        return result;
    }

    float DMSMonitor::calculateEyeOpenness(const cv::Mat& eyeRoiGray)
    {
        cv::Mat blur;
        cv::GaussianBlur(eyeRoiGray, blur, { 5,5 }, 0);

        cv::Mat bin;
        cv::threshold(
            blur,
            bin,
            0,
            255,
            cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(
            bin,
            contours,
            cv::RETR_EXTERNAL,
            cv::CHAIN_APPROX_SIMPLE);

        if (contours.empty())
            return 0.0f;

        double maxArea = 0.0;
        cv::Rect bestRect;

        for (const auto& c : contours)
        {
            double area = cv::contourArea(c);

            if (area > maxArea)
            {
                maxArea = area;
                bestRect = cv::boundingRect(c);
            }
        }

        float openness =
            static_cast<float>(bestRect.height) /
            static_cast<float>(eyeRoiGray.rows);

        return std::clamp(openness, 0.0f, 1.0f);
    }

    float DMSMonitor::estimateHeadTurn(const cv::Rect& faceRect, const cv::Size& frameSize)
    {
        static float smoothedYaw = 0.0f;

        if (faceRect.area() <= 0 || frameSize.width <= 0)
            return smoothedYaw;

        float faceCenterX = faceRect.x + faceRect.width * 0.5f;
        float frameCenterX = frameSize.width * 0.5f;

        float offset = (faceCenterX - frameCenterX) / frameCenterX;

        if (std::abs(offset) < 0.03f)
            offset = 0.0f;

        float yaw = offset * 35.0f;
        smoothedYaw = 0.85f * smoothedYaw + 0.15f * yaw;

        return smoothedYaw;
    }

    void DMSMonitor::updateEyeHistory(bool eyesOpen)
    {
        m_lastFrames.emplace_back(eyesOpen);

        while (m_lastFrames.size() > m_frameCount)
        {
            m_lastFrames.pop_front();
        }
    }

    bool DMSMonitor::computeDrowsiness() const
    {
        int closedFrames = 0;

        for (bool open : m_lastFrames)
        {
            if (!open)
                ++closedFrames;
        }

        return closedFrames >= m_frameLimit;
    }
}