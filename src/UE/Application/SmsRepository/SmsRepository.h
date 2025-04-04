#pragma once

#include "ISmsRepository.h"
#include <fstream>

namespace ue
{

class SmsRepository : ISmsRepository
{
public:
    SmsRepository();
    ~SmsRepository();

    // ISmsRepository interface
    void save(const SmsEntity &) override;
    std::vector<SmsEntity> getAll(const common::PhoneNumber &) override;
private:
    std::fstream file;
};

}
