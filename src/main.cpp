#include <iostream>
#include <atomic>
#include <thread>
#include <env.h>
#include <fstream>

#include <obd_parser.h>
#include <onnx_classifier.h>
#include <dashboard.h>
#include <dms_hud.h>
#include <logger.h>
#include <stat_tool.h>

void test_obdParsing()
{
    obd::OBDParser parser;
    if (!parser.loadFile(DATASET_PATH))
    {
        std::cerr << "Cannot load CSV\n";
        return;
    }

    int row_count = parser.readAll();
    size_t slow_count = 0;
    size_t normal_count = 0;
    size_t aggres_count = 0;
    for (size_t i = 0; i < row_count; ++i)
    {
        OBDRecord& record = parser.getRecord(i);
        if (record.m_label == LabelType::SLOW)
            slow_count++;
        else if (record.m_label == LabelType::NORMAL)
            normal_count++;
        else if (record.m_label == LabelType::AGGRESSIVE)
            aggres_count++;
    }

    std::cout << "{ SLOW : " << slow_count << ", NORMAL : " << normal_count << ", " << "AGGRESSIVE : " << aggres_count << " }" << std::endl;
    for (size_t i = 0; i < 5; ++i)
    {
        std::cout << parser.getRecord(i) << std::endl;
    }
}

int test_onnxClassifier()
{
    onnx::ONNXClassifier classifier;

    if (classifier.loadModel(DRIVER_CLASS_MODEL_PATH) != 0)
    {
        std::cerr << "Failed to load model\n";
        return 1;
    }
    if (classifier.loadJson(NORM_PARAMS_PATH) != 0)
    {
        std::cerr << "Failed to load JSON params\n";
        return 1;
    }

    // Заголовок таблицы
    std::cout << std::left
        << std::setw(12) << "Истинная "
        << std::setw(14) << "Предсказанная "
        << std::setw(12) << "Уверенность " << "\n";
    std::cout << std::string(38, '-') << "\n";

    size_t correct = 0;

    obd::OBDParser parser;
    if (!parser.loadFile(DATASET_PATH))
    {
        std::cerr << "Cannot load CSV\n";
        return 1;
    }

    int row_count = parser.readAll();
    for (size_t i = 2320; i < std::min(row_count, 2320 + 20); ++i)
    {
        OBDRecord& record = parser.getRecord(i);
        onnx::ArrayF<6> features = {
            record.m_speed,
            record.m_engine_rpm,
            record.m_throttle_position,
            record.m_coolant_temp,
            record.m_fuel_level,
            record.m_intake_air_temp };

        auto result = classifier.classify(features);

        LabelType true_label = record.m_label;
        LabelType prediction_label = result.m_label;

        if (true_label == prediction_label)
            ++correct;

        std::cout << std::left
            << std::setw(12) << true_label
            << std::setw(14) << prediction_label
            << std::fixed << std::setprecision(3) << result.m_confidence << "\n";
    }

    double accuracy = (static_cast<double>(correct) / 20) * 100.0;
    std::cout << "\nAccuracy: " << std::fixed << std::setprecision(1) << accuracy << "%\n";
    std::cout << "Требование: >= 80%\n";

    return 0;
}

