#pragma once

#include "SmsEntity.h"
#include "Messages/PhoneNumber.hpp"
#include <vector>

namespace ue
{

class ISmsRepository
{
public:
    virtual ~ISmsRepository() = default;

    virtual void save(const SmsEntity&) = 0;
    virtual std::vector<SmsEntity> getAll() = 0;
};

}
