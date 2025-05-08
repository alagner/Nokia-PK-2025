#pragma once

#include "SmsRepository/ISmsRepository.h"
#include "SmsRepository/SmsEntity.h"
#include "Messages/PhoneNumber.hpp"
#include <vector>
#include <algorithm>
#include <gmock/gmock.h>


namespace ue
{
class ISmsRepositoryMock : public ISmsRepository
{
public:
    ISmsRepositoryMock();
    ~ISmsRepositoryMock() override;

    MOCK_METHOD(void, save, (const SmsEntity&), (final));
    MOCK_METHOD(std::vector<SmsEntity>, getAll, (), (final));
    MOCK_METHOD(void, saveAll, (const std::vector<SmsEntity>&, bool ), (final));

    std::vector<SmsEntity> database;
};
}

