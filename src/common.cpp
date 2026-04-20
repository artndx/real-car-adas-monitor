#include <common.h>

std::ostream& operator<<(std::ostream& out, LabelType type)
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

std::string getLabelTypeStr(LabelType label_type)
{
    switch (label_type) 
    {
        case LabelType::SLOW: 
            return "SLOW";
        case LabelType::NORMAL: 
            return "NORMAL";
        case LabelType::AGGRESSIVE: 
            return "AGGRESSIVE";
    }
     
    return "NONE";
}

std::ostream& operator<<(std::ostream& out, const OBDRecord& r)
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