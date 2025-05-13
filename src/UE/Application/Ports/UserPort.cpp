#include "UserPort.hpp"
#include "UeGui/IListViewMode.hpp"
#include "UeGui/ITextMode.hpp"
#include "UeGui/IDialMode.hpp"
#include "UeGui/ICallMode.hpp"

#include <string>

namespace ue
{

    UserPort::UserPort(common::ILogger &logger, IUeGui &gui, common::PhoneNumber phoneNumber)
        : logger(logger, "[USER-PORT]"),
          gui(gui),
          phoneNumber(phoneNumber)
    {
    }

    void UserPort::start(IEventsHandler &handler)
    {
        this->handler = &handler;
        gui.setTitle("Nokia " + common::to_string(phoneNumber));

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
        currentViewMode = details::VIEW_MODE_UNKNOWN;
        gui.showNotConnected();
    }

    void UserPort::showConnecting()
    {
        currentViewMode = details::VIEW_MODE_UNKNOWN;
        gui.showConnecting();
    }

    void UserPort::showConnected()
    {
        currentViewMode = details::VIEW_MODE_MAIN_MENU;
        logger.logInfo("Showing Main Menu");
        IUeGui::IListViewMode &menu = gui.setListViewMode();
        menu.clearSelectionList();

        menu.addSelectionListItem("Compose SMS", "Send a new text message");
        menu.addSelectionListItem("View SMS", "Read received messages");
        menu.addSelectionListItem("Dial", "Dial a phone number");

        gui.showConnected();
    }

    void UserPort::mailCallback()
    {
        acceptCallback();
    }

    void UserPort::showNewSms()
    {
        logger.logInfo("New SMS notification");
        gui.showNewSms(true);
    }

    void UserPort::showSmsList(const std::vector<SmsMessage> &messages)
    {
        currentViewMode = details::VIEW_MODE_SMS_LIST;
        logger.logInfo("Showing SMS List (Count: ", messages.size(), ")");
        IUeGui::IListViewMode &menu = gui.setListViewMode();
        menu.clearSelectionList();

        for (const auto &sms : messages)
        {
            bool isRead = (sms.direction == SmsMessage::Direction::INCOMING) ? (sms.status == SmsMessage::Status::RECEIVED_READ) : true;

            std::string prefix = isRead ? "  " : "* ";

            std::string directionLabel = (sms.direction == SmsMessage::Direction::INCOMING) ? "From: " : "To: ";

            std::string label = prefix + directionLabel + common::to_string(sms.peer);

            if (sms.direction == SmsMessage::Direction::OUTGOING && sms.status == SmsMessage::Status::FAILED)
            {
                label += " [FAILED]";
            }

            menu.addSelectionListItem(label, sms.text);
        }
    }

    void UserPort::showSmsView(const SmsMessage &message)
    {
        currentViewMode = details::VIEW_MODE_SMS_VIEW;

        std::string labelPrefix = (message.direction == SmsMessage::Direction::INCOMING) ? "From: " : "To: ";
        logger.logInfo(labelPrefix, message.peer);

        IUeGui::ITextMode &viewer = gui.setViewTextMode();

        std::string displayText = labelPrefix + common::to_string(message.peer);
        displayText += "\n\n--- Treść wiadomości ---\n";
        displayText += message.text;

        if (message.direction == SmsMessage::Direction::OUTGOING)
        {
            std::string statusText = (message.status == SmsMessage::Status::FAILED) ? " [FAILED]" : "";
            displayText += statusText;
        }
        logger.logDebug("Wyświetlam wiadomość: ", displayText);
        viewer.setText(displayText);
    }

    void UserPort::showAlert(const std::string &title, const std::string &message)
    {

        currentViewMode = details::VIEW_MODE_UNKNOWN;
        logger.logInfo("Showing Alert: ", title);
        IUeGui::ITextMode &alerter = gui.setAlertMode();
        alerter.setText(title + "\n\n" + message);
    }

    void UserPort::showSmsCompose()
    {
        currentViewMode = details::VIEW_MODE_SMS_COMPOSE;
        logger.logInfo("Showing SMS Compose screen");
        currentSmsMode = &gui.setSmsComposeMode();
        currentSmsMode->clearSmsText();
    }

