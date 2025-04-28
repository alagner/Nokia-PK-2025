#include "SmsRepository.h"
#include <Json/json.hpp>

using json = nlohmann::json;

namespace ue
{
SmsRepository::SmsRepository(common::PhoneNumber phoneNumber, common::ILogger &logger) : logger(logger, "[SMS REPOSITORY]"), phoneNumber(phoneNumber)
{
  this->filename = "./database" + std::to_string(this->phoneNumber.value) + ".json";
  std::ifstream file(this->filename);
  if (!file){
    file.close();
    std::ofstream createdFile(this->filename);
    createdFile << "[]";
  }
}

void SmsRepository::save(const SmsEntity & newSms)
{
  newSms.save(this->filename);
}

std::vector<SmsEntity> SmsRepository::getAll()
{
  std::vector<SmsEntity> returnSmsVector;
  std::ifstream databaseFile(this->filename);

  json database;
  databaseFile >> database;

  for (auto & smsData : database) {
    SmsEntity entity {smsData["from"], smsData["to"], smsData["text"], smsData["isRead"]};
    returnSmsVector.push_back(entity);
  }

  return returnSmsVector;
}

void SmsRepository::saveAll(const std::vector<SmsEntity> & smsVector, bool clearDb)
{
  if (clearDb){
    std::ofstream writeDatabaseFile(this->filename);
    writeDatabaseFile << "[]";
    writeDatabaseFile.close();
  }

  for (auto & sms : smsVector) {
    sms.save(this->filename);
  }
}
}
