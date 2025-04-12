#pragma once

#include "Ports/ITimerPort.hpp"
#include "Ports/IBtsPort.hpp"
#include "Ports/IUserPort.hpp"
#include <string>


namespace ue
{

class IEventsHandler : public IBtsEventsHandler,
                       public IUserEventsHandler,
                       public ITimerEventsHandler
{
  public:
  virtual void handleCallRequest(common::PhoneNumber from) = 0;
  virtual void handleCallAccepted(common::PhoneNumber from) = 0;
  virtual void handleCallDropped(common::PhoneNumber from) = 0;
  virtual void handleCallTalk(common::PhoneNumber from, const std::string& text) = 0;

};

}
