#include "ComposeSmsState.hpp"
#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"
#include "Messages/PhoneNumber.hpp"

namespace ue{

    ComposeSmsState::ComposeSmsState(Context &context)
        : BaseState(context, "ComposingSmsState"){
        logger.logInfo("Entering SMS Composition");
        context.user.showMessageComp();
    }

    void ComposeSmsState::handleUiAction(std::optional<std::size_t> selectedIndex){
        logger.logInfo("Sending SMS initiated");

        auto reciver = context.user.getMessageRecipient();
        auto text = context.user.getMessageText();

        if (!reciver.isValid() || text.empty()) {
            logger.logInfo("Cannot send SMS: Invalid reciver or empty text");
            context.user.showNotify("Error", "Invalid reciver or empty text");
            return;
        }

        context.smsStorage.addSentMessage(reciver, text, SmsMessage::Status::sent);
        context.bts.sendMessage(reciver, text);
        logger.logInfo("SMS sending to: ", reciver);

        context.setState<ConnectedState>();
    }

    void ComposeSmsState::handleUiBack(){
        logger.logInfo("SMS composition cancelled by user");
        context.setState<ConnectedState>();
    }

    void ComposeSmsState::handleDisconnected(){
        logger.logInfo("Disconnected while composing SMS");
        context.user.showNotify("Disconnected", "Connection lost during SMS composition.");
        context.setState<NotConnectedState>();
    }

    void ComposeSmsState::handleMessageReceive(common::PhoneNumber from, std::string text){
        logger.logInfo("SMS received from ", from, " while composing - adding to inbox");
        context.smsStorage.addMessage(from, text);
        context.user.showNewMessage();
    }

    void ComposeSmsState::handleMessageSentResult(common::PhoneNumber to, bool success){
        logger.logInfo("SMS send result for ", to, " received while composing - ignoring");
    }

}
