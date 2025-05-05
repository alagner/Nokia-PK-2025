#include "TalkingState.hpp"
#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"
#include "UeGui/ICallMode.hpp"

namespace ue {

TalkingState::TalkingState(Context& ctx, common::PhoneNumber peer)
    : BaseState(ctx, "TalkingState"), peer(peer)
{
    context.user.showTalkingOverlay();

    IUeGui::ICallMode& ui = context.user.showCallMode();
    ui.clearIncomingText();
    ui.clearOutgoingText();

    context.bts.sendCallAccepted(peer);

    ui.appendIncomingText("Talking");
}

void TalkingState::handleUserAction(const std::string& id)
{
    if (id == "ACCEPT") {
        IUeGui::ICallMode& ui = context.user.showCallMode();
        std::string out = ui.getOutgoingText();
        context.bts.sendCallTalk(peer, out);
        ui.clearOutgoingText();
        ui.appendIncomingText("> " + out);
    }
    else if (id == "REJECT") {
        context.bts.sendCallDropped(peer);
        context.setState<ConnectedState>();
    }
    else {
        BaseState::handleUserAction(id);
    }
}

void TalkingState::handleCallTalk(common::PhoneNumber from, const std::string& text)
{
    IUeGui::ICallMode& ui = context.user.showCallMode();
    ui.appendIncomingText(text);
}

void TalkingState::handleCallDropped(common::PhoneNumber)
{
    context.setState<ConnectedState>();
}

void TalkingState::handleDisconnect()
{
    context.setState<NotConnectedState>();
}

void TalkingState::handleUnknownRecipient(common::MessageId, common::PhoneNumber)
{
    context.setState<ConnectedState>();
}

} // namespace ue
