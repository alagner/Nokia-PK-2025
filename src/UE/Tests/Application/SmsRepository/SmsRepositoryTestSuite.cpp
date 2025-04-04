#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "SmsRepository/SmsRepository.h"
#include "SmsRepository/SmsEntity.h"
#include "Messages/PhoneNumber.hpp"

namespace ue
{
using namespace ::testing;

class SmsRepositoryTestSuite : public Test
{
protected:
    SmsRepository objectUnderTest;
};

TEST_F(SmsRepositoryTestSuite, shallStartStop)
{

}

}
