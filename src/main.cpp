#include <iostream>
#include <env.h>
#include <fstream>
#include <obd_parser.h>
#include <onnx_classifier.h>
#include <dashboard.h>

void test_obdParsing()
{
    obd::OBDParser parser;
    int row_count = parser.load(DATASET_PATH);
    size_t slow_count = 0;
    size_t normal_count = 0;
    size_t aggres_count = 0;
    for(size_t i = 0; i < row_count; ++i)
    {
        OBDRecord& record = parser.getRecord(i);
        if(record.m_label == LabelType::SLOW)
            slow_count++;
        else if(record.m_label == LabelType::NORMAL)
            normal_count++;
        else if(record.m_label == LabelType::AGGRESSIVE)
            aggres_count++;
    }

    std::cout << "{ SLOW : " << slow_count << ", NORMAL : " << normal_count << ", " << "AGGRESSIVE : " << aggres_count << " }" << std::endl;
    for(size_t i = 0; i < 5; ++i)
    {
        std::cout << parser.getRecord(i) << std::endl;
    }
}

int test_onnxClassifier() 
{  
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
        OBDRecord& record = parser.getRecord(i);
        onnx::ArrayF<6> features = {
            record.m_speed,
            record.m_engine_rpm,
            record.m_throttle_position,
            record.m_coolant_temp,
            record.m_fuel_level,
            record.m_intake_air_temp
        };

        auto result = classifier.classify(features);
        
        LabelType true_label = record.m_label;
        LabelType prediction_label = result.m_label;

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

int test_dashboard() 
{
    using namespace dashboard;

    try 
    {
        Dashboard dash;

        obd::OBDParser parser;
        int row_count = parser.load(DATASET_PATH);
        size_t row_idx = 1000;

        std::cout << "Press any key to exit..." << std::endl;
        while(cv::waitKey(150) == -1)
        {
            if(row_idx >= row_count)
                break;

            OBDRecord record = parser.getRecord(row_idx++);
            cv::Mat frame = dash.draw(record);
            cv::imshow("Dashboard Test", frame);
        }

        cv::destroyAllWindows();
    
        return 0;
    } catch (const std::exception& e) 
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

int main() {
    return test_dashboard();
}