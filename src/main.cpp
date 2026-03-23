#include <iostream>
#include <obd_parser.h>
#include <env.h>

int main() {
    obd::OBDParser parser;
    int row_count = parser.load(DATASET);
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