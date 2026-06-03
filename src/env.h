#pragma once
#include <filesystem>

namespace fs = std::filesystem;
using path = fs::path;

/**
 * @brief Корневая директория проекта (родитель рабочей директории).
 */
static const path PROJECT_DIR = fs::current_path().parent_path();

/* data */
/**
 * @brief Имя папки с входными данными (относительно корня проекта).
 */
static const std::string DATA_FOLDER = "data";

/**
 * @brief Имя CSV-файла с OBD-датасетом.
 */
static const std::string DATASET_FILE = "dataset.csv";

/**
 * @brief Полный путь к CSV-файлу с OBD-датасетом.
 */
static const path DATASET_PATH = PROJECT_DIR / DATA_FOLDER / DATASET_FILE;

/* output */
#define PRINT_FRAMES

/**
 * @brief Имя выходной папки проекта.
 */
static const std::string OUTPUT_FOLDER = "output";

/**
 * @brief Имя подпапки для сохранения отдельных кадров.
 */
static const std::string FRAMES_FOLDER = "frames";

/**
 * @brief Путь к папке для сохранения отдельных кадров.
 */
static const path FRAMES_PATH = PROJECT_DIR / OUTPUT_FOLDER / FRAMES_FOLDER;

/**
 * @brief Имя файла для сохранения скриншота.
 */
static const std::string SCREENSHOT_FILE = "screenshot.png";

/**
 * @brief Полный путь к файлу скриншота.
 */
static const path SCREENSHOT_PATH = PROJECT_DIR / OUTPUT_FOLDER / SCREENSHOT_FILE;

/**
 * @brief Имя выходного видеофайла с записью процесса мониторинга.
 */
static const std::string VIDEO_RECORD_FILE = "result_situation2.mp4";

/**
 * @brief Полный путь к выходному видеофайлу.
 */
static const path VIDEO_RECORD_PATH = PROJECT_DIR / OUTPUT_FOLDER / VIDEO_RECORD_FILE;

/**
 * @brief Имя файла лога предупреждений DMS.
 */
static const std::string LOG_FILE = "dms_alerts.log";

/**
 * @brief Полный путь к файлу лога предупреждений DMS.
 */
static const path LOG_PATH = PROJECT_DIR / OUTPUT_FOLDER / LOG_FILE;

/* models */

/**
 * @brief Имя папки с моделями машинного обучения.
 */
static const std::string MODELS_FOLDER = "models";

/**
 * @brief Имя ONNX-модели классификатора стиля вождения.
 */
static const std::string DRIVER_CLASS_MODEL_FILE = "driver_classifier.onnx";

/**
 * @brief Полный путь к ONNX-модели классификатора стиля вождения.
 */
static const path DRIVER_CLASS_MODEL_PATH = PROJECT_DIR / MODELS_FOLDER / DRIVER_CLASS_MODEL_FILE;

/**
 * @brief Имя JSON-файла с параметрами нормализации признаков.
 */
static const std::string NORM_PARAMS_FILE = "normalization_params.json";

/**
 * @brief Полный путь к файлу с параметрами нормализации признаков.
 */
static const path NORM_PARAMS_PATH = PROJECT_DIR / MODELS_FOLDER / NORM_PARAMS_FILE;

/**
 * @brief Имя prototxt-файла описания DNN-модели детекции лица.
 */
static const std::string DNN_FACE_DETECTOR_DEPLOY_FILE = "deploy.prototxt.txt";

/**
 * @brief Полный путь к prototxt-файлу описания DNN-модели детекции лица.
 */
static const path DNN_FACE_DETECTOR_DEPLOY_PATH = PROJECT_DIR / MODELS_FOLDER / DNN_FACE_DETECTOR_DEPLOY_FILE;

/**
 * @brief Имя файла с бинарными весами DNN-модели детекции лица.
 */
static const std::string DNN_FACE_DETECTOR_MODEL_FILE = "res10_300x300_ssd_iter_140000.caffemodel";

/**
 * @brief Полный путь к файлу с бинарными весами DNN-модели детекции лица.
 */
static const path DNN_FACE_DETECTOR_MODEL_PATH = PROJECT_DIR / MODELS_FOLDER / DNN_FACE_DETECTOR_MODEL_FILE;

/**
 * @brief Имя XML-файла каскада Хаара для детекции глаз.
 */
static const std::string HAAR_CASCADE_FILE = "haarcascade_eye.xml";

/**
 * @brief Полный путь к файлу каскада Хаара для детекции глаз.
 */
static const path HAAR_CASCADE_PATH = PROJECT_DIR / MODELS_FOLDER / HAAR_CASCADE_FILE;

/* input frames*/
#define INPUT_FRAMES_FROM_MND_FOLDER

/**
 * @brief Путь к входному кадру на WSL-монтировании.
 */
static const path MNT_FRAME_PATH = "/mnt/c/frame.jpg";

/**
 * @brief Пустая функция-заглушка, всегда возвращает 0.
 *
 * Используется как пример/no-op в шаблонных/тестовых сценариях.
 *
 * @return Всегда 0.
 */
[[noodiscard]] inline int nop()
{
    return 0;
}