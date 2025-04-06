#include "ConnectedState.hpp"
#include <sstream>
namespace ue
{

ConnectedState::ConnectedState(Context &context)
    : BaseState(context, "ConnectedState")
{
    context.user.showConnected();
}
void ConnectedState::handleSms(common::PhoneNumber from, std::string text)
{
    context.logger.log(common::ILogger::INFO_LEVEL, "[ConnectedState] Received SMS from: ", from, ", text: ", text);

    context.user.showNewSms();
}

}
