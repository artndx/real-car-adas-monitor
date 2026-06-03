#pragma once
#include <onnxruntime_cxx_api.h>
#include <common.h>

namespace onnx
{
    template<std::size_t S>
    using ArrayF = std::array<float, S>;
    using Labels = std::array<LabelType, 3>;

    /**
     * @brief Результат классификации стиля вождения.
     *
     * Содержит итоговую метку, степень уверенности и сырые оценки по классам.
     */
    struct ClassificationResult
    {
        LabelType m_label;
        float m_confidence = 0.0;
        ArrayF<3> m_scores;
    };

    /**
     * @brief Параметры нормализации (среднее и стандартное отклонение) для одного признака.
     */
    struct NormalizeParam
    {
        NormalizeParam(float mean, float std)
            : m_mean(mean), m_std(std) {
        }

        float m_mean = 0.0;
        float m_std = 0.0;
    };

    /**
     * @brief Обёртка над ONNX Runtime для классификации стиля вождения.
     *
     * Загружает модель и параметры нормализации, выполняет инференс
     * на наборе OBD-признаков.
     */
    class ONNXClassifier
    {
    public:
        ONNXClassifier();

        /**
         * @brief Импортирует ONNX-модель из файла.
         *
         * @param model_path Путь к .onnx файлу модели.
         * @return Код результата (0 при успехе, ненулевое значение при ошибке).
         */
        int loadModel(const std::string& model_path);

        /**
         * @brief Импортирует параметры нормализации из JSON-файла.
         *
         * @param json_path Путь к JSON-файлу со средними и СКО по каждому признаку.
         * @return Код результата (0 при успехе, ненулевое значение при ошибке).
         */
        int loadJson(const std::string& json_path);

        /**
         * @brief Классифицирует набор из 6 признаков.
         *
         * @param features Массив из 6 нормализованных признаков.
         * @return Результат классификации (метка, уверенность, оценки).
         */
        ClassificationResult classify(const ArrayF<6>& features) const;

        /**
         * @brief Классифицирует стиль вождения по OBD-записи.
         *
         * Извлекает признаки из OBDRecord, применяет нормализацию и
         * передаёт их в модель.
         *
         * @param record OBD-запись с телеметрией.
         * @return Результат классификации (метка, уверенность, оценки).
         */
        ClassificationResult classify(const OBDRecord& record) const;
    private:
        std::unique_ptr<Ort::Env> m_env;
        std::unique_ptr<Ort::Session> m_current_session;

        ArrayF<6> m_mean{};                     // средние значения признаков
        ArrayF<6> m_std{};                      // стандартные отклонения
        Labels m_labels;

        /**
         * @brief Применяет softmax к вектору логитов.
         *
         * @param logits Вектор логитов.
         * @return Вектор вероятностей после softmax.
         */
        static ArrayF<3> softmax(const ArrayF<3>& logits);
    };
}