#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Ports/UserPort.hpp"
#include "Mocks/ILoggerMock.hpp"
#include "Mocks/IUserPortMock.hpp"
#include "Messages/PhoneNumber.hpp"
#include "Mocks/IUeGuiMock.hpp"
#include "SmsDb.hpp"

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
    StrictMock<ISmsComposeModeMock> smsComposeModeMock;

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
    EXPECT_CALL(guiMock, setAcceptCallback(_));
    objectUnderTest.showConnected();
}

TEST_F(UserPortTestSuite, shallShowSmsListView)
{
    const common::PhoneNumber sender1{123};
    const common::PhoneNumber sender2{200};
    std::vector<Sms> testSmsList = {
        Sms{sender1, "Hello1", true},
        Sms{sender2, "Hello2", false}
    };
    
    objectUnderTest.setSmsList(testSmsList);
    
    EXPECT_CALL(guiMock, setListViewMode()).WillOnce(ReturnRef(listViewModeMock));
    EXPECT_CALL(listViewModeMock, clearSelectionList());
    
    EXPECT_CALL(listViewModeMock, addSelectionListItem("From: 123", "Read"));
    EXPECT_CALL(listViewModeMock, addSelectionListItem("From: 200", "Unread"));
    EXPECT_CALL(listViewModeMock, addSelectionListItem("Back", "Return to main menu"));
    
    EXPECT_CALL(guiMock, setAcceptCallback(_));
    EXPECT_CALL(guiMock, setRejectCallback(_));
    
    objectUnderTest.showSmsList();
}

TEST_F(UserPortTestSuite, shallShowSmsContent)
{
    const std::string fromNumber = "123";
    const std::string smsText = "Hello World!";
    
    EXPECT_CALL(guiMock, setViewTextMode()).WillOnce(ReturnRef(textModeMock));
    
    std::string expectedContent = "From: " + fromNumber + "\n\n" + smsText;
    EXPECT_CALL(textModeMock, setText(expectedContent));
    
    EXPECT_CALL(guiMock, setRejectCallback(_));
    
    objectUnderTest.showSmsContent(fromNumber, smsText);
}

TEST_F(UserPortTestSuite, shallShowSmsComposeView)
{
    EXPECT_CALL(guiMock, setSmsComposeMode()).WillOnce(ReturnRef(smsComposeModeMock));
    
    EXPECT_CALL(smsComposeModeMock, clearSmsText());
    
    EXPECT_CALL(guiMock, setAcceptCallback(_));
    EXPECT_CALL(guiMock, setRejectCallback(_));
    
    objectUnderTest.showSmsComposeView();
}

TEST_F(UserPortTestSuite, shallHandleSmsSelection)
{
    const common::PhoneNumber sender{123};
    std::vector<Sms> testSmsList = {
        Sms{sender, "Test message", false}
    };

    objectUnderTest.setSmsList(testSmsList);
    
    bool callbackCalled = false;
    size_t selectedIndex = 999; // Invalid initial value
    
    objectUnderTest.setSelectSmsCallback([&callbackCalled, &selectedIndex](size_t index) {
        callbackCalled = true;
        selectedIndex = index;
    });
    
    EXPECT_CALL(guiMock, setListViewMode()).WillOnce(ReturnRef(listViewModeMock));
    EXPECT_CALL(listViewModeMock, clearSelectionList());
    EXPECT_CALL(listViewModeMock, addSelectionListItem("From: 123", "Unread"));
    EXPECT_CALL(listViewModeMock, addSelectionListItem("Back", "Return to main menu"));
    
    std::function<void()> capturedCallback;
    EXPECT_CALL(guiMock, setAcceptCallback(_))
        .WillOnce(SaveArg<0>(&capturedCallback));
    EXPECT_CALL(guiMock, setRejectCallback(_));
    
    objectUnderTest.showSmsList();
    
    EXPECT_CALL(guiMock, setListViewMode()).WillOnce(ReturnRef(listViewModeMock));
    EXPECT_CALL(listViewModeMock, getCurrentItemIndex())
        .WillOnce(Return(std::make_pair(true, 0)));
    
    capturedCallback();
    
    EXPECT_TRUE(callbackCalled);
    EXPECT_EQ(selectedIndex, 0);
}

TEST_F(UserPortTestSuite, shallShowUnreadSmsIndicator)
{
    EXPECT_CALL(guiMock, showNewSms(true));
    objectUnderTest.showNewSms(true);
    
    EXPECT_CALL(guiMock, showNewSms(false));
    objectUnderTest.showNewSms(false);
}

TEST_F(UserPortTestSuite, shallShowEmptySmsListCorrectly)
{
    std::vector<Sms> emptySmsList;
    objectUnderTest.setSmsList(emptySmsList);
    
    EXPECT_CALL(guiMock, setListViewMode()).WillOnce(ReturnRef(listViewModeMock));
    EXPECT_CALL(listViewModeMock, clearSelectionList());

    EXPECT_CALL(listViewModeMock, addSelectionListItem("No messages", ""));
    EXPECT_CALL(listViewModeMock, addSelectionListItem("Back", "Return to main menu"));

    EXPECT_CALL(guiMock, setAcceptCallback(_));
    EXPECT_CALL(guiMock, setRejectCallback(_));
    
    objectUnderTest.showSmsList();
}

TEST_F(UserPortTestSuite, shallShowSentSmsContent)
{
    const std::string toNumber = "123";
    const std::string smsText = "Sent message";
    
    EXPECT_CALL(guiMock, setViewTextMode()).WillOnce(ReturnRef(textModeMock));
    
    std::string expectedContent = "To: " + toNumber + "\n\n" + smsText;
    EXPECT_CALL(textModeMock, setText(expectedContent));
    
    EXPECT_CALL(guiMock, setRejectCallback(_));
    
    objectUnderTest.showSentSmsContent(toNumber, smsText);
}

TEST_F(UserPortTestSuite, shallHandleSentSmsInList)
{
    const common::PhoneNumber ownNumber{111};
    const common::PhoneNumber recipient{250};
    
    std::vector<Sms> smsList = {
        Sms{ownNumber, recipient, "Sent message"}
    };
    
    objectUnderTest.setSmsList(smsList);
    
    EXPECT_CALL(guiMock, setListViewMode()).Times(1).WillOnce(ReturnRef(listViewModeMock));
    EXPECT_CALL(listViewModeMock, clearSelectionList()).Times(1);
    EXPECT_CALL(listViewModeMock, addSelectionListItem("To: 250", "Sent")).Times(1);
    EXPECT_CALL(listViewModeMock, addSelectionListItem("Back", "Return to main menu")).Times(1);
    EXPECT_CALL(guiMock, setAcceptCallback(_));
    EXPECT_CALL(guiMock, setRejectCallback(_));
    
    objectUnderTest.showSmsList();
}

}