#include "NotConnectedState.hpp"
#include "ConnectingState.hpp"

namespace ue
{

NotConnectedState::NotConnectedState(Context &context)
    : BaseState(context, "NotConnectedState")
{
    context.user.showNotConnected();
}

void NotConnectedState::handleSib(common::BtsId btsId)
{
    using namespace std::chrono_literals;
    context.timer.startTimer(500ms);
    context.bts.sendAttachRequest(btsId);
    context.setState<ConnectingState>();
}

void NotConnectedState::sendSms(const SmsEntity& sms)
{
    logger.logInfo("Cannot send SMS - not connected to the network");
    context.user.showNotConnected();
}

void NotConnectedState::composeSms()
{
    logger.logInfo("Cannot compose SMS - not connected to the network");
    context.user.showNotConnected();
}

void NotConnectedState::handleTalkMessage(common::PhoneNumber, const std::string&)
{
    logger.logInfo("Cannot handle talk message - not connected to the network");
    context.user.showNotConnected();
}

}
