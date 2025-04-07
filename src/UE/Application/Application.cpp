#include "Application.hpp" // Includes Context.hpp
#include "Context.hpp"     // Include again just to be safe? Not really needed.

// Include needed State headers here for setState calls and destructor context
#include "States/BaseState.hpp"
#include "States/NotConnectedState.hpp"
#include "States/ConnectingState.hpp"
#include "States/ConnectedState.hpp"
// Add other state headers if Application calls setState with them directly


namespace ue
{

// REMOVED: setState definition (moved to Context.cpp)

Application::Application(common::PhoneNumber phoneNumber,
                         common::ILogger &iLogger,
                         IBtsPort &bts,
                         IUserPort &user,
                         ITimerPort &timer)
    : context{iLogger, bts, user, timer, *this},
      logger(iLogger, "[APP] "),
      phoneNumber(phoneNumber)
{
    logger.logInfo("Started");
    // setState definition must be visible here. If it's in Context.cpp,
    // this call might require explicit instantiation in Context.cpp OR
    // the template definition needs to be accessible (e.g., move back to Context.hpp
    // but ensure Context.hpp only includes BaseState.fwd.hpp type header - complex).
    // Let's rely on the explicit instantiation in Context.cpp for now.
    context.setState<NotConnectedState>();
}

// Definition of the destructor
Application::~Application()
{
    logger.logInfo("Stopped");
    // Context destructor called here, needs definition of BaseState (included above)
}

// ... rest of the Application method implementations (no changes from previous version)...

void Application::handleTimeout()
{
    if (context.state) context.state->handleTimeout();
}

void Application::handleSib(common::BtsId btsId)
{
    if (context.state) context.state->handleSib(btsId);
}

void Application::handleAttachAccept()
{
    if (context.state) context.state->handleAttachAccept();
}

void Application::handleAttachReject()
{
    if (context.state) context.state->handleAttachReject();
}

void Application::handleDisconnect()
{
     logger.logInfo("Handle disconnect event from transport");
     context.timer.stopTimer();
     context.user.showNotConnected();
     // setState definition needs to be visible here too.
     context.setState<NotConnectedState>();
}


void Application::handleSms(const common::PhoneNumber& from, const std::string& text)
{
    if (context.state) context.state->handleSms(from, text);
}

void Application::handleUserAction(const std::string& id)
{
    logger.logDebug("User action: ", id);
    if (context.state) context.state->handleUserAction(id);
}


void Application::storeReceivedSms(const common::PhoneNumber& from, const std::string& text)
{
    logger.logInfo("Storing SMS from: ", from);
    smsDb.push_back({from, text, false});
    updateSmsIndicator();
}

const std::vector<data::SmsData>& Application::getSmsDb() const
{
    return smsDb;
}

void Application::markSmsAsRead(std::size_t index)
{
    if (index < smsDb.size())
    {
         logger.logInfo("Marking SMS at index ", index, " as read.");
         smsDb[index].isRead = true;
         updateSmsIndicator();
    }
    else
    {
         logger.logError("Attempted to mark invalid SMS index as read: ", index);
    }
}

void Application::updateSmsIndicator()
{
    bool unread_remain = false;
    for(const auto& sms : smsDb) {
        if (!sms.isRead) {
            unread_remain = true;
            break;
        }
    }
    logger.logInfo("Updating SMS indicator status: ", (unread_remain ? "ON" : "OFF"));
    context.user.showNewSms(unread_remain);
}


} // namespace ue