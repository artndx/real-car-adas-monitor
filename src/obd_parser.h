#pragma once
#include <vector>
#include <common.h>
#include <fstream>

namespace obd
{
    using OBDRecords = std::vector<OBDRecord>;

    /**
     * @brief Парсер OBD-данных из CSV файла.
     *
     * Читает телеметрические записи
     * как построчно, так и целиком из CSV файла.
     */
    class OBDParser
    {
    private:
        std::ifstream m_input;
        bool m_load = false;
        OBDRecords m_records;

    public:
        /**
         * @brief Открывает и загружает CSV файл с OBD-данными.
         *
         * @param dataset_path Путь к CSV файлу с данными.
         * @return true в случае успешного открытия, false при ошибке.
         */
        bool loadFile(const std::string& dataset_path);

        /**
         * @brief Построчно читает следующую OBD-запись из файла.
         *
         * @param record Ссылка на структуру, в которую будет записана прочитанная запись.
         * @return Код результата (0 при успешном чтении, ненулевое значение при ошибке или EOF).
         */
        int read(OBDRecord& record);

        /**
         * @brief Считывает все записи из открытого CSV файла в память.
         *
         * @return Код результата (0 при успешном выполнении, ненулевое значение при ошибке).
         */
        int readAll();

        /**
         * @brief Возвращает запись по индексу (для чтения/изменения).
         *
         * @param index Индекс записи в наборе.
         * @return Ссылка на запись с указанным индексом.
         */
        OBDRecord& getRecord(size_t index);

        /**
         * @brief Возвращает запись по индексу (только для чтения).
         *
         * @param index Индекс записи в наборе.
         * @return Константная ссылка на запись с указанным индексом.
         */
        const OBDRecord& getRecord(size_t index) const;

    private:
        /**
         * @brief Разделяет строку на подстроки по указанному разделителю.
         *
         * @param line Входная строка.
         * @param delimiter Символ-разделитель (по умолчанию ',').
         * @return Вектор полученных подстрок.
         */
        std::vector<std::string> split(std::string_view line, char delimiter = ',');
    };
}