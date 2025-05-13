#include "CallState.hpp"

namespace ue
{
DiallingState::DiallingState(Context& context) : BaseState(context, "DiallingState") 
{
    this->logger.logInfo("Entered DiallingState");
};
}