#ifndef TALKINGSTATE_H
#define TALKINGSTATE_H
#pragma once

#include "States/BaseState.hpp"

namespace ue {

class TalkingState : public BaseState
{
   public:
    explicit TalkingState(Context& context);
    void handleDisconnect() override;
};

}

#endif  // TALKINGSTATE_H
