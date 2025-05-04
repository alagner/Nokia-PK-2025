#include "SmsEntity.h"
#include <Json/json.hpp>

using json = nlohmann::json;

namespace ue{
void SmsEntity::save(const std::string & filename) const
{
  std::ifstream readDatabase(filename);
  json database;
  readDatabase >> database;
  json smsData;
  smsData["from"] = this->from;
  smsData["to"] = this->to;
  smsData["text"] = this->text;
  smsData["isRead"] = this->isRead;
  database.push_back(smsData);

  readDatabase.close();
  std::ofstream writeDatabase(filename);

  writeDatabase << std::setw(4) << database << std::endl;
}

}

