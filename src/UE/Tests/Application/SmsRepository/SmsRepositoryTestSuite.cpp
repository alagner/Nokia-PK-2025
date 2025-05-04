#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>
#include <cstdlib>

#include "SmsRepository/SmsRepository.h"
#include "SmsRepository/SmsEntity.h"
#include "Messages/PhoneNumber.hpp"
#include "Mocks/ILoggerMock.hpp"

namespace ue
{
using namespace ::testing;

class SmsRepositoryTestSuite : public Test
{
protected:
    NiceMock<common::ILoggerMock> loggerMock;
    const common::PhoneNumber PHONE_NUMBER{0};
    const std::string filename{"database0.json"};
    SmsRepository objectUnderTest{PHONE_NUMBER, loggerMock};

    SmsRepositoryTestSuite()
    {
      std::ifstream file(filename);
      EXPECT_TRUE(file.is_open());
    }
    ~SmsRepositoryTestSuite()
    {
      remove(filename.c_str());
      std::ifstream file(filename);
      EXPECT_FALSE(file.is_open());
    }
};

TEST_F(SmsRepositoryTestSuite, shallCreateFileAndDelete)
{

}

TEST_F(SmsRepositoryTestSuite, shallSaveAndReadSms)
{
  SmsEntity smsToSave{1, 0, "Test", 0};

  objectUnderTest.save(smsToSave);
  auto databaseData = objectUnderTest.getAll();

  EXPECT_EQ(databaseData.size(), 1);;
  EXPECT_TRUE(smsToSave == databaseData[0]);
}

TEST_F(SmsRepositoryTestSuite, shallSaveAndReadMultipleSms)
{
  SmsEntity smsToSave1{1, 0, "Test1", 0};
  SmsEntity smsToSave2{2, 0, "Test2", 0};
  SmsEntity smsToSave3{3, 0, "Test3", 0};

  objectUnderTest.save(smsToSave1);

  std::vector<SmsEntity> smsVector;

  smsVector.push_back(smsToSave2);
  smsVector.push_back(smsToSave3);

  objectUnderTest.saveAll(smsVector);
  auto databaseData = objectUnderTest.getAll();

  EXPECT_EQ(databaseData.size(), 2);
  for (int i = 0; i < 2; i++){
    EXPECT_TRUE(smsVector[i] == databaseData[i]);
  }
}

TEST_F(SmsRepositoryTestSuite, shallSaveAndReadMultipleSmsWithoutCleaningDb)
{
  SmsEntity smsToSave1{1, 0, "Test1", 0};
  SmsEntity smsToSave2{2, 0, "Test2", 0};
  SmsEntity smsToSave3{3, 0, "Test3", 0};

  objectUnderTest.save(smsToSave1);

  std::vector<SmsEntity> smsVector;

  smsVector.push_back(smsToSave2);
  smsVector.push_back(smsToSave3);

  objectUnderTest.saveAll(smsVector, false);
  auto databaseData = objectUnderTest.getAll();

  EXPECT_EQ(databaseData.size(), 3);

  EXPECT_TRUE(smsToSave1 == databaseData[0]);
  for (int i = 0; i < 2; i++){
    EXPECT_TRUE(smsVector[i] == databaseData[i+1]);
  }
}

}
