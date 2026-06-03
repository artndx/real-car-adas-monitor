#pragma once
#include <dms_monitor.h>

namespace dms
{
    /**
     * @brief HUD (head-up display) подсистемы мониторинга состояния водителя (DMS).
     *
     * Отвечает за наложение графической информации о состоянии водителя
     * (усталость, отвлечение, положение лица) на видеокадр.
     */
    class DMSHUD
    {
    public:
        /**
         * @brief Отрисовывает HUD поверх целевого кадра.
         *
         * @param targetFrame Кадр, на который будет наложен HUD.
         * @param cameraFrame Исходный кадр с камеры (для вспомогательной отрисовки).
         * @param state Текущее состояние водителя, полученное от DMSMonitor.
         */
        void draw(cv::Mat& targetFrame, const cv::Mat& cameraFrame, const DriverState& state);
    private:
        /**
         * @brief Рисует углы рамки вокруг обнаруженного лица.
         *
         * @param frame Кадр для отрисовки.
         * @param rect Прямоугольник, ограничивающий лицо.
         * @param color Цвет углов в формате OpenCV Scalar.
         */
        void drawFaceCorners(cv::Mat& frame, const cv::Rect& rect, const cv::Scalar& color);

        /**
         * @brief Рисует строку статуса с индикатором состояния.
         *
         * @param frame Кадр для отрисовки.
         * @param y Вертикальная координата строки.
         * @param text Текст подписи статуса.
         * @param value Логическое значение состояния (true — зелёный, false — красный).
         */
        void drawStatusLine(cv::Mat& frame, int y, const std::string& text, bool value);

        /**
         * @brief Рисует предупреждение о сонливости водителя.
         *
         * @param frame Кадр для отрисовки.
         */
        void drawDrowsinessAlert(cv::Mat& frame);

        /**
         * @brief Рисует предупреждение об отвлечении внимания водителя.
         *
         * @param frame Кадр для отрисовки.
         */
        void drawDistractionAlert(cv::Mat& frame);
    };
}