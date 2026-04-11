#include <iostream>
#include <env.h>
#include <fstream>
#include <obd_parser.h>
#include <onnx_classifier.h>

void obdParsing()
{
    obd::OBDParser parser;
    int row_count = parser.load(DATASET_PATH);
    size_t slowLabelCount = 0;
    size_t normalLabelCount = 0;
    size_t aggressiveLabelCount = 0;
    for(size_t i = 0; i < row_count; ++i)
    {
        obd::OBDRecord& record = parser.getRecord(i);
        if(record.m_label == obd::LabelType::SLOW)
            slowLabelCount++;
        else if(record.m_label == obd::LabelType::NORMAL)
            normalLabelCount++;
        else if(record.m_label == obd::LabelType::AGGRESSIVE)
            aggressiveLabelCount++;
    }

    std::cout << "{ SLOW : " << slowLabelCount << ", NORMAL : " << normalLabelCount << ", " << "AGGRESSIVE : " << aggressiveLabelCount << " }" << std::endl;
    for(size_t i = 0; i < 5; ++i)
    {
        std::cout << parser.getRecord(i) << std::endl;
    }
}

int main() {  
    onnx::ONNXClassifier classifier;
    
    if (classifier.loadModel(MODEL_PATH) != 0) {
        std::cerr << "Failed to load model\n";
        return 1;
    }
    if (classifier.loadJson(NORM_PARAMS_PATH) != 0) {
        std::cerr << "Failed to load JSON params\n";
        return 1;
    }
    
    // Заголовок таблицы
    std::cout << std::left 
              << std::setw(12) << "Истинная "
              << std::setw(14) << "Предсказанная "
              << std::setw(12) << "Уверенность " << "\n";
    std::cout << std::string(38, '-') << "\n";

    size_t correct = 0;

    obd::OBDParser parser;
    int row_count = parser.load(DATASET_PATH);
    for(size_t i = 2320; i < std::min(row_count, 2320+20); ++i)
    {
        obd::OBDRecord& record = parser.getRecord(i);
        onnx::ArrayF<6> features = {
            record.m_speed,
            record.m_engine_rpm,
            record.m_throttle_position,
            record.m_coolant_temp,
            record.m_fuel_level,
            record.m_intake_air_temp
        };

        auto result = classifier.classify(features);
        
        obd::LabelType true_label = record.m_label;
        obd::LabelType prediction_label = result.m_label;

        if (true_label == prediction_label)
            ++correct;
        
        std::cout << std::left
                  << std::setw(12) << true_label
                  << std::setw(14) << prediction_label
                  << std::fixed << std::setprecision(3) << result.m_confidence << "\n";
    }

    double accuracy = (static_cast<double>(correct) / 20) * 100.0;
    std::cout << "\nAccuracy: " << std::fixed << std::setprecision(1) << accuracy << "%\n";
    std::cout << "Требование: >= 80%\n";
    
    return 0;
}