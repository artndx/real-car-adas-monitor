#include <obd_parser.h>
#include <iostream>
#include <sstream>

namespace obd
{
    bool OBDParser::loadFile(const std::string& dataset_path)
    {
        m_input.open(dataset_path, std::ios_base::in);

        if (!m_input.is_open())
        {
            std::cerr << "Error open file" << std::endl;
            return false;
        }

        std::string header;
        std::getline(m_input, header);

        m_load = true;
        return true;
    }

    int OBDParser::read(OBDRecord& record)
    {
        if (!m_input.is_open())
        {
            std::cerr << "Error open file" << std::endl;
            return -1;
        }

        std::string line;
        if (!std::getline(m_input, line))
        {
            std::cerr << "End of file" << std::endl;
            return -1;
        }

        try
        {
            if (line.empty())
                throw std::string("Empty row error");

            line.erase(line.find_last_not_of("\r\n") + 1);
            auto tokens = split(line);
            if (tokens.size() != 7)
                throw std::string("Invalid row");

            record.m_speed = std::stof(tokens.at(0));
            record.m_engine_rpm = std::stof(tokens.at(1));
            record.m_throttle_position = std::stof(tokens.at(2));
            record.m_coolant_temp = std::stof(tokens.at(3));
            record.m_fuel_level = std::stof(tokens.at(4));
            record.m_intake_air_temp = std::stof(tokens.at(5));
            record.m_label = getLabelType(tokens.at(6));
        }
        catch (const std::string& errMessage)
        {
            std::cerr << errMessage << std::endl;
            return -2;
        }
        catch (const std::exception& exception)
        {
            std::cerr << exception.what() << std::endl;
            return -2;
        }

        return 0;
    }

    int OBDParser::readAll()
    {
        if (!m_input.is_open())
        {
            std::cerr << "Error open file" << std::endl;
            return false;
        }

        m_records.clear();

        std::string line;
        std::size_t row_count = 0;
        while (std::getline(m_input, line))
        {
            try
            {
                if (line.empty())
                    throw std::string("Empty row error");

                line.erase(line.find_last_not_of("\r\n") + 1);
                auto tokens = split(line);
                if (tokens.size() != 7)
                    throw std::string("Invalid row: ") + std::to_string(row_count);

                m_records.emplace_back(
                    std::stof(tokens.at(0)),
                    std::stof(tokens.at(1)),
                    std::stof(tokens.at(2)),
                    std::stof(tokens.at(3)),
                    std::stof(tokens.at(4)),
                    std::stof(tokens.at(5)),
                    getLabelType(tokens.at(6)));
            }
            catch (const std::string& errMessage)
            {
                std::cerr << errMessage << std::endl;
                continue;
            }
            catch (const std::exception& exception)
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

        while (std::getline(ss, token, delimiter))
        {
            tokens.emplace_back(std::move(token));
        }

        return tokens;
    }
}