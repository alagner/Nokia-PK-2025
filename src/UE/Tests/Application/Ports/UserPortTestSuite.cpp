#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Ports/UserPort.hpp"
#include "Mocks/ILoggerMock.hpp"
#include "Mocks/IUserPortMock.hpp"
#include "Messages/PhoneNumber.hpp"
#include "Mocks/IUeGuiMock.hpp"

namespace ue
{
using namespace ::testing;

class UserPortTestSuite : public Test
{
protected:
    const common::PhoneNumber PHONE_NUMBER{112};
    NiceMock<common::ILoggerMock> loggerMock;
    StrictMock<IUserEventsHandlerMock> handlerMock;
    StrictMock<IUeGuiMock> guiMock;
    StrictMock<IListViewModeMock> listViewModeMock;
    StrictMock<ITextModeMock> textModeMock;

    UserPort objectUnderTest{loggerMock, guiMock, PHONE_NUMBER};

    UserPortTestSuite()
    {
        EXPECT_CALL(guiMock, setTitle(HasSubstr(to_string(PHONE_NUMBER))));
        objectUnderTest.start(handlerMock);
    }
    ~UserPortTestSuite()
    {
        objectUnderTest.stop();
    }
};

TEST_F(UserPortTestSuite, shallStartStop)
{
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
    EXPECT_CALL(listViewModeMock, addSelectionListItem(_, _)).Times(AtLeast(1));
    EXPECT_CALL(guiMock, setAcceptCallback(_)).Times(Exactly(1));
    EXPECT_CALL(guiMock, setRejectCallback(_)).Times(Exactly(1));

    objectUnderTest.showConnected();
}

TEST_F(UserPortTestSuite, shallShowSmsList)
{
    std::vector<SmsEntity> smsVector;
    smsVector.push_back({12,123,"test1"});
    smsVector.push_back({12,123,"test2"});
    smsVector.push_back({123,12,"test3"});

    EXPECT_CALL(guiMock, setListViewMode()).WillOnce(ReturnRef(listViewModeMock));
    EXPECT_CALL(listViewModeMock, clearSelectionList());
    EXPECT_CALL(listViewModeMock, addSelectionListItem(_, _)).Times(Exactly(3));
    EXPECT_CALL(guiMock, showNewSms(false)).Times(Exactly(1));
    EXPECT_CALL(guiMock, setAcceptCallback(_)).Times(Exactly(1));
    EXPECT_CALL(guiMock, setRejectCallback(_)).Times(Exactly(1));

    objectUnderTest.showSmsList(smsVector);
}

TEST_F(UserPortTestSuite, shallShowSms)
{
    SmsEntity testSms{12,123,"test"};

    EXPECT_CALL(guiMock, setViewTextMode()).WillOnce(ReturnRef(textModeMock));
    EXPECT_CALL(textModeMock, setText("test"));
    EXPECT_CALL(guiMock, setAcceptCallback(_)).Times(Exactly(1));
    EXPECT_CALL(guiMock, setRejectCallback(_)).Times(Exactly(1));

    objectUnderTest.showSms(testSms);
}

}
