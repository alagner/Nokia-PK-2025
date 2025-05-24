#include "UserPort.hpp"
#include "UeGui/IListViewMode.hpp"
#include "UeGui/ITextMode.hpp"
#include "SmsRepository/SmsRepository.h"
#include "IUeGui.hpp"
#include "UeGui/ISmsComposeMode.hpp"
#include "UeGui/IDialMode.hpp"
#include "UeGui/ICallMode.hpp"

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
    logger.logInfo("Transition to Connected View");
    IUeGui::IListViewMode& menu = gui.setListViewMode();
    menu.clearSelectionList();
    menu.addSelectionListItem("Compose SMS", "");
    menu.addSelectionListItem("View SMS", "");
    menu.addSelectionListItem("Make Call","");

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
                case 2: {
                    handler->startDial();
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

void UserPort::startDial()
{
    IUeGui::IDialMode& dialScreen = gui.setDialMode();
    gui.setAcceptCallback([this, &dialScreen]() {
        common::PhoneNumber number = dialScreen.getPhoneNumber();
        handler->sendCallRequest(number);
    });
    gui.setRejectCallback([this]() {
        showConnected();
    });
}

void UserPort::showDialing()
{
    IUeGui::ITextMode& mode = gui.setAlertMode();
    mode.setText("Dialing...");
    gui.setRejectCallback([this]() {
        logger.logInfo("User canceled the call");
        handler->cancelCallRequest();
    });
}

void UserPort::showTalking()
{
    IUeGui::ICallMode& chat = gui.setCallMode();
    chat.clearIncomingText();

    gui.setAcceptCallback([this, &chat]() {
        std::string message = chat.getOutgoingText();
        if (!message.empty()) {
            chat.appendIncomingText("You: " + message);
            handler->sendTalkMessage(message);
            chat.clearOutgoingText();
        }
    });

    gui.setRejectCallback([this]() {
        logger.logInfo("User ended the call");
        handler->cancelCallRequest();
    });
}

void UserPort::showPartnerNotAvailable()
{
    IUeGui::ITextMode& mode = gui.setAlertMode();
    mode.setText("Partner is not available.");
    gui.setRejectCallback([this]() {

    });
    gui.setAcceptCallback([this]() {

    });
}

void UserPort::displayMessage(common::PhoneNumber from, const std::string& text)
{
    IUeGui::ICallMode& chat = gui.setCallMode();
    chat.appendIncomingText("Peer: " + text);
}

void UserPort::showCallRequest(common::PhoneNumber from)
{
    IUeGui::ITextMode& mode = gui.setAlertMode();
    mode.setText("Call request ...");
    gui.setAcceptCallback([this,from]() {
        handler->callAccept(from);
    });

    gui.setRejectCallback([this, from]() {
        handler->callDrop(from);
    });
}

}
