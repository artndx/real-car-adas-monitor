#pragma once
#include <ostream>
#include <string_view>

enum class LabelType
{
    NONE,
    SLOW,
    NORMAL,
    AGGRESSIVE
};

std::ostream& operator<<(std::ostream& out, LabelType type);

LabelType getLabelType(std::string_view token);

std::string getLabelTypeStr(LabelType label_type);

struct OBDRecord
{
    OBDRecord() = default;
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

std::ostream& operator<<(std::ostream& out, const OBDRecord& r);