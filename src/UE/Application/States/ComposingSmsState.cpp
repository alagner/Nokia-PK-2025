#include "ComposingSmsState.hpp"
#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"
#include "Messages/PhoneNumber.hpp"
#include "IncomingCallState.hpp"

namespace ue
{

    ComposingSmsState::ComposingSmsState(Context &context)
        : BaseState(context, "ComposingSmsState")
    {
        logger.logInfo("Entering SMS Composition");
        context.user.showSmsCompose(); // Display the compose UI
    }

    void ComposingSmsState::handleUiAction(std::optional<std::size_t> selectedIndex)
    {
        logger.logInfo("Sending SMS initiated");

        auto recipient = context.user.getSmsRecipient();
        auto text = context.user.getSmsText();

        if (!recipient.isValid() || text.empty())
        {
            logger.logInfo("Cannot send SMS: Invalid recipient or empty text");
            context.user.showAlert("Error", "Invalid recipient or empty text");
            return;
        }

        context.smsDb.addSentSms(recipient, text, SmsMessage::Status::SENT);
        context.bts.sendSms(recipient, text);
        logger.logInfo("SMS sending to: ", recipient);

        context.setState<ConnectedState>();
    }

    void ComposingSmsState::handleUiBack()
    {
        logger.logInfo("SMS composition cancelled by user");
        context.setState<ConnectedState>();
    }

    void ComposingSmsState::handleDisconnected()
    {
        logger.logInfo("Disconnected while composing SMS");
        context.user.showAlert("Disconnected", "Connection lost during SMS composition.");
        context.setState<NotConnectedState>();
    }

    void ComposingSmsState::handleSmsReceived(common::PhoneNumber from, std::string text)
    {
        logger.logInfo("SMS received from ", from, " while composing - adding to inbox");
        context.smsDb.addReceivedSms(from, text);
        context.user.showNewSms();
    }

    void ComposingSmsState::handleSmsSentResult(common::PhoneNumber to, bool success)
    {
        logger.logInfo("SMS send result for ", to, " received while composing - ignoring");
    }

    void ComposingSmsState::handleCallRequest(common::PhoneNumber from)
    {
        logger.logInfo("Incoming call while composing SMS from: ", from);
        context.setState<IncomingCallState>(from);
    }

} // namespace ue