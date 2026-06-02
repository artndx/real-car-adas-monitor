#pragma once

#include <iostream>
#include <fstream>

class Logger
{
    std::ofstream m_output;
public:
    bool open(const std::string& log_file)
    {
        m_output.open(log_file, std::ios::app);
        return m_output.is_open();
    }

    ~Logger()
    {
        if (m_output.is_open())
            m_output.close();
    }

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