#include "ComposingSmsState.hpp"
#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"
#include "Messages/PhoneNumber.hpp"
#include "IncomingCallState.hpp"

namespace ue
{

 ComposingSmsState::ComposingSmsState(Context& context)
     : BaseState(context, "ComposingSmsState")
 {
  logger.logInfo("Opened SMS compose screen");
  context.user.showSmsCompose(); // Show compose UI
 }

 void ComposingSmsState::handleUiAction(std::optional<std::size_t> selectedIndex)
 {
  logger.logInfo("User pressed accept - attempting to send SMS");

  auto recipient = context.user.getSmsRecipient();
  auto text = context.user.getSmsTextMessage();

  if (!recipient.isValid() || text.empty()) {
   logger.logInfo("Cannot proceed: recipient invalid or text empty");
   context.user.showAlert("Error", "Invalid recipient or empty message");
   return;
  }

  context.smsDatabase.addOutgoingSms(recipient, text, SmsMessage::Status::Sent);
  context.bts.sendSms(recipient, text);
  logger.logInfo("SMS sent request issued to: ", recipient);

  context.setState<ConnectedState>();
 }

 void ComposingSmsState::handleUiBack()
 {
  logger.logInfo("User canceled SMS composition - returning to menu");
  context.setState<ConnectedState>();
 }

 void ComposingSmsState::handleDisconnected()
 {
  logger.logInfo("Lost connection during SMS composition");
  context.user.showAlert("Disconnected", "Connection lost while composing SMS.");
  context.setState<NotConnectedState>();
 }

 void ComposingSmsState::handleSmsReceive(common::PhoneNumber from, std::string textMessage)
 {
  logger.logInfo("Received new SMS from ", from, " while composing another - saving");
  context.smsDatabase.addIncomingSms(from, textMessage);
  context.user.showSms();
 }

 void ComposingSmsState::handleSmsSent(common::PhoneNumber to, bool succeed)
 {
  logger.logInfo("Received SMS send status for ", to, " during compose - ignoring");
 }

 void ComposingSmsState::handleCallRequest(common::PhoneNumber from)
 {
  logger.logInfo("Received call request from ", from, " while composing SMS - ignoring");
  context.setState<IncomingCallState>(from);
 }

} // namespace ue
