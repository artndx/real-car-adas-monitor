#pragma once
#include <ostream>
#include <string_view>
#include <chrono>

/**
 * @brief Перечисление типов стиля вождения.
 *
 * Используется для маркировки телеметрических записей и результатов классификации.
 */
enum class LabelType
{
    NONE,
    SLOW,
    NORMAL,
    AGGRESSIVE
};

/**
 * @brief Выводит строковое представление типа стиля вождения в поток.
 *
 * @param out Выходной поток.
 * @param type Тип стиля вождения.
 * @return Ссылка на выходной поток.
 */
std::ostream& operator<<(std::ostream& out, LabelType type);

/**
 * @brief Преобразует строковый токен в тип стиля вождения.
 *
 * @param token Строковое представление метки.
 * @return Соответствующий LabelType.
 */
LabelType getLabelType(std::string_view token);

/**
 * @brief Возвращает строковое представление типа стиля вождения.
 *
 * @param label_type Тип стиля вождения.
 * @return Строковое имя типа.
 */
std::string getLabelTypeStr(LabelType label_type);

/**
 * @brief OBD-запись из CSV файла
 *
 * Содержит телеметрические параметры автомобиля и метку стиля вождения.
 */
struct OBDRecord
{
    OBDRecord() = default;
    OBDRecord(double speed, double engine_rpm,
        double throttle_pos, double cool_temp,
        double fuel_level, double inake_air_temp,
        LabelType label)
        : m_speed(speed), m_engine_rpm(engine_rpm),
        m_throttle_position(throttle_pos), m_coolant_temp(cool_temp),
        m_fuel_level(fuel_level), m_intake_air_temp(inake_air_temp),
        m_label(label) {
    }

    float m_speed = 0.0;
    float m_engine_rpm = 0.0;
    float m_throttle_position = 0.0;

    float m_coolant_temp = 0.0;
    float m_fuel_level = 0.0;
    float m_intake_air_temp = 0.0;

    LabelType m_label = LabelType::SLOW;
};

/**
 * @brief Выводит содержимое OBD-записи в поток.
 *
 * @param out Выходной поток.
 * @param r Ссылка на OBD-запись.
 * @return Ссылка на выходной поток.
 */
std::ostream& operator<<(std::ostream& out, const OBDRecord& r);

/**
 * @brief Возвращает текущее системное время в виде строки.
 *
 * @return Строковое представление текущего времени.
 */
std::string getCurrentTime();