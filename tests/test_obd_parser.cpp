#include <gtest/gtest.h>
#include <env.h>
#include <fstream>
#include <obd_parser.h>

using namespace obd;

TEST(OBDParser, LabelTypeParsing) 
{
    EXPECT_EQ(getLabelType("SLOW"), LabelType::SLOW);
    EXPECT_EQ(getLabelType("NORMAL"), LabelType::NORMAL);
    EXPECT_EQ(getLabelType("AGGRESSIVE"), LabelType::AGGRESSIVE);
    
    EXPECT_EQ(getLabelType("UNKNOWN"), LabelType::NONE);
    EXPECT_EQ(getLabelType(""), LabelType::NONE);
}

class OBDParserTest : public ::testing::Test {
protected:
    inline static const std::string TEST_DATASET_NAME = "test.csv";
    inline static const path TEST_DATASET = PROJECT_DIR / DATA_FOLDER / TEST_DATASET_NAME;
    
    void TearDown() override {
        if (fs::exists(TEST_DATASET)) {
            fs::remove(TEST_DATASET);
        }
    }
    
    void CreateValidCSV(int rows = 3) {
        std::ofstream out(TEST_DATASET);
        out << "speed_kmh,engine_rpm,throttle_pos,coolant_temp,fuel_level,intake_air_temp,label\n";
        for (int i = 0; i < rows; ++i) {
            out << (60.0 + i * 10) << "," 
                << (2500 + i * 100) << ","
                << (45.0 + i * 5) << ","
                << (90.0 + i * 2) << ","
                << (75.0 - i * 5) << ","
                << (25.0 + i * 3) << ","
                << "NORMAL\n";
        }
        out.close();
    }

    void CreateInvalidCSV(int rows = 3) {
        std::ofstream out(TEST_DATASET);
        out << "speed_kmh,engine_rpm,throttle_pos,coolant_temp,fuel_level,intake_air_temp,label\n"
            << "60.5,2500,45.0,90.0,75.0,25.0,NORMAL\n"
            << "INVALID,ROW\n"
            << "80.0,3200,60.0,92.0,70.0,28.0,AGGRESSIVE\n"
            << "70.0,abc,50.0,88.0,65.0,20.0,SLOW\n"
            << "50.0,2000,40.0,85.0,80.0,22.0,SLOW\n";
        out.close();
    }
};

TEST_F(OBDParserTest, NonExistData) {
    OBDParser parser;
    EXPECT_EQ(parser.load("-.csv"), -1);
}

TEST_F(OBDParserTest, InvalidIndex) {
    CreateValidCSV(1);
    
    OBDParser parser;
    int count = parser.load(TEST_DATASET);
    EXPECT_THROW(parser.getRecord(10), std::out_of_range);
}

TEST_F(OBDParserTest, ValidData) {
    CreateValidCSV(5);
    
    OBDParser parser;
    int count = parser.load(TEST_DATASET);
    
    EXPECT_EQ(count, 5);
    EXPECT_NO_THROW(parser.getRecord(0));
    EXPECT_NO_THROW(parser.getRecord(4));
}

TEST_F(OBDParserTest, InvalidRowsData) {
    CreateInvalidCSV(5);
    
    OBDParser parser;
    int count = parser.load(TEST_DATASET);
    
    EXPECT_EQ(count, 3);  // 3 корректные из 5
}