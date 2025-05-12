#include "UserPort.hpp"
#include "UeGui/IListViewMode.hpp"
#include "UeGui/ITextMode.hpp"
#include <string>

namespace ue
{

UserPort::UserPort(common::ILogger &logger, IUeGui &gui, common::PhoneNumber phoneNumber)
    : logger(logger, "[USER-PORT]"),
      gui(gui),
      phoneNumber(phoneNumber)
{}

void UserPort::start(IEventsHandler &handler)
{
    this->handler = &handler;
    gui.setTitle("UE Nokia " + common::to_string(phoneNumber));
    gui.setAcceptCallback(std::bind(&UserPort::onAccept, this));
    gui.setRejectCallback(std::bind(&UserPort::onReject, this));
}

void UserPort::stop()
{
    handler = nullptr;
    gui.setAcceptCallback(nullptr);
    gui.setRejectCallback(nullptr);
    // gui.setEnvelopeCallback(nullptr);
}

void UserPort::showNotConnected()
{
    currentViewMode = details::GuiViewMode::Unknown;
    gui.showNotConnected();
}

void UserPort::showConnecting()
{
    currentViewMode = details::GuiViewMode::Unknown;
    gui.showConnecting();
}

void UserPort::showConnected()
{
    currentViewMode = details::GuiViewMode::MainMenu;
    logger.logInfo("Displaying Main Menu");
    auto& menu = gui.setListViewMode();
    menu.clearSelectionList();
    menu.addSelectionListItem("Compose SMS", "Write new SMS");
    menu.addSelectionListItem("View SMS", "Check received messages");
    gui.showConnected();
}

void UserPort::showSms()
{
    logger.logInfo("New SMS message");
    gui.showNewSms(true);
}

void UserPort::showSmsList(const std::vector<SmsMessage>& messages)
{
    currentViewMode = details::GuiViewMode::SmsList;
    logger.logInfo("Showing inbox with ", messages.size(), " messages");

    auto& listView = gui.setListViewMode();
    listView.clearSelectionList();

    for (const auto& sms : messages)
    {
        bool isRead = (sms.direction == SmsMessage::Direction::Incoming && sms.status == SmsMessage::Status::ReceivedRead);

        std::string prefix = isRead ? "  " : "* ";
        std::string label = prefix + (sms.direction == SmsMessage::Direction::Incoming ? "From: " : "To: ") + common::to_string(sms.contact);

        if (sms.direction == SmsMessage::Direction::Outgoing && sms.status == SmsMessage::Status::Failed)
        {
            label += " [FAILED]";
        }

        listView.addSelectionListItem(label, sms.body);
    }
}

void UserPort::showSmsView(const SmsMessage& message)
{
    currentViewMode = details::GuiViewMode::SmsView;
    std::string labelPrefix = (message.direction == SmsMessage::Direction::Incoming) ? "From: " : "To: ";
    logger.logInfo(labelPrefix, message.contact);

    auto& viewer = gui.setViewTextMode();
    std::string displayText = labelPrefix + common::to_string(message.contact) + "\n\n--- Treść wiadomości ---\n" + message.body;

    if (message.direction == SmsMessage::Direction::Outgoing && message.status == SmsMessage::Status::Failed)
    {
        displayText += "\n[FAILED]";
    }

    logger.logDebug("Displaying message: ", displayText);
    viewer.setText(displayText);
}

void UserPort::showAlert(const std::string& title, const std::string& message)
{
    currentViewMode = details::GuiViewMode::Unknown;
    logger.logInfo("Alert: ", title);
    auto& alertView = gui.setAlertMode();
    alertView.setText(title + "\n\n" + message);
}

void UserPort::onAccept()
{
    if (!handler) return;

    std::optional<std::size_t> selectedIndex;

    if (currentViewMode == details::GuiViewMode::MainMenu ||
        currentViewMode == details::GuiViewMode::SmsList ||
        currentViewMode == details::GuiViewMode::SmsMenu)
    {
        auto& listView = gui.setListViewMode();
        auto [hasSelection, index] = listView.getCurrentItemIndex();
        if (hasSelection) selectedIndex = index;

        if (currentViewMode == details::GuiViewMode::SmsMenu && selectedIndex.has_value())
        {
            if (selectedIndex.value() == 0)
            {
                logger.logInfo("Compose SMS selected");
                showSmsCompose();
                selectedIndex = std::nullopt;
            }
        }
    }
    else if (currentViewMode == details::GuiViewMode::SmsCompose)
    {
        logger.logDebug("Accept pressed in SMS compose screen");
        selectedIndex = std::nullopt;
    }

    logger.logDebug("Accept triggered, mode: ", static_cast<int>(currentViewMode));
    handler->handleUiAction(selectedIndex);
}

void UserPort::onReject()
{
    if (!handler) return;
    logger.logDebug("Reject triggered, mode: ", static_cast<int>(currentViewMode));
    handler->handleUiBack();
}

void UserPort::onEnvelopeClicked()
{
    if (!handler) return;
    logger.logDebug("Envelope icon clicked");

    if (currentViewMode == details::GuiViewMode::SmsCompose)
    {
        logger.logInfo("Sending composed SMS");

        auto recipient = getSmsRecipient();
        auto text = getSmsTextMessage();

        if (!recipient.isValid() || text.empty())
        {
            showAlert("Error", "Invalid recipient or empty text.");
            return;
        }

        handler->handleSmsCompose(recipient, text);
        gui.setSmsComposeMode().clearSmsText();
    }
    else
    {
        currentViewMode = details::GuiViewMode::SmsMenu;
        auto& menu = gui.setListViewMode();
        menu.clearSelectionList();
        menu.addSelectionListItem("Compose SMS", "Write new SMS");
        menu.addSelectionListItem("View SMS", "Check received messages");
    }
}

void UserPort::showSmsCompose()
{
    currentViewMode = details::GuiViewMode::SmsCompose;
    logger.logInfo("Opening SMS compose window");

    auto& composeMode = gui.setSmsComposeMode();
    composeMode.clearSmsText();
}

common::PhoneNumber UserPort::getSmsRecipient()
{
    auto& composeMode = gui.setSmsComposeMode();
    return composeMode.getPhoneNumber();
}

std::string UserPort::getSmsTextMessage()
{
    auto& composeMode = gui.setSmsComposeMode();
    return composeMode.getSmsText();
}

} // namespace ue
