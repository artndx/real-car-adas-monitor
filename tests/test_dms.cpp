#include <gtest/gtest.h>
#include <env.h>
#include <dms_monitor.h>

using namespace dms;

TEST(DMSMonitor, NotLoadedByDefault)
{
    DMSMonitor monitor;
    EXPECT_FALSE(monitor.isLoaded());
}

TEST(DMSMonitor, AnalyzeEmptyFrame)
{
    DMSMonitor monitor;
    monitor.loadModels(DNN_FACE_DETECTOR_DEPLOY_PATH, DNN_FACE_DETECTOR_MODEL_PATH, HAAR_CASCADE_PATH);

    DriverState state;
    cv::Mat emptyFrame;
    EXPECT_NO_THROW(state = monitor.analyze(emptyFrame));
    EXPECT_FALSE(state.m_face_detected);
}

TEST(DMSMonitor, AnalyzeBlackFrame)
{
    DMSMonitor monitor;
    monitor.loadModels(DNN_FACE_DETECTOR_DEPLOY_PATH, DNN_FACE_DETECTOR_MODEL_PATH, HAAR_CASCADE_PATH);

    cv::Mat blackFrame(480, 640, CV_8UC3, cv::Scalar(0, 0, 0));
    DriverState state;
    EXPECT_NO_THROW(state = monitor.analyze(blackFrame));
    EXPECT_FALSE(state.m_face_detected);
}

TEST(DMSMonitorTest, LoadModels_FilesNotFound)
{
    DMSMonitor monitor;
    EXPECT_FALSE(monitor.loadModels("no.prototxt", "no.caffemodel", "no.xml"));
    EXPECT_FALSE(monitor.isLoaded());
}

TEST(DMSMonitorTest, LoadModels_ValidFiles)
{
    DMSMonitor monitor;
    EXPECT_TRUE(monitor.loadModels(DNN_FACE_DETECTOR_DEPLOY_PATH, DNN_FACE_DETECTOR_MODEL_PATH, HAAR_CASCADE_PATH));
    EXPECT_TRUE(monitor.isLoaded());
}

TEST(DMSMonitorTest, AnalyzeEmptyFrame_AfterLoad)
{
    DMSMonitor monitor;
    monitor.loadModels(DNN_FACE_DETECTOR_DEPLOY_PATH, DNN_FACE_DETECTOR_MODEL_PATH, HAAR_CASCADE_PATH);

    cv::Mat emptyFrame;
    DriverState state;

    EXPECT_NO_THROW(state = monitor.analyze(emptyFrame));
    EXPECT_FALSE(state.m_face_detected);
}