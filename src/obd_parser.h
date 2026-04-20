#pragma once
#include <vector>
#include <common.h>

namespace obd
{
    using OBDRecords = std::vector<OBDRecord>;

    class OBDParser
    {
    private:
        OBDRecords m_records;
    public:
        int load(const std::string& dataset_path);
        OBDRecord& getRecord(size_t index);
        const OBDRecord& getRecord(size_t index) const;
    private:
        std::vector<std::string> split(std::string_view line, char delimiter = ',');
    }; 
}