    void UserPort::showIncomingCall(common::PhoneNumber from)
    {
        currentViewMode = details::VIEW_MODE_UNKNOWN;
        logger.logInfo("Showing incoming call screen from: ", from);
        auto &callMode = gui.setCallMode();
        callMode.appendIncomingText("Incoming call from: " + common::to_string(from));
    }

    void UserPort::showTalkingScreen(common::PhoneNumber peer)
    {
        currentViewMode = details::VIEW_MODE_UNKNOWN;
        logger.logInfo("Showing talking screen with: ", peer);
        auto &callMode = gui.setCallMode();
        callMode.appendIncomingText("Connected with: " + common::to_string(peer));
    }

    void UserPort::showDialMode()
    {
        currentViewMode = details::VIEW_MODE_DIAL;
        logger.logInfo("Going into dialing.");
        gui.setDialMode();
    }

    void UserPort::acceptCallback()
    {
        if (!handler)
            return;

        std::optional<std::size_t> selectedIndexOpt;

        if (currentViewMode == details::VIEW_MODE_MAIN_MENU)
        {
            logger.logDebug("Accept in main menu - getting selected index");
            auto &listView = gui.setListViewMode();
            auto indexPair = listView.getCurrentItemIndex();
            selectedIndexOpt = indexPair.first ? std::optional<std::size_t>(indexPair.second) : std::nullopt;
        }
        else if (currentViewMode == details::VIEW_MODE_SMS_MENU)
        {
            logger.logDebug("Accept in SMS menu - getting selected index");
            auto &listView = gui.setListViewMode();
            auto indexPair = listView.getCurrentItemIndex();
            selectedIndexOpt = indexPair.first ? std::optional<std::size_t>(indexPair.second) : std::nullopt;

            if (selectedIndexOpt.has_value() && currentViewMode == details::VIEW_MODE_SMS_MENU)
            {
                if (selectedIndexOpt.value() == 0)
                {
                    logger.logInfo("Compose SMS selected from SMS menu");
                    showSmsCompose();
                    selectedIndexOpt = std::nullopt;
                }
            }
        }
        else if (currentViewMode == details::VIEW_MODE_SMS_LIST)
        {
            logger.logDebug("Accept in SMS list - getting selected SMS");
            auto &listView = gui.setListViewMode();
            auto indexPair = listView.getCurrentItemIndex();
            selectedIndexOpt = indexPair.first ? std::optional<std::size_t>(indexPair.second) : std::nullopt;
        }
        else if (currentViewMode == details::VIEW_MODE_SMS_COMPOSE)
        {
            logger.logDebug("Accept in SMS compose - sending message");
            selectedIndexOpt = std::nullopt;
        }
        else if (currentViewMode == details::VIEW_MODE_UNKNOWN)
        {
            logger.logDebug("Accept in UNKNOWN view (likely alert) — forwarding to handler");
            selectedIndexOpt = std::nullopt;
        }

        logger.logDebug("Sending UI action to handler, mode: ", currentViewMode);
        handler->handleUiAction(selectedIndexOpt);
    }
    void UserPort::rejectCallback()
    {
        if (!handler)
            return;
        logger.logDebug("UI Action (Reject/Back), Mode: ", currentViewMode);

        handler->handleUiBack();
    }

    common::PhoneNumber UserPort::getSmsRecipient() const
    {
        return currentSmsMode ? currentSmsMode->getPhoneNumber() : common::PhoneNumber{};
    }

    std::string UserPort::getSmsText() const
    {
        return currentSmsMode ? currentSmsMode->getSmsText() : "";
    }
    std::string UserPort::getCallText() const
    {
        return gui.setCallMode().getOutgoingText();
    }

    void UserPort::appendIncomingText(const std::string &text)
    {
        gui.setCallMode().appendIncomingText(text);
    }

    void UserPort::clearOutgoingText()
    {
        gui.setCallMode().clearOutgoingText();
    }

    common::PhoneNumber UserPort::getDialedPhoneNumber() const
    {
        return gui.setDialMode().getPhoneNumber();
    }

}
