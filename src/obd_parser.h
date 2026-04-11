#pragma once
#include <vector>
#include <ostream>
#include <string_view>

namespace obd
{
    enum class LabelType
    {
        NONE,
        SLOW,
        NORMAL,
        AGGRESSIVE
    };

    inline std::ostream& operator<<(std::ostream& out, LabelType type)
    {
        switch (type)
        {
            case LabelType::NONE:      out << "NONE"; break;
            case LabelType::SLOW:      out << "SLOW"; break;
            case LabelType::NORMAL:    out << "NORMAL"; break;
            case LabelType::AGGRESSIVE:out << "AGGRESSIVE"; break;
            default:                   out << "UNKNOWN"; break;
        }
        return out;
    }

    LabelType getLabelType(std::string_view token);
    
    struct OBDRecord
    {
        OBDRecord(size_t id, double speed, double engine_rpm, 
                double throttle_pos, double cool_temp, 
                double fuel_level, double inake_air_temp,
                LabelType label)
                : m_id(id), m_speed(speed), m_engine_rpm(engine_rpm), 
                m_throttle_position(throttle_pos), m_coolant_temp(cool_temp), 
                m_fuel_level(fuel_level), m_intake_air_temp(inake_air_temp), 
                m_label(label){}
        
        size_t m_id = 0;
        float m_speed = 0.0;
        float m_engine_rpm = 0.0;
        float m_throttle_position = 0.0;

        float m_coolant_temp = 0.0;
        float m_fuel_level = 0.0;
        float m_intake_air_temp = 0.0;

        LabelType m_label = LabelType::SLOW;
    };

    inline std::ostream& operator<<(std::ostream& out, const OBDRecord& r)
    {
        out << "{ speed_kmh: " << r.m_speed 
        << ", engine_rpm: " << r.m_engine_rpm
        << ", throttle_pos: " << r.m_throttle_position
        << ", coolant_temp: " << r.m_coolant_temp
        << ", fuel_level: " << r.m_fuel_level
        << ", intake_air_temp: " << r.m_intake_air_temp
        << ", label: " << r.m_label << " }";
        return out;
    }

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