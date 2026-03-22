#include <gtest/gtest.h>

// ==================== OpenCV ====================
#include <opencv2/opencv.hpp>
#include <opencv2/core/version.hpp>

// ==================== ONNX Runtime ====================
#include <onnxruntime_cxx_api.h>

// ==================== Тесты OpenCV ====================
TEST(OpenCVTest, VersionStringNotEmpty) {
    EXPECT_FALSE(std::string(CV_VERSION).empty());
    std::cout << "OpenCV version: " << CV_VERSION << std::endl;
}

TEST(OpenCVTest, CanCreateMat) {
    cv::Mat mat(100, 100, CV_8UC3, cv::Scalar(0, 255, 0));
    
    EXPECT_EQ(mat.rows, 100);
    EXPECT_EQ(mat.cols, 100);
    EXPECT_EQ(mat.channels(), 3);
    EXPECT_EQ(mat.type(), CV_8UC3);
}

TEST(OpenCVTest, CanProcessImage) {
    // Создаём тестовое изображение
    cv::Mat src = cv::Mat::zeros(64, 64, CV_8UC1);
    cv::circle(src, cv::Point(32, 32), 20, cv::Scalar(255), -1);
    
    // Применяем размытие — проверка, что алгоритмы работают
    cv::Mat dst;
    cv::GaussianBlur(src, dst, cv::Size(5, 5), 1.5);
    
    EXPECT_EQ(dst.size(), src.size());
    EXPECT_GT(cv::mean(dst)[0], 0);  // После размытия должны быть ненулевые значения
}

// ==================== Тесты ONNX Runtime ====================
TEST(OnnxRuntimeTest, CanCreateEnv) {
    // Ort::Env должен создаваться без исключений
    EXPECT_NO_THROW({
        Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "TestEnv");
        EXPECT_TRUE(env);
    });
}

TEST(OnnxRuntimeTest, CanCreateAllocator) {
    Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "TestEnv");
    
    EXPECT_NO_THROW({
        Ort::AllocatorWithDefaultOptions allocator;
        // Проверяем, что аллокатор работает — выделяем и освобождаем память
        void* ptr = allocator.Alloc(1024);
        EXPECT_NE(ptr, nullptr);
        allocator.Free(ptr);
    });
}

TEST(OnnxRuntimeTest, VersionStringAvailable) {
    // Проверяем, что можем получить версию через C-API
    const char* version = OrtGetApiBase()->GetVersionString();
    EXPECT_NE(version, nullptr);
    if (version) {
        std::cout << "ONNX Runtime API version: " << version << std::endl;
        EXPECT_GT(std::strlen(version), 0);
    }
}

// ==================== Интеграционный тест ====================
TEST(IntegrationTest, OpenCV_And_ONNX_Together) {
    // Проверяем, что библиотеки не конфликтуют при совместном использовании
    
    // 1. Создаём изображение через OpenCV
    cv::Mat image(224, 224, CV_32FC3, cv::Scalar(0.5f, 0.5f, 0.5f));
    
    // 2. Инициализируем ONNX Runtime
    Ort::Env env(ORT_LOGGING_LEVEL_ERROR, "IntegrationTest");
    Ort::SessionOptions session_options;
    session_options.SetIntraOpNumThreads(1);
    
    // 3. Проверяем, что данные из OpenCV можно использовать
    // (например, получить указатель на данные для передачи в ONNX)
    float* data = image.ptr<float>(0);
    EXPECT_NE(data, nullptr);
    EXPECT_EQ(image.total() * image.channels(), 224 * 224 * 3);
    
    // 4. Проверяем, что ONNX Environment всё ещё жив
    EXPECT_TRUE(env);
}