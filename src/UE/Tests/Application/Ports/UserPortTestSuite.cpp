// UserPortTestSuite.cpp

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Ports/UserPort.hpp"
#include "Mocks/ILoggerMock.hpp"
// IUserPortMock might not be needed here if not testing interaction WITH the port mock itself
// #include "Mocks/IUserPortMock.hpp"
#include "Messages/PhoneNumber.hpp"
#include "Mocks/IUeGuiMock.hpp"
#include "Mocks/IEventsHandlerMock.hpp"

namespace ue
{
    using namespace ::testing;

    class UserPortTestSuite : public Test
    {
    protected:
        const common::PhoneNumber PHONE_NUMBER{112};
        NiceMock<common::ILoggerMock> loggerMock; // NiceMock for logger is usually fine
        StrictMock<IEventsHandlerMock> handlerMock;
        StrictMock<IUeGuiMock> guiMock;                 // Keeping StrictMock for GUI interaction verification
        StrictMock<IListViewModeMock> listViewModeMock; // Mock for the list view mode

        UserPort objectUnderTest{loggerMock, guiMock, PHONE_NUMBER};

        // Constructor: Set expectations for calls happening during objectUnderTest.start()
        UserPortTestSuite()
        {
            EXPECT_CALL(guiMock, setTitle(HasSubstr(common::to_string(PHONE_NUMBER)))); // Already had this
            // Expect the callbacks to be set during start()
            EXPECT_CALL(guiMock, setAcceptCallback(_)); // Use '_' to match any callback function object
            EXPECT_CALL(guiMock, setRejectCallback(_));

            // Call start AFTER setting expectations for it
            objectUnderTest.start(handlerMock);
        }

        // Destructor: Set expectations for calls happening during objectUnderTest.stop()
        ~UserPortTestSuite() override // Use override for virtual destructor
        {
            // Expect the callbacks to be cleared (set to nullptr) during stop()
            EXPECT_CALL(guiMock, setAcceptCallback(IsNull())); // IsNull() matches nullptr
            EXPECT_CALL(guiMock, setRejectCallback(IsNull()));

            // Call stop() within the destructor scope AFTER setting expectations
            objectUnderTest.stop();
            // Note: objectUnderTest is destroyed after this destructor finishes
        }
    };

    // Test is primarily checking constructor/destructor expectations
    TEST_F(UserPortTestSuite, shallStartStop)
    {
        // Expectations are handled by the fixture's constructor and destructor
        SUCCEED(); // If fixture setup/teardown doesn't fail, this passes
    }

    TEST_F(UserPortTestSuite, shallShowNotConnected)
    {
        EXPECT_CALL(guiMock, showNotConnected());
        objectUnderTest.showNotConnected();
    }

    TEST_F(UserPortTestSuite, shallShowConnecting)
    {
        EXPECT_CALL(guiMock, showConnecting());
        objectUnderTest.showConnecting();
    }

    TEST_F(UserPortTestSuite, shallShowDialMode)
    {
        EXPECT_CALL(guiMock, setDialMode());
        objectUnderTest.showDialMode();
    }

    TEST_F(UserPortTestSuite, shallShowMenuOnConnected)
    {
        // Expectations for setting up the list view
        EXPECT_CALL(guiMock, setListViewMode()).WillOnce(ReturnRef(listViewModeMock));
        EXPECT_CALL(listViewModeMock, clearSelectionList());
        EXPECT_CALL(listViewModeMock, addSelectionListItem("Compose SMS", _)); // Expect specific items added
        EXPECT_CALL(listViewModeMock, addSelectionListItem("View SMS", _));
        EXPECT_CALL(listViewModeMock, addSelectionListItem("Dial", _));
        // Expect the GUI state update call made within UserPort::showConnected
        EXPECT_CALL(guiMock, showConnected());

        // ACTION: Call the method under test
        objectUnderTest.showConnected();
    }

} // namespace ue