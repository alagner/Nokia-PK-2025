#pragma once

#include "Application/IEventsHandler.hpp"
#include <gmock/gmock.h>
#include <optional>

namespace ue
{

    class IEventsHandlerMock : public IEventsHandler
    {
    public:
        // BTS-related event mocks
        MOCK_METHOD(void, handleSib, (common::BtsId), (override));
        MOCK_METHOD(void, handleAttachAccept, (), (override));
        MOCK_METHOD(void, handleAttachReject, (), (override));
        MOCK_METHOD(void, handleDisconnected, (), (override));
        MOCK_METHOD(void, handleSmsReceive, (common::PhoneNumber from, std::string msgText), (override));

        // Timer-related event mock
        MOCK_METHOD(void, handleTimeout, (), (override));

        // UI event mocks (custom handlers)
        MOCK_METHOD(void, handleUiAction, (std::optional<std::size_t> ind), (override));
        MOCK_METHOD(void, handleUiBack, (), (override));
    };

} // namespace ue
