#include <gmock/gmock.h>
#include <gtest/gtest.h>

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
    const common::PhoneNumber PHONE_NUMBER{112};
    SmsRepository objectUnderTest{PHONE_NUMBER, loggerMock};
};

TEST_F(SmsRepositoryTestSuite, shallStartStop)
{

}

}
