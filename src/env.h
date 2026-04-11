#pragma once
#include <filesystem>

namespace fs = std::filesystem;
using path = fs::path;

static const path PROJECT_DIR = fs::current_path().parent_path();

static const std::string DATA_FOLDER = "data";
static const std::string DATASET_FILE = "dataset.csv";
static const path DATASET_PATH = PROJECT_DIR / DATA_FOLDER / DATASET_FILE;

static const std::string MODELS_FOLDER = "models";
static const std::string MODEL_FILE = "driver_classifier.onnx";
static const path MODEL_PATH = PROJECT_DIR / MODELS_FOLDER / MODEL_FILE;

static const std::string NORM_PARAMS_FILE = "normalization_params.json";
static const path NORM_PARAMS_PATH = PROJECT_DIR / MODELS_FOLDER / NORM_PARAMS_FILE;