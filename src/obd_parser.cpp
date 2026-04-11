#include <obd_parser.h>
#include <iostream>
#include <fstream>
#include <sstream>

namespace obd
{
    LabelType getLabelType(std::string_view token)
    {
        if(token == "SLOW")
            return LabelType::SLOW;
        else if(token == "NORMAL")
            return LabelType::NORMAL;
        else if(token == "AGGRESSIVE")
            return LabelType::AGGRESSIVE;

        return LabelType::NONE;
    }

    int OBDParser::load(const std::string& dataset_path)
    {
        std::ifstream in(dataset_path, std::ios_base::in);
        if(!in.is_open())
        {
            std::cerr << "Error open file" << std::endl;
            return -1;
        }

        m_records.clear();

        std::string line;
        if(!std::getline(in, line))
        {
            std::cerr << R"(
            CSV Header matching error.
            Expects:
            speed_kmh, engine_rpm, throttle_pos, coolant_temp, fuel_level, intake_air_temp, label)"  << std::endl;
            return -1;
        }

        std::size_t row_count = 0;
        while(std::getline(in, line))
        {
            try
            {
                if(line.empty())
                    throw std::string("Empty row error");

                line.erase(line.find_last_not_of("\r\n") + 1);
                auto tokens = split(line);
                if (tokens.size() != 7)
                    throw std::string("Invalid row: ") + std::to_string(row_count);
                    
                m_records.emplace_back(
                    row_count,
                    std::stof(tokens.at(0)),
                    std::stof(tokens.at(1)),
                    std::stof(tokens.at(2)),
                    std::stof(tokens.at(3)),
                    std::stof(tokens.at(4)),
                    std::stof(tokens.at(5)),
                    getLabelType(tokens.at(6))
                );
            }
            catch(const std::string& errMessage)
            {
                std::cerr << errMessage << std::endl;
                continue;
            }
            catch(const std::exception& exception)
            {
                std::cerr << exception.what() << std::endl;
                continue;
            }

            ++row_count;
        }

        return row_count;
    }

    OBDRecord& OBDParser::getRecord(size_t index)
    {
        return m_records.at(index);
    }

    const OBDRecord& OBDParser::getRecord(size_t index) const
    {
        return m_records.at(index);
    }

    std::vector<std::string> OBDParser::split(std::string_view line, char delimiter)
    {
        std::vector<std::string> tokens;

        std::string token;
        std::stringstream ss(line.data());

        while (std::getline(ss, token, delimiter)) {
            tokens.emplace_back(std::move(token));
        }

        return tokens;
    }
}