#include "SmsRepository.h"

namespace ue
{
SmsRepository::SmsRepository(common::ILogger &logger) : logger(logger, "[SMS REPOSITORY]")
{

}

void SmsRepository::save(const SmsEntity & newSms)
{

}

std::vector<SmsEntity> SmsRepository::getAll(const common::PhoneNumber &)
{

}
}
