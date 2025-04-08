#include "UserPort.hpp"
#include "UeGui/IListViewMode.hpp"


namespace ue
{

UserPort::UserPort(common::ILogger &logger, IUeGui &gui, common::PhoneNumber phoneNumber)
    : logger(logger, "[USER-PORT]"),
      gui(gui),
      phoneNumber(phoneNumber),
      smsDb(phoneNumber, this->logger)

{
}

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
}
void UserPort::showNewSms()
{
    logger.log(common::ILogger::INFO_LEVEL, "New SMS received!");
    gui.showNewSms(true);

}
void UserPort::addReceivedSms(common::PhoneNumber from, std::string text){
    smsDb.save(SmsEntity(from.value, phoneNumber.value,text));
}
}
