#pragma once

#include "ISmsRepository.h"
#include "Logger/PrefixedLogger.hpp"
#include <fstream>

namespace ue
{

class SmsRepository : ISmsRepository
{
public:
    SmsRepository(common::PhoneNumber, common::ILogger &);

    // ISmsRepository interface
    void save(const SmsEntity &) override;
    std::vector<SmsEntity> getAll() override;
    void saveAll(const std::vector<SmsEntity> &, bool = true) override;
private:
    common::PrefixedLogger logger;
    common::PhoneNumber phoneNumber;
    std::string filename;
};

}
