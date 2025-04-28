#pragma once

#include "ISmsRepository.h"
#include "Logger/PrefixedLogger.hpp"
#include <fstream>

namespace ue
{

class SmsRepository : public ISmsRepository
{
public:
    SmsRepository(common::PhoneNumber, common::ILogger &);

    // ISmsRepository interface
    void save(const SmsEntity &) override;
    std::vector<SmsEntity> getAll() override;
private:
    std::fstream file;
    common::PrefixedLogger logger;
    common::PhoneNumber phoneNumber;
    std::string filename;
};

}
