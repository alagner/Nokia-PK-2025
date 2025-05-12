#include "BaseState.hpp"

namespace ue
{

    BaseState::BaseState(Context& context, const std::string& stateName)
        : context(context),
          logger(context.logger, "[" + stateName + "]")
    {
        logger.logDebug("Entering state");
    }

    BaseState::~BaseState()
    {
        logger.logDebug("Exiting state");
    }

    void BaseState::handleTimeout()
    {
        logger.logError("Unexpected event: handleTimeout");
    }

    void BaseState::handleSib(common::BtsId btsId)
    {
        logger.logError("Unexpected event: handleSib(", btsId, ")");
    }

    void BaseState::handleAttachAccept()
    {
        logger.logError("Unexpected event: handleAttachAccept");
    }

    void BaseState::handleAttachReject()
    {
        logger.logError("Unexpected event: handleAttachReject");
    }

    void BaseState::handleDisconnected()
    {
        logger.logError("Unexpected event: handleDisconnected");
    }

    void BaseState::handleSmsReceive(common::PhoneNumber from, std::string messageText)
    {
        logger.logError("Unexpected SMS received from: ", from, ", text: ", messageText);
    }

    void BaseState::handleUiAction(std::optional<std::size_t> selectedIndex)
    {
        std::string indexStr = selectedIndex ? std::to_string(*selectedIndex) : "none";
        logger.logError("Unexpected UI action. Selected index: ", indexStr);
    }

    void BaseState::handleUiBack()
    {
        logger.logError("Unexpected UI back action");
    }

    void BaseState::handleSmsSent(common::PhoneNumber to, bool succeed)
    {
        logger.logError("Unexpected: handleSmsSentResult for: ", to, ", Success: ", succeed);
    }

    void BaseState::handleSmsCompose(common::PhoneNumber to, const std::string &textMessage)
    {
        logger.logError("Unexpected: handleSmsComposeResult for: ", to, ", text: ", textMessage);
    }

}
