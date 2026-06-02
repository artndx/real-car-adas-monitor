#pragma once
#include <vector>
#include <common.h>
#include <fstream>

namespace obd
{
    using OBDRecords = std::vector<OBDRecord>;

    class OBDParser
    {
    private:
        std::ifstream m_input;
        bool m_load = false;
        OBDRecords m_records;

    public:
        bool loadFile(const std::string& dataset_path);
        int read(OBDRecord& record);
        int readAll();
        OBDRecord& getRecord(size_t index);
        const OBDRecord& getRecord(size_t index) const;

    private:
        std::vector<std::string> split(std::string_view line, char delimiter = ',');
    };
}