#pragma once

#include "ISmsRepository.h"
#include "Logger/PrefixedLogger.hpp"
#include <fstream>

namespace ue
{

class SmsRepository : ISmsRepository
{
public:
    SmsRepository(common::ILogger &);

    // ISmsRepository interface
    void save(const SmsEntity &) override;
    std::vector<SmsEntity> getAll(const common::PhoneNumber &) override;
private:
    std::fstream file;
    common::PrefixedLogger logger;
};

}
