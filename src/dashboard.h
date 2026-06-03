#pragma once
#include <opencv2/opencv.hpp>
#include <common.h>
#include <string>

namespace dashboard
{
    /**
     * @brief Параметры для отрисовки одного датчика (gauge).
     *
     * Содержит минимальное/максимальное/текущее значения, порог предупреждения,
     * подпись и единицу измерения.
     */
    struct GaugeParams
    {
        float m_min_val;
        float m_max_val;
        float m_current_val;
        float m_warning_limit;
        std::string label;
        std::string unit;
    };

    /**
     * @brief Класс для отрисовки приборной панели (дашборда) на кадре.
     *
     * Использует данные OBDRecord для визуализации текущего состояния
     * автомобиля в виде круговых и линейных индикаторов.
     */
    class Dashboard {
    public:
        Dashboard() = default;

        /**
         * @brief Отрисовывает приборную панель на переданном кадре.
         *
         * @param frame Кадр OpenCV, на который будет наложена панель.
         * @param record OBD-запись с текущими значениями телеметрии.
         */
        void draw(cv::Mat& frame, const OBDRecord& record);
    private:
        /**
         * @brief Рисует круговой индикатор с цветовой индикацией предупреждения.
         *
         * @param frame Кадр для отрисовки.
         * @param center Центр индикатора.
         * @param radius Радиус индикатора.
         * @param params Параметры индикатора.
         * @param warning_threshold Порог предупреждения (-1.0f — использовать params.m_warning_limit).
         */
        void drawGauge(cv::Mat& frame, const cv::Point& center, int radius,
            const GaugeParams& params, float warning_threshold = -1.0f);

        /**
         * @brief Рисует линейный индикатор.
         *
         * @param frame Кадр для отрисовки.
         * @param start Начальная точка индикатора.
         * @param size Размер индикатора.
         * @param params Параметры индикатора.
         * @param warning_threshold Порог предупреждения (-1.0f — использовать params.m_warning_limit).
         * @param warn_below Если true — предупреждение срабатывает при значении ниже порога.
         */
        void drawLinearGauge(cv::Mat& frame, const cv::Point& start, const cv::Size& size,
            const GaugeParams& params, float warning_threshold = -1.0f, bool warn_below = false);

        /**
         * @brief Рисует предупреждающее сообщение на кадре.
         *
         * @param img Кадр для отрисовки.
         * @param msg Текст предупреждения.
         * @param pos Позиция отрисовки текста.
         */
        void drawWarning(cv::Mat& img, const std::string& msg, const cv::Point& pos);
    };
}