int test_dashboard()
{
    using namespace dashboard;

    try
    {
        Dashboard dash;

        obd::OBDParser parser;
        if (!parser.loadFile(DATASET_PATH))
        {
            std::cerr << "Cannot load CSV\n";
            return -1;
        }
        int row_count = parser.readAll();
        size_t row_idx = 1000;

        std::cout << "Press any key to exit..." << std::endl;
        while (cv::waitKey(150) == -1)
        {
            if (row_idx >= row_count)
                break;

            OBDRecord record = parser.getRecord(row_idx++);
            cv::Mat frame;
            dash.draw(frame, record);
            cv::imshow("Dashboard Test", frame);
        }

        cv::destroyAllWindows();

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

int test_dms()
{
    dms::DMSMonitor monitor;
    dms::DMSHUD hud;

    if (!monitor.loadModels(DNN_FACE_DETECTOR_DEPLOY_PATH, DNN_FACE_DETECTOR_MODEL_PATH, HAAR_CASCADE_PATH))
    {
        std::cerr << "Error load models" << std::endl;
        return 1;
    }
#ifndef INPUT_FRAMES_FROM_MND_FOLDER
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
    {
        std::cerr << "Webcam is not opened" << std::endl;
        return 1;
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    cap.set(cv::CAP_PROP_BUFFERSIZE, 1);
#endif

    cv::Mat frame;
    while (true)
    {
#ifdef INPUT_FRAMES_FROM_MND_FOLDER
        frame = cv::imread(MNT_FRAME_PATH);
        if (frame.empty())
            continue;
#else
        cap.grab();
        if (!cap.retrieve(frame))
            continue;
#endif

        dms::DriverState state = monitor.analyze(frame);
        cv::Mat output(480, 1280, CV_8UC3, cv::Scalar(30, 30, 30));

        hud.draw(output, frame, state);

        cv::imshow("test_dms", output);
        if (cv::waitKey(1) == 27)
            break;
    }

#ifndef INPUT_FRAMES_FROM_MND_FOLDER
    cap.release();
    cv::destroyAllWindows();
#endif

    return 0;
}

struct SharedState
{
    OBDRecord m_record;
    onnx::ClassificationResult m_result;
    size_t m_alertCount = 0;
    std::atomic<bool> m_running = true;
    std::atomic<bool> m_pause = false;
    std::mutex m_mutex;
};

int test_final()
{
    using namespace std::literals;

    obd::OBDParser parser;
    if (!parser.loadFile(DATASET_PATH))
    {
        std::cerr << "Cannot load CSV\n";
        return 1;
    }

    onnx::ONNXClassifier classifier;
    if (classifier.loadModel(DRIVER_CLASS_MODEL_PATH) != 0)
    {
        std::cerr << "Failed to load model\n";
        return 1;
    }

    if (classifier.loadJson(NORM_PARAMS_PATH) != 0)
    {
        std::cerr << "Failed to load JSON params\n";
        return 1;
    }

    dashboard::Dashboard dash;
    dms::DMSMonitor monitor;
    dms::DMSHUD hud;

    if (!monitor.loadModels(DNN_FACE_DETECTOR_DEPLOY_PATH, DNN_FACE_DETECTOR_MODEL_PATH, HAAR_CASCADE_PATH))
    {
        std::cerr << "Error load models" << std::endl;
        return 1;
    }

    cv::VideoWriter writer(VIDEO_RECORD_PATH,
        cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
        10, cv::Size(1280, 480)
    );

    if (!writer.isOpened())
    {
        std::cerr << "Cannot open video writer" << std::endl;
        return 1;
    }

    Logger logger;
    if (!logger.open(LOG_PATH))
    {
        std::cerr << "Cannot open log file" << std::endl;
        return 1;
    }

#ifndef INPUT_FRAMES_FROM_MND_FOLDER
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
    {
        std::cerr << "Webcam is not opened" << std::endl;
        return 1;
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    cap.set(cv::CAP_PROP_BUFFERSIZE, 1);
#endif
    StatTool stats;
    stats.start();

    SharedState state;
    std::jthread obdParsing(
        [&]
        {
            while (state.m_running)
            {
                if (state.m_pause)
                {
                    std::this_thread::sleep_for(100ms);
                    continue;
                }

                OBDRecord record;
                int res = parser.read(record);
                if (res == -1)
                    break;

                if (res == -2)
                    continue;

                onnx::ClassificationResult result = classifier.classify(record);

                {
                    std::lock_guard<std::mutex> lock(state.m_mutex);
                    state.m_record = record;
                    state.m_result = result;

                    stats.recordOBD();
                    if (result.m_label == LabelType::AGGRESSIVE)
                    {
                        state.m_alertCount++;
                        stats.recordAlert(StatTool::AlertType::AGGRESSIVE);
                        logger << std::format("AGGRESSIVE driving detected. Alert #{}", state.m_alertCount);
                    }
                }

                std::this_thread::sleep_for(100ms);
            }
        });

    cv::Mat output(480, 1280, CV_8UC3, cv::Scalar(30, 30, 30));
    std::jthread dmsAnalyzing(
        [&]
        {
            while (state.m_running)
            {
                int key = cv::waitKey(1);
                if (key == 'q' || key == 'Q')
                {
                    std::lock_guard<std::mutex> lock(state.m_mutex);
                    state.m_running = false;
                    break;
                }
                else if (key == ' ')
                {
                    std::lock_guard<std::mutex> lock(state.m_mutex);
                    state.m_pause = !state.m_pause;
                }
                else if (key == 's' || key == 'S')
                {
                    cv::imwrite(SCREENSHOT_PATH, output);
                }

                if (state.m_pause)
                {
                    std::this_thread::sleep_for(100ms);
                    continue;
                }

                cv::Mat frame;
#ifdef INPUT_FRAMES_FROM_MND_FOLDER
                frame = cv::imread(MNT_FRAME_PATH);
                if (frame.empty())
                    continue;
#else
                cap.grab();
                if (!cap.retrieve(frame))
                    continue;
#endif
                dms::DriverState driverState = monitor.analyze(frame);

                if (driverState.m_alert_drowsy)
                    stats.recordAlert(StatTool::AlertType::DROWSY);
                else if (driverState.m_alert_distracted)
                    stats.recordAlert(StatTool::AlertType::DISTRACTED);

                OBDRecord record;
                onnx::ClassificationResult result;
                {
                    std::lock_guard<std::mutex> lock(state.m_mutex);
                    record = state.m_record;
                    result = state.m_result;
                }

                cv::Mat dashboard = output(cv::Rect(0, 0, 640, 480));
                cv::Mat camera = output(cv::Rect(640, 0, 640, 480));

                dash.draw(dashboard, record);
                hud.draw(output, frame, driverState);

                cv::imshow("test_final", output);
                writer.write(output);

                std::this_thread::sleep_for(100ms);
            }

            cv::destroyAllWindows();
            writer.release();
#ifndef INPUT_FRAMES_FROM_MND_FOLDER
            cap.release();
#endif
            stats.printSummary();
        });

    return 0;
}

int main()
{
    return test_final();
}