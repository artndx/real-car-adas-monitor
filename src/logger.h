#pragma once

#include <iostream>
#include <fstream>

/**
 * @brief Простой логгер, пишущий сообщения в файл с добавлением временной метки.
 *
 * Сообщения отправляются через оператор <<. Если файл не открыт, выводит
 * сообщение об ошибке в std::cerr и игнорирует запись.
 */
class Logger
{
    std::ofstream m_output;
public:
    /**
     * @brief Открывает файл лога в режиме дозаписи (append).
     *
     * @param log_file Путь к файлу лога.
     * @return true, если файл удалось открыть.
     */
    bool open(const std::string& log_file)
    {
        m_output.open(log_file, std::ios::app);
        return m_output.is_open();
    }

    /**
     * @brief Закрывает файл лога, если он открыт.
     */
    ~Logger()
    {
        if (m_output.is_open())
            m_output.close();
    }

    /**
     * @brief Шаблонный оператор записи сообщения в лог.
     *
     * Перед сообщением автоматически добавляется текущее системное время.
     *
     * @tparam T Тип записываемого значения.
     * @param message Значение, которое будет записано в лог.
     * @return Ссылка на текущий объект Logger (для цепочки вызовов).
     */
    template<typename T>
    Logger& operator<<(const T& message)
    {
        if (!m_output.is_open())
        {
            std::cerr << "Error: Log file is not open" << std::endl;
            return *this;
        }

        m_output << getCurrentTime() << message << std::endl;

        return *this;;
    }
};