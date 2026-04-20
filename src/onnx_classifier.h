#pragma once
#include <onnxruntime_cxx_api.h>
#include <common.h>

namespace onnx
{   
    template<std::size_t S>
    using ArrayF = std::array<float, S>;
    using Labels = std::array<LabelType, 3>;

    struct ClassificationResult
    {
        LabelType m_label;
        float m_confidence = 0.0;
        ArrayF<3> m_scores;
    };

    struct NormalizeParam
    {
        NormalizeParam(float mean, float std)
        : m_mean(mean), m_std(std){}

        float m_mean = 0.0;
        float m_std = 0.0;
    };
    
    class ONNXClassifier
    {
    public:
        ONNXClassifier();

        // импорт модели
        int loadModel(const std::string& model_path);
        // импорт параметров из json
        int loadJson(const std::string& json_path);
        // классификация одного набора признаков
        ClassificationResult classify(const ArrayF<6>& features) const;
    private:
        std::unique_ptr<Ort::Env> m_env;
        std::unique_ptr<Ort::Session> m_current_session;

        ArrayF<6> m_mean{};                     // средние значения признаков
        ArrayF<6> m_std{};                      // стандартные отклонения
        Labels m_labels; 

        static ArrayF<3> softmax(const ArrayF<3>& logits);
    };
}