#include "ComposeSmsState.hpp"
#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"

namespace ue {

ComposeSmsState::ComposeSmsState(Context& ctx)
    : BaseState(ctx, "ComposeSmsState")
{
    logger.logInfo("ComposeSmsState: entering SMS composition");
    context.user.showMessageComp();
}

void ComposeSmsState::handleUiAction(std::optional<std::size_t>)
{
    logger.logInfo("ComposeSmsState: user tapped Send");
    validateAndSendSms();
}

void ComposeSmsState::validateAndSendSms()
{
    const auto recipient = context.user.getMessageRecipient();
    const auto body      = context.user.getMessageText();

    if (!inputsAreValid(recipient, body)) {
        logger.logInfo("ComposeSmsState: invalid recipient or empty text");
        context.user.showNotify("Error", "Invalid recipient or empty message");
        return;
    }

    context.smsStorage.addSentMessage(recipient, body, SmsMessage::Status::sent);
    context.bts.sendMessage(recipient, body);
    logger.logInfo("ComposeSmsState: SMS queued to ", recipient);

    context.setState<ConnectedState>();
}

bool ComposeSmsState::inputsAreValid(const common::PhoneNumber& recipient,
                                     const std::string& body) const
{
    return recipient.isValid() && !body.empty();
}

void ComposeSmsState::handleUiBack()
{
    logger.logInfo("ComposeSmsState: composition canceled");
    context.setState<ConnectedState>();
}

void ComposeSmsState::handleDisconnected()
{
    logger.logInfo("ComposeSmsState: lost connection mid-compose");
    context.user.showNotify("Disconnected",
        "Connection lost during SMS composition.");
    context.setState<NotConnectedState>();
}

void ComposeSmsState::handleMessageReceive(common::PhoneNumber from, std::string text)
{
    logger.logInfo("ComposeSmsState: incoming SMS from ", from);
    onIncomingSms(from, text);
}

void ComposeSmsState::onIncomingSms(common::PhoneNumber from, const std::string& body)
{
    context.smsStorage.addMessage(from, body);
    context.user.showNewMessage();
}

void ComposeSmsState::handleMessageSentResult(common::PhoneNumber to, bool)
{
    logger.logInfo("Sms sent result for: ", to, " received while composing - ignoring");
}

}
