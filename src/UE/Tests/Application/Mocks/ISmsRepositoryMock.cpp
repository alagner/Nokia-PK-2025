#include "ISmsRepositoryMock.h"

namespace ue
{

ISmsRepositoryMock::ISmsRepositoryMock(){
    ON_CALL(*this, save).WillByDefault([this](SmsEntity sms) {
        database.push_back(sms);
    });

    ON_CALL(*this, getAll).WillByDefault([this]() {
        return database;
    });

    ON_CALL(*this, saveAll).WillByDefault([this](const std::vector<SmsEntity>& smsVector, bool clearDb) {
        if (clearDb)
            database.clear();

        for (auto &sms : smsVector)
            database.push_back(sms);
    });
}
ISmsRepositoryMock::~ISmsRepositoryMock() = default;

}

