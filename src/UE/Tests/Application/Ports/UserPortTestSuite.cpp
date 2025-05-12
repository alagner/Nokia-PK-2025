#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Ports/UserPort.hpp"
#include "Mocks/ILoggerMock.hpp"
#include "Mocks/IUserPortMock.hpp"
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
    NiceMock<common::ILoggerMock> loggerMock;
    StrictMock<IEventsHandlerMock> handlerMock;
    StrictMock<IUeGuiMock> guiMock;
    StrictMock<IListViewModeMock> listViewModeMock;

    UserPort objectUnderTest{loggerMock, guiMock, PHONE_NUMBER};

    UserPortTestSuite()
    {
        EXPECT_CALL(guiMock, setTitle(HasSubstr(common::to_string(PHONE_NUMBER)))); // Already had this
        // Expect the callbacks to be set during start()
        EXPECT_CALL(guiMock, setAcceptCallback(_)); // Use '_' to match any callback function object
        EXPECT_CALL(guiMock, setRejectCallback(_));

        objectUnderTest.start(handlerMock);
    }
    ~UserPortTestSuite()
    {
        EXPECT_CALL(guiMock, setAcceptCallback(IsNull())); // IsNull() matches nullptr
        EXPECT_CALL(guiMock, setRejectCallback(IsNull()));

        objectUnderTest.stop();
    }
};

TEST_F(UserPortTestSuite, shallStartStop)
{
    SUCCEED();
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

TEST_F(UserPortTestSuite, shallShowMenuOnConnected)
{
    EXPECT_CALL(guiMock, setListViewMode()).WillOnce(ReturnRef(listViewModeMock));
    EXPECT_CALL(listViewModeMock, clearSelectionList());
    EXPECT_CALL(listViewModeMock, addSelectionListItem("New Message", _)).Times(1);
    EXPECT_CALL(listViewModeMock, addSelectionListItem("Check Messages", _)).Times(1);

    EXPECT_CALL(guiMock, showConnected());
    objectUnderTest.showConnected();
}

}
