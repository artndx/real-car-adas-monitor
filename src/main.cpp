#include <iostream>

#include <opencv2/opencv.hpp>
#include <onnxruntime_cxx_api.h>

int main() {
    std::cout << "OpenCV version: " << CV_VERSION << std::endl;
    
    Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "test");
    std::cout << "ONNX Runtime initialized successfully" << std::endl;
}