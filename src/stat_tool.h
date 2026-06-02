#pragma once
#include <chrono>
#include <iostream>

class StatTool
{
    std::chrono::steady_clock::time_point m_start;
    int m_recordCount = 0;
    int m_alertCount = 0;
    int m_drowsyCount = 0;
    int m_distractedCount = 0;
    int m_aggressiveCount = 0;
public:
    enum class AlertType
    {
        DROWSY,
        DISTRACTED,
        AGGRESSIVE
    };

    void start()
    {
        m_start = std::chrono::steady_clock::now();
    }

    void recordOBD()
    {
        m_recordCount++;
    }

    void recordAlert(AlertType alertType)
    {
        m_alertCount++;
        switch (alertType)
        {
        case AlertType::DROWSY: m_drowsyCount++;     break;
        case AlertType::DISTRACTED: m_distractedCount++;  break;
        case AlertType::AGGRESSIVE: m_aggressiveCount++;  break;
        default: break;
        }
    }

    void printSummary() const
    {
        auto end = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - m_start).count();

        std::cout << "Uptime:              " << elapsed / 3600 << "h "
            << (elapsed % 3600) / 60 << "m "
            << elapsed % 60 << "s\n";
        std::cout << "OBD records:         " << m_recordCount << "\n";
        std::cout << "Total alerts:        " << m_alertCount << "\n";
        std::cout << "  Fatigue:           " << m_drowsyCount << "\n";
        std::cout << "  Distracted:        " << m_distractedCount << "\n";
        std::cout << "  Aggressive:        " << m_aggressiveCount << "\n";
        std::cout << "===========================\n";
    }
};