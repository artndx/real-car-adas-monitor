#pragma once
#include <opencv2/opencv.hpp>

namespace dms
{
    /**
     * @brief Состояние водителя, полученное в результате анализа кадра.
     *
     * Содержит признаки обнаружения лица, состояния глаз, направления взгляда,
     * а также сработавшие предупреждения и координаты лица.
     */
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

    /**
     * @brief Прямоугольник глаза и оценка его открытости.
     */
    struct EyeRect
    {
        float m_openness = 0.0f;
        cv::Rect m_rect;
    };

    using EyesFragment = std::pair<EyeRect, EyeRect>;

    /**
     * @brief Монитор состояния водителя (Driver Monitoring System).
     *
     * Выполняет детекцию лица, оценку открытости глаз и направления взгляда,
     * а также формирует предупреждения о сонливости и отвлечении.
     */
    class DMSMonitor
    {
    public:
        /**
         * @brief Загружает модели детекции лица и глаз.
         *
         * @param deploy_path Путь к prototxt-файлу с описанием DNN-модели.
         * @param dnn_face_detect_path Путь к бинарным весам DNN-модели детекции лица.
         * @param haar_cascade_path Путь к XML-файлу каскада Хаара для глаз.
         * @return true в случае успешной загрузки всех моделей, false при ошибке.
         */
        bool loadModels(const std::string& deploy_path,
            const std::string& dnn_face_detect_path,
            const std::string& haar_cascade_path);

        /**
         * @brief Проверяет, загружены ли модели монитора.
         *
         * @return true, если модели загружены и монитор готов к работе.
         */
        bool isLoaded() const;

        /**
         * @brief Анализирует кадр и возвращает текущее состояние водителя.
         *
         * @param frame Кадр OpenCV (BGR) с камеры, направленной на водителя.
         * @return Структура DriverState с результатами анализа.
         */
        DriverState analyze(const cv::Mat& frame);
    private:
        bool m_isLoaded = false;
        const static size_t m_frameCount = 15;
        const static size_t m_frameLimit = 10;
        std::deque<bool> m_lastFrames;
        cv::dnn::Net m_faceDetector;
        cv::CascadeClassifier m_eyeCascade;

        /**
         * @brief Обнаруживает лицо на кадре.
         *
         * @param frame Кадр OpenCV (BGR).
         * @return Прямоугольник, ограничивающий найденное лицо.
         */
        cv::Rect detectFace(const cv::Mat& frame);

        /**
         * @brief Оценивает степень открытости обоих глаз в пределах области лица.
         *
         * @param frame Кадр OpenCV (BGR).
         * @param faceRect Прямоугольник с областью лица.
         * @return Пара структур EyeRect для левого и правого глаза.
         */
        EyesFragment estimateEyeOpenness(const cv::Mat& frame, const cv::Rect& faceRect);

        /**
         * @brief Вычисляет числовую оценку открытости глаза по его ROI в градациях серого.
         *
         * @param eyeRoiGray Изображение области глаза в градациях серого.
         * @return Оценка открытости глаза.
         */
        float calculateEyeOpenness(const cv::Mat& eyeRoiGray);

        /**
         * @brief Оценивает угол поворота головы на основе положения лица в кадре.
         *
         * @param faceRect Прямоугольник с областью лица.
         * @param frameSize Размер кадра.
         * @return Оценка угла поворота головы в градусах.
         */
        float estimateHeadTurn(const cv::Rect& faceRect, const cv::Size& frameSize);

        /**
         * @brief Обновляет историю состояния глаз для временного сглаживания.
         *
         * @param eyesOpen Текущее состояние глаз (true — открыты).
         */
        void updateEyeHistory(bool eyesOpen);

        /**
         * @brief Вычисляет факт срабатывания предупреждения о сонливости.
         *
         * @return true, если сонливость подтверждена по истории кадров.
         */
        bool computeDrowsiness() const;
    };
}