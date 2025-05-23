#include "IncomingCallState.hpp"
#include "ConnectedState.hpp"
#include "OutgoingCallState.hpp"
#include "NotConnectedState.hpp"

namespace ue
{

    IncomingCallState::IncomingCallState(Context& context, common::PhoneNumber from)
        : BaseState(context, "IncomingCallState"), caller(from)
    {
        logger.logInfo("Incoming call from: ", caller);
        context.user.showIncomingCall(from);
        context.timer.startTimer(TIMEOUT);
    }

    IncomingCallState::~IncomingCallState()
    {
        context.timer.stopTimer();
    }

    void IncomingCallState::handleUiAction(std::optional<std::size_t> chosenIndex)
    {
        logger.logInfo("Call accepted from: ", caller);
        context.timer.stopTimer();
        context.bts.sendAcceptCall(caller);
        context.setState<OutgoingCallState>(caller);
    }

    void IncomingCallState::handleUiBack()
    {
        logger.logInfo("Call rejected from: ", caller);
        context.timer.stopTimer();
        context.bts.sendCallDropped(caller);
        context.setState<ConnectedState>();
    }

    void IncomingCallState::handleTimeout()
    {
        logger.logInfo("Incoming call timed out");
        context.timer.stopTimer();
        context.bts.sendCallDropped(caller);
        context.setState<ConnectedState>();
    }

    void IncomingCallState::handleDisconnected()
    {
        logger.logInfo("Lost connection during incoming call");
        context.timer.stopTimer();
        context.user.showAlert("Disconnected", "Connection lost during incoming call.");
        context.setState<NotConnectedState>();
    }

    void IncomingCallState::handleCallDropped(common::PhoneNumber to)
    {
      if(caller == to)
      {
          logger.logInfo("Call dropped from: ", to);
          context.timer.stopTimer();
          context.user.showConnected();
          context.setState<ConnectedState>();
      }
      else
      {
          logger.logInfo("Call dropped from unknown number: ", to);
      }
    }

    void IncomingCallState::handleCallReject(common::PhoneNumber to)
    {
        if(caller == to)
        {
            logger.logInfo("Call rejected from: ", to);
            context.timer.stopTimer();
            context.user.showAlert("Call Rejected", "Call was rejected.");
            context.setState<ConnectedState>();
        }
        else
        {
            logger.logInfo("Call rejected from unknown number: ", to);
        }
    }

    void IncomingCallState::handleSmsReceive(common::PhoneNumber from, std::string messageText)
    {
        logger.logInfo("SMS received from number: ", from);
        std::size_t index = context.smsDatabase.addIncomingSms(from, messageText);
        logger.logDebug("SMS added to database with index: ", index);
        context.user.showSms();
    }

    void IncomingCallState::handleCallRequest(common::PhoneNumber from)
    {
        if(from != caller)
        {
            logger.logInfo("Received call request from: ", from);
            context.bts.sendCallDropped(caller);
        }
        else
        {
            logger.logInfo("Received call request from the same number: ", from);
        }
        context.setState<IncomingCallState>(from);
    }

}