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
    NiceMock<common::ILoggerMock> loggerMock;
    StrictMock<IEventsHandlerMock> handlerMock;
    StrictMock<IUeGuiMock> guiMock;
    StrictMock<IListViewModeMock> listViewModeMock;

    UserPort objectUnderTest{loggerMock, guiMock, PHONE_NUMBER};

    UserPortTestSuite()
    {
        EXPECT_CALL(guiMock, setTitle(HasSubstr(common::to_string(PHONE_NUMBER))));
        EXPECT_CALL(guiMock, setAcceptCallback(_));
        EXPECT_CALL(guiMock, setRejectCallback(_));

        objectUnderTest.start(handlerMock);
    }

    ~UserPortTestSuite() override
    {
        EXPECT_CALL(guiMock, setAcceptCallback(IsNull()));
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
    EXPECT_CALL(listViewModeMock, addSelectionListItem("Compose SMS", _));
    EXPECT_CALL(listViewModeMock, addSelectionListItem("View SMS", _));

    EXPECT_CALL(guiMock, showConnected()); 

    objectUnderTest.showConnected();
}

}