#include <onnx_classifier.h>
#include <iostream>
#include <fstream>
#include <sstream>

namespace onnx
{
    ONNXClassifier::ONNXClassifier()
    : m_env(std::make_unique<Ort::Env>()){}

    int ONNXClassifier::loadModel(const std::string& model_path)
    {
        try
        {
            Ort::SessionOptions session_options;
            session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

            m_currentSession = std::make_unique<Ort::Session>(*m_env, model_path.c_str(),session_options);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            return -1;
        }

        return 0;
    }

    int ONNXClassifier::loadJson(const std::string& json_path)
    {
        std::ifstream file(json_path);
        if (!file.is_open()) {
            std::cerr << "Cannot open JSON file: " << json_path << std::endl;
            return -1;
        }

        std::string content{
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()
        };

        auto extractFloat = [&](const std::string& key, ArrayF<6>& out)
        {
            std::string search = "\"" + key + "\"";
            size_t pos = content.find(search);
            if (pos == std::string::npos) return -1;
            
            size_t open  = content.find('[', pos);
            size_t close = content.find(']', open);
            if (open == std::string::npos || close == std::string::npos) return -1;

            std::istringstream iss(content.substr(open + 1, close - open - 1));
            for (size_t i = 0; i < out.size(); ++i) {
                char sep;
                if (!(iss >> out[i])) 
                    return -1;
                if (i < out.size() - 1) 
                    iss >> sep;
            }
            return 0;
        };

        auto extractLabels = [&](const std::string& key, Labels& out)
        {
            std::string search = "\"" + key + "\"";
            size_t pos = content.find(search);
            if (pos == std::string::npos) return -1;
            
            size_t open  = content.find('[', pos);
            size_t close = content.find(']', open);
            if (open == std::string::npos || close == std::string::npos) return -1;

            std::istringstream iss(content.substr(open + 1, close - open - 1));
            for (size_t i = 0; i < out.size(); ++i) {
                std::string token;
                char sep;
                iss >> token;
                if (token.empty()) 
                    return -1;
                
                if (token.front() == '"') 
                    token.erase(token.begin());
                if (token.back() == ',') 
                    token.pop_back();
                if (token.back() == '"') 
                    token.pop_back();
                
                out[i] = obd::getLabelType(token);
            }
            return 0;
        };

        if (extractFloat("mean", m_mean) != 0) 
        {
            std::cerr << "Failed to parse 'mean'" << std::endl; 
            return -1;
        }

        if (extractFloat("std", m_std) != 0)
        {
            std::cerr << "Failed to parse 'std'" << std::endl; 
            return -1;
        }

        if (extractLabels("classes", m_labels) != 0) 
        {
            std::cerr << "Failed to parse 'classes'" << std::endl; 
            return -1;
        }

        return 0;
    }

    ClassificationResult ONNXClassifier::classify(const ArrayF<6>& features) const
    {
        // случай когда модель не загружена – бросить std::runtime_error. 
        if (!m_currentSession) 
            throw std::runtime_error("ONNX model not loaded");

        // нормализовать их (z-score: (x - mean) / std); 
        auto normalized = features;
        for (size_t i = 0; i < features.size(); ++i) {
            float std = ((fabs(m_std[i]) < 1e-6) ? 1.0 : m_std[i]);
            normalized[i] = (features[i] - m_mean[i]) / std;
        }

        Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
        std::array<int64_t, 2> input_shape = {1, 6};

        Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
            memory_info,
            normalized.data(),
            normalized.size(),
            input_shape.data(),
            input_shape.size()
        );

        const char* input_names[] = {"features"};
        const char* output_names[] = {"class_scores"};

        std::vector<Ort::Value> output_tensors = m_currentSession->Run(
            Ort::RunOptions{nullptr},
            input_names,
            &input_tensor,
            1,
            output_names,
            1
        );

        float* logits_ptr = output_tensors[0].GetTensorMutableData<float>();
        ArrayF<3> logits = {logits_ptr[0], logits_ptr[1], logits_ptr[2]};
        ArrayF<3> probs = softmax(logits);

        size_t max_prob_index = std::distance(probs.begin(), std::max_element(probs.begin(), probs.end()));

        return ClassificationResult{m_labels.at(max_prob_index), probs.at(max_prob_index), probs};
    }

    ArrayF<3> ONNXClassifier::softmax(const ArrayF<3>& logits)
    {
        float max_val = *std::max_element(logits.begin(), logits.end());
        
        ArrayF<3> probs;
        float sum = 0.0f;
        
        for (size_t i = 0; i < 3; ++i) 
        {
            probs[i] = std::exp(logits[i] - max_val);
            sum += probs[i];
        }
        
        for (auto& prob : probs) 
            prob /= sum;

        return probs;
    }
}
