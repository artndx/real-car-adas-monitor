#pragma once
#include <filesystem>

namespace fs = std::filesystem;
using path = fs::path;

static const path PROJECT_DIR = fs::current_path().parent_path();

/* data */
static const std::string DATA_FOLDER = "data";

static const std::string DATASET_FILE = "dataset.csv";
static const path DATASET_PATH = PROJECT_DIR / DATA_FOLDER / DATASET_FILE;

/* output */
#define PRINT_FRAMES
static const std::string OUTPUT_FOLDER = "output";
static const std::string FRAMES_FOLDER = "frames";
static const path FRAMES_PATH = PROJECT_DIR / OUTPUT_FOLDER / FRAMES_FOLDER;
static const std::string SCREENSHOT_FILE = "screenshot.png";
static const path SCREENSHOT_PATH = PROJECT_DIR / OUTPUT_FOLDER / SCREENSHOT_FILE;
static const std::string VIDEO_RECORD_FILE = "result_situation2.mp4";
static const path VIDEO_RECORD_PATH = PROJECT_DIR / OUTPUT_FOLDER / VIDEO_RECORD_FILE;
static const std::string LOG_FILE = "dms_alerts.log";
static const path LOG_PATH = PROJECT_DIR / OUTPUT_FOLDER / LOG_FILE;

/* models */
static const std::string MODELS_FOLDER = "models";

static const std::string DRIVER_CLASS_MODEL_FILE = "driver_classifier.onnx";
static const path DRIVER_CLASS_MODEL_PATH = PROJECT_DIR / MODELS_FOLDER / DRIVER_CLASS_MODEL_FILE;

static const std::string NORM_PARAMS_FILE = "normalization_params.json";
static const path NORM_PARAMS_PATH = PROJECT_DIR / MODELS_FOLDER / NORM_PARAMS_FILE;

static const std::string DNN_FACE_DETECTOR_DEPLOY_FILE = "deploy.prototxt.txt";
static const path DNN_FACE_DETECTOR_DEPLOY_PATH = PROJECT_DIR / MODELS_FOLDER / DNN_FACE_DETECTOR_DEPLOY_FILE;

static const std::string DNN_FACE_DETECTOR_MODEL_FILE = "res10_300x300_ssd_iter_140000.caffemodel";
static const path DNN_FACE_DETECTOR_MODEL_PATH = PROJECT_DIR / MODELS_FOLDER / DNN_FACE_DETECTOR_MODEL_FILE;

static const std::string HAAR_CASCADE_FILE = "haarcascade_eye.xml";
static const path HAAR_CASCADE_PATH = PROJECT_DIR / MODELS_FOLDER / HAAR_CASCADE_FILE;

/* input frames*/
#define INPUT_FRAMES_FROM_MND_FOLDER
static const path MNT_FRAME_PATH = "/mnt/c/frame.jpg";

[[noodiscard]] inline int nop()
{
    return 0;
}