#include "UserPort.hpp"
#include "UeGui/IListViewMode.hpp"
#include "UeGui/ITextMode.hpp"
#include <string>
#include <type_traits>

namespace ue
{
using view_mode = view_details::GuiViewMode;
using view_mode_type = std::underlying_type_t<view_details::GuiViewMode>;

UserPort::UserPort(common::ILogger &logger, IUeGui &gui, common::PhoneNumber phoneNumber)
    : logger(logger, "[USER-PORT]"), gui(gui), phoneNumber(phoneNumber)
{}

void UserPort::start(IEventsHandler &handler)
{
    this->handler = &handler;
    gui.setTitle("Nokia " + to_string(phoneNumber));
    gui.setAcceptCallback(std::bind(&UserPort::acceptCallback, this));
    gui.setRejectCallback(std::bind(&UserPort::rejectCallback, this));
}

void UserPort::stop()
{
    handler = nullptr;

    gui.setAcceptCallback(nullptr);
    gui.setRejectCallback(nullptr);
}

void UserPort::showNotConnected()
{
    currentViewMode = view_mode::Default;
    gui.showNotConnected();
}

void UserPort::showConnecting()
{
    currentViewMode = view_mode::Default;
    gui.showConnecting();
}

void UserPort::showConnected()
{
    currentViewMode = view_mode::Menu;
    IUeGui::IListViewMode &menu = gui.setListViewMode();
    menu.clearSelectionList();
    menu.addSelectionListItem("Compose SMS", "");
    menu.addSelectionListItem("View SMS", "");
    menu.addSelectionListItem("Call", "");

    gui.showConnected();
}

void UserPort::messageCallback()
{
    if (!handler)
        return;

    logger.logDebug("Message button pressed (SMS functionality)");

    if (currentViewMode == view_mode::Message_compose)
    {
        logger.logInfo("Message button in compose mode - sending message");
        auto reciver = getMessageRecipient();
        auto text = getMessageText();

        if (!reciver.isValid() || text.empty())
        {
            showNotify("Error", "Invalid reciver or empty text");
            return;
        }
        handler->handleMessageComposeResult(reciver, text);

        gui.setSmsComposeMode().clearSmsText();
    }
    else
    {
        currentViewMode = view_mode::Message_menu;
        IUeGui::IListViewMode &menu = gui.setListViewMode();
        menu.clearSelectionList();
        menu.addSelectionListItem("Compose SMS", "Send a new text message");
        menu.addSelectionListItem("View SMS", "Read received messages");
    }
}

void UserPort::showNewMessage()
{
    logger.logInfo("New Message has arrived");
    gui.showNewSms(true);
}

void UserPort::showListMessage(const std::vector<SmsMessage> &messages)
{
    currentViewMode = view_mode::Message_list;
    logger.logInfo("Showing SMS List (Count: ", messages.size(), ")");
    IUeGui::IListViewMode &menu = gui.setListViewMode();
    menu.clearSelectionList();

    for (const auto &sms: messages)
    {
        bool isRead = (sms.direction == SmsMessage::Dir::in) ? (sms.status == SmsMessage::Status::receiveR) : true;

        std::string prefix = isRead ? "  " : "* ";

        std::string directionLabel = (sms.direction == SmsMessage::Dir::in) ? "From: " : "To: ";

        std::string label = prefix + directionLabel + common::to_string(sms.sender);

        if (sms.direction == SmsMessage::Dir::out && sms.status == SmsMessage::Status::failed)
        {
            label += " [FAILED]";
        }

        menu.addSelectionListItem(label, sms.text);
    }
}

void UserPort::showMessageView(const SmsMessage &message)
{
    currentViewMode = view_mode::Message_view;

    std::string labelPrefix = (message.direction == SmsMessage::Dir::in) ? "From: " : "To: ";
    logger.logInfo(labelPrefix, message.sender);

    IUeGui::ITextMode &viewer = gui.setViewTextMode();

    std::string displayText = labelPrefix + common::to_string(message.sender);
    displayText += "\n\n--- Treść wiadomości ---\n";
    displayText += message.text;

    if (message.direction == SmsMessage::Dir::out)
    {
        std::string statusText = (message.status == SmsMessage::Status::failed) ? " [FAILED]" : "";
        displayText += statusText;
    }
    logger.logDebug("Wyświetlam wiadomość: ", displayText);
    viewer.setText(displayText);
}

void UserPort::showNotify(const std::string &title, const std::string &message)
{

    currentViewMode = view_mode::Default;
    logger.logInfo("Showing Alert: ", title);
    IUeGui::ITextMode &alerter = gui.setAlertMode();
    alerter.setText(title + "\n\n" + message);
}

void UserPort::showMessageComp()
{
    currentViewMode = view_mode::Message_compose;
    logger.logInfo("Showing SMS Compose screen");
    auto &composeMode = gui.setSmsComposeMode();

    composeMode.clearSmsText();
}

void UserPort::showIncomingCall(const common::PhoneNumber &caller)
{}
void UserPort::showCallInProgress(const common::PhoneNumber &otherPhoneNumber)
{}
void UserPort::showEndedCall(const common::PhoneNumber &otherPhoneNumber, const std::string &reason)
{}
void UserPort::showCallFailed(const common::PhoneNumber &otherPhoneNumber, const std::string &errorMessage)
{}

void UserPort::showCallMenu()
{
    currentViewMode = view_mode::Call_menu;
    IUeGui::IListViewMode &menu = gui.setListViewMode();
    menu.clearSelectionList();
    menu.addSelectionListItem("Dial Number", "Enter a number to call");
    menu.addSelectionListItem("Call History", "View recent calls");
    gui.showConnected(); // zmienić metodę
}

void UserPort::acceptCallback()
{
    if (!handler)
        return;

    std::optional<std::size_t> selectedIndexOpt;

    if (currentViewMode == view_mode::Menu)
    {
        logger.logDebug("Accept in main menu - getting selected index");
        auto &listView = gui.setListViewMode();
        auto indexPair = listView.getCurrentItemIndex();
        selectedIndexOpt = indexPair.first ? std::optional<std::size_t>(indexPair.second) : std::nullopt;
    }
    else if (currentViewMode == view_mode::Message_menu)
    {
        logger.logDebug("Accept in SMS menu - getting selected index");
        auto &listView = gui.setListViewMode();
        auto indexPair = listView.getCurrentItemIndex();
        selectedIndexOpt = indexPair.first ? std::optional<std::size_t>(indexPair.second) : std::nullopt;

        if (selectedIndexOpt.has_value())
        {
            if (selectedIndexOpt.value() == 0)
            {
                logger.logInfo("Compose SMS selected from SMS menu");
                showMessageComp();
                selectedIndexOpt = std::nullopt;
            }
        }
    }
    else if (currentViewMode == view_mode::Call_menu)
    {
        logger.logDebug("Accept in Call menu - getting selected index");
        auto &listView = gui.setListViewMode();
        auto indexPair = listView.getCurrentItemIndex();
        selectedIndexOpt = indexPair.first ? std::optional<std::size_t>(indexPair.second) : std::nullopt;

        if (selectedIndexOpt.has_value())
        {
            if (selectedIndexOpt.value() == 0)
            {
                logger.logInfo("Dial Number selected from Call menu");
                // TODO: Implement dial number UI or action
                selectedIndexOpt = std::nullopt;
            }
            else if (selectedIndexOpt.value() == 1)
            {
                logger.logInfo("Call History selected from Call menu");
                // TODO: Implement call history UI or action
                selectedIndexOpt = std::nullopt;
            }
        }
    }
    else if (currentViewMode == view_mode::Message_list)
    {
        logger.logDebug("Accept in SMS list - getting selected SMS");
        auto &listView = gui.setListViewMode();
        auto indexPair = listView.getCurrentItemIndex();
        selectedIndexOpt = indexPair.first ? std::optional<std::size_t>(indexPair.second) : std::nullopt;
    }
    else if (currentViewMode == view_mode::Message_compose)
    {
        logger.logDebug("Accept in SMS compose - sending message");
        selectedIndexOpt = std::nullopt;
    }

    logger.logDebug("Sending UI action to handler, mode: ", static_cast<view_mode_type>(currentViewMode));
    handler->handleUiAction(selectedIndexOpt);
}

void UserPort::rejectCallback()
{
    if (!handler)
        return;
    logger.logDebug("UI Action (Reject/Back), Mode: ", static_cast<view_mode_type>(currentViewMode));

    handler->handleUiBack();
}

common::PhoneNumber UserPort::getMessageRecipient() const
{
    return gui.setSmsComposeMode().getPhoneNumber();
}

std::string UserPort::getMessageText() const
{
    return gui.setSmsComposeMode().getSmsText();
}

common::PhoneNumber UserPort::getCallRecipient() const
{
    return gui.setSmsComposeMode().getPhoneNumber();
}

}