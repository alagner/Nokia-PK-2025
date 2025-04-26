#pragma once

#include "Ports/IBtsPort.hpp"
#include "Ports/IUserPort.hpp"
#include "Ports/ITimerPort.hpp"
#include "Messages/PhoneNumber.hpp"
#include <optional> // Include for optional

namespace ue
{
    // Forward declaration if needed, or include specific GUI mode headers
    namespace details
    {
        using GuiViewMode = int; // Or an enum class
        constexpr GuiViewMode VIEW_MODE_UNKNOWN = 0;
        constexpr GuiViewMode VIEW_MODE_MAIN_MENU = 1;
        constexpr GuiViewMode VIEW_MODE_SMS_LIST = 2;
        constexpr GuiViewMode VIEW_MODE_SMS_VIEW = 3;
        constexpr GuiViewMode VIEW_MODE_SMS_COMPOSE = 4;
        constexpr GuiViewMode VIEW_MODE_SMS_MENU = 5;
        constexpr GuiViewMode VIEW_MODE_DIAL = 6;

        // Add others like COMPOSE, DIAL etc. as needed
    }

    class IEventsHandler : public IBtsEventsHandler,
                           public ITimerEventsHandler,
                           public IUserEventsHandler
    {
    public:
        virtual ~IEventsHandler() = default;

        // New UI Interaction Handlers
        // Called when user presses "Accept/OK/Select" in the current view
        virtual void handleUiAction(std::optional<std::size_t> selectedIndex) = 0;
        // Called when user presses "Reject/Back/Cancel" in the current view
        virtual void handleUiBack() = 0;
        // virtual void handleSmsReceived(common::PhoneNumber from, std::string text) = 0;
        // virtual void handleSmsSentResult(common::PhoneNumber to, bool success) = 0;
        virtual void handleSmsComposeResult(common::PhoneNumber recipient, const std::string &text) = 0;
        virtual void handleCallRequest(common::PhoneNumber from) = 0;
        virtual void handleCallDropped(common::PhoneNumber peer) = 0;
        virtual void handleUnknownRecipient(common::PhoneNumber peer) = 0;
        virtual void handleCallTalk(common::PhoneNumber from, const std::string &text) = 0;
        virtual void handleCallReject(common::PhoneNumber from) = 0;
    };

}