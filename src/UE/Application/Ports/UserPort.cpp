#include "UserPort.hpp"
#include "UeGui/IListViewMode.hpp"
#include "UeGui/ITextMode.hpp"
#include "SmsRepository/SmsRepository.h"
#include "IUeGui.hpp"
#include "UeGui/ISmsComposeMode.hpp"


namespace ue
{

UserPort::UserPort(common::ILogger &logger, IUeGui &gui, common::PhoneNumber phoneNumber)
    : logger(logger, "[USER-PORT]"),
      gui(gui),
      phoneNumber(phoneNumber)
{}

void UserPort::start(IUserEventsHandler &handler)
{
    this->handler = &handler;
    gui.setTitle("Nokia " + to_string(phoneNumber));
}

void UserPort::stop()
{
    handler = nullptr;
}

void UserPort::showNotConnected()
{
    gui.showNotConnected();
}

void UserPort::showConnecting()
{
    gui.showConnecting();
}

void UserPort::showConnected()
{
    IUeGui::IListViewMode& menu = gui.setListViewMode();
    menu.clearSelectionList();
    menu.addSelectionListItem("Compose SMS", "");
    menu.addSelectionListItem("View SMS", "");

    gui.setAcceptCallback([this, &menu](){
        auto listIndex = menu.getCurrentItemIndex();

        if (listIndex.first){
            switch (listIndex.second){
                case 0: {
                    handler->composeSms();
                    break;
                }
                case 1: {
                    handler->viewSmsList();
                    break;
                }
            }
        }
        else{
            logger.logError("There is no such choice.");
        }
    });

    gui.setRejectCallback([this](){});
}

void UserPort::showNewSms()
{
    logger.log(common::ILogger::INFO_LEVEL, "New SMS received!");
    gui.showNewSms(true);
}

void UserPort::showSms(const SmsEntity & sms)
{
    IUeGui::ITextMode& smsScreen = gui.setViewTextMode();
    smsScreen.setText(sms.text);

    gui.setRejectCallback([this](){
        handler->viewSmsList();
    });

    gui.setAcceptCallback([this](){});
}

void UserPort::showSmsList(const std::vector<SmsEntity> & smsList)
{
    IUeGui::IListViewMode& menu = gui.setListViewMode();
    menu.clearSelectionList();
    gui.showNewSms(false);
    int smsAmount = smsList.size();

    for (auto sms = smsList.rbegin(); sms != smsList.rend(); ++sms){
        if (sms->from == phoneNumber.value)
            menu.addSelectionListItem("Sms to: " + std::to_string(sms->to), "");
        else{
            if (!sms->isRead)
                menu.addSelectionListItem("Sms from: " + std::to_string(sms->from) + " •", "");
            else
                menu.addSelectionListItem("Sms from: " + std::to_string(sms->from), "");
        }
    }

    gui.setAcceptCallback([this, &menu, smsAmount](){
        auto listIndex = menu.getCurrentItemIndex();

        if (listIndex.first){
            unsigned int realSmsIndex = smsAmount - listIndex.second - 1;
            handler->viewSms(realSmsIndex);
        }
        else
            logger.logError("There is no such SMS.");
    });

    gui.setRejectCallback([this](){
        showConnected();
    });
}

void UserPort::composeSms()
{
    IUeGui::ISmsComposeMode& composeMode = gui.setSmsComposeMode();
    composeMode.clearSmsText();

    gui.setAcceptCallback([this, &composeMode](){
        auto number = composeMode.getPhoneNumber();
        auto text = composeMode.getSmsText();
        SmsEntity sms(phoneNumber.value, number.value, text, false);
        handler->sendSms(sms);
    });

    gui.setRejectCallback([this](){
        showConnected();
    });
}

common::PhoneNumber UserPort::getPhoneNumber() const
{
    return phoneNumber;
}

}
