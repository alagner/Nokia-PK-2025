#pragma once
#include <gmock/gmock.h>
#include "UeGui/ICallMode.hpp"

namespace ue {

struct ICallModeMock : public IUeGui::ICallMode {
    MOCK_METHOD(void, appendIncomingText, (const std::string &text), (override));
    MOCK_METHOD(void, clearIncomingText, (), (override));
    MOCK_METHOD(void, clearOutgoingText, (), (override));
    MOCK_METHOD(std::string, getOutgoingText, (), (const, override));
};

} // namespace ue
