#include "SmsRepository.h"

namespace ue
{
SmsRepository::SmsRepository(common::PhoneNumber phoneNumber, common::ILogger &logger) : logger(logger, "[SMS REPOSITORY]"), phoneNumber(phoneNumber)
{
    this->filename = "./database" + std::to_string(this->phoneNumber.value) + ".dat";
}

void SmsRepository::save(const SmsEntity & newSms)
{
    std::uint8_t* nonConstFrom = const_cast<std::uint8_t*>(&newSms.from);
    std::uint8_t* nonConstTo = const_cast<std::uint8_t*>(&newSms.to);
    bool* nonConstBool = const_cast<bool*>(&newSms.isRead);
    int textSize = newSms.text.size();

    this->file.open(this->filename, std::ios_base::binary | std::ios_base::app);
    if (this->file.is_open()){
        this->file.write(reinterpret_cast<char*>(nonConstFrom), sizeof(std::uint8_t));
        this->file.write(reinterpret_cast<char*>(nonConstTo), sizeof(std::uint8_t));
        this->file.write(reinterpret_cast<char*>(&textSize), sizeof(textSize));
        this->file.write(newSms.text.c_str(), textSize);
        this->file.write(reinterpret_cast<char*>(nonConstBool), sizeof(bool));
        this->logger.logInfo("Zapisano!");
    }
    this->file.close();
}

std::vector<SmsEntity> SmsRepository::getAll()
{
    std::vector<SmsEntity> returnSmsVector;
    std::uint8_t phoneNumberFrom;
    std::uint8_t phoneNumberTo;
    bool isRead;

    std::string text;
    int textSize;

    this->file.open(this->filename, std::ios_base::binary | std::ios_base::in);

    this->file.seekg (0, this->file.end);
    int length = this->file.tellg();
    this->file.seekg (0, this->file.beg);

    if (this->file.is_open()){
        while (length > 0){
            this->file.read(reinterpret_cast<char*>(&phoneNumberFrom), sizeof(std::uint8_t));
            this->file.read(reinterpret_cast<char*>(&phoneNumberTo), sizeof(std::uint8_t));
            this->file.read(reinterpret_cast<char*>(&textSize), sizeof(textSize));
            text.resize(textSize);
            this->file.read(text.data(), textSize);
            this->file.read(reinterpret_cast<char*>(&isRead), sizeof(bool));
            length -= sizeof(std::uint8_t) + sizeof(std::uint8_t) + textSize + sizeof(textSize) + sizeof(bool);

            SmsEntity tempSms = {phoneNumberFrom, phoneNumberTo, text, isRead};
            returnSmsVector.push_back(tempSms);
        }
        this->logger.logInfo("Odczytano!");
    }
    this->file.close();

    return returnSmsVector;
}
}
