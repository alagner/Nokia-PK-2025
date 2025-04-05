#include "SmsRepository.h"

namespace ue
{
SmsRepository::SmsRepository(common::ILogger &logger) : logger(logger, "[SMS REPOSITORY]")
{

}

void SmsRepository::save(const SmsEntity & newSms)
{
    std::uint8_t* nonConstFrom = const_cast<std::uint8_t*>(&newSms.from);
    int textSize = newSms.text.size();

    this->file.open("./database.dat", std::ios_base::binary | std::ios_base::app);
    if (this->file.is_open()){
        this->file.write(reinterpret_cast<char*>(nonConstFrom), sizeof(std::uint8_t));
        this->file.write(reinterpret_cast<char*>(&textSize), sizeof(textSize));
        this->file.write(newSms.text.c_str(), textSize);
        this->logger.Debug("Zapisano!");
    }
    this->file.close();
}

std::vector<SmsEntity> SmsRepository::getAll()
{
    std::vector<SmsEntity> returnSmsVector;
    std::uint8_t phoneNumber;

    std::string text;
    int textSize;

    this->file.open("./database.dat", std::ios_base::binary | std::ios_base::in);

    this->file.seekg (0, this->file.end);
    int length = this->file.tellg();
    this->file.seekg (0, this->file.beg);

    if (this->file.is_open()){
        while (length > 0){
            this->file.read(reinterpret_cast<char*>(&phoneNumber), sizeof(std::uint8_t));
            this->file.read(reinterpret_cast<char*>(&textSize), sizeof(textSize));
            text.resize(textSize);
            this->file.read(text.data(), textSize);
            length -= sizeof(std::uint8_t) + textSize + sizeof(textSize);

            SmsEntity tempSms = {phoneNumber, text};
            returnSmsVector.push_back(tempSms);
        }
        this->logger.logDebug("Odczytano!");
    }
    this->file.close();

    return returnSmsVector;
}
}
