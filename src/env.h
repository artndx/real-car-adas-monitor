#pragma once
#include <filesystem>

namespace fs = std::filesystem;
using path = fs::path;

static const path PROJECT_DIR = fs::current_path().parent_path();

/* data */
static const std::string DATA_FOLDER = "data";

static const std::string DATASET_FILE = "dataset.csv";
static const path DATASET_PATH = PROJECT_DIR / DATA_FOLDER / DATASET_FILE;

/* models*/
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