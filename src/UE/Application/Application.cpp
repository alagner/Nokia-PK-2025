#include "Application.hpp"
#include "States/NotConnectedState.hpp"

namespace ue
{

Application::Application(common::PhoneNumber phoneNumber,
                         common::ILogger &iLogger,
                         IBtsPort &bts,
                         IUserPort &user,
                         ITimerPort &timer,
                         ISmsRepository &smsDb)
    : context{iLogger, bts, user, timer, smsDb},
      logger(iLogger, "[APP] "),
      phoneNumber(phoneNumber)
{
    logger.logInfo("Started");
    context.setState<NotConnectedState>();
}

Application::~Application()
{
    logger.logInfo("Stopped");
}

void Application::viewSmsList()
{
    context.state->viewSmsList();
}

void Application::viewSms(unsigned int index)
{
    context.state->viewSms(index);
}

void Application::sendSms(const SmsEntity& sms)
{
    context.state->sendSms(sms);
}

void Application::composeSms()
{
    context.state->composeSms();
}

void Application::handleTimeout()
{
    context.state->handleTimeout();
}

void Application::handleSib(common::BtsId btsId)
{
    context.state->handleSib(btsId);
}

void Application::handleAttachAccept()
{
    context.state->handleAttachAccept();
}

void Application::handleSms(common::PhoneNumber from, std::string text)
{
    logger.logInfo("SMS received from: ", from);
    context.smsDb.save(SmsEntity(from.value,phoneNumber.value,text));
    context.state->handleSms(from, text);
}

void Application::handleAttachReject()
{
    context.state->handleAttachReject();
}

void Application::handleDisconnect()
{
    context.state->handleDisconnect();
}

void Application::handleSmsDeliveryFailure(common::PhoneNumber from)
{
    context.state->handleSmsDeliveryFailure(from);
}

}
