#pragma once
#include <filesystem>

namespace fs = std::filesystem;
using path = fs::path;

static const path PROJECT_DIR = fs::current_path().parent_path();

static const std::string DATA_FOLDER = "data";
static const std::string DATASET_NAME = "dataset.csv";
static const path DATASET = PROJECT_DIR / DATA_FOLDER / DATASET_NAME;