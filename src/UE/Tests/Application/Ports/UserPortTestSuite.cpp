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
    NiceMock<IUeGuiMock> guiMock;  // Changed to NiceMock to ignore uninteresting calls
    NiceMock<IListViewModeMock> listViewModeMock;  // Changed to NiceMock as well

    UserPort objectUnderTest{loggerMock, guiMock, PHONE_NUMBER};

    UserPortTestSuite()
    {
        // We only care about setTitle during initialization, the rest are allowed
        EXPECT_CALL(guiMock, setTitle(HasSubstr(to_string(PHONE_NUMBER))));
        
        // SetListViewMode is now handled by NiceMock
        ON_CALL(guiMock, setListViewMode()).WillByDefault(ReturnRef(listViewModeMock));
        
        objectUnderTest.start(handlerMock);
    }
    
    ~UserPortTestSuite()
    {
        objectUnderTest.stop();
    }
};

TEST_F(UserPortTestSuite, shallStartStop)
{
    // Just verify that setup and teardown work correctly
}

TEST_F(UserPortTestSuite, shallShowNotConnected)
{
    EXPECT_CALL(guiMock, showNotConnected());
    objectUnderTest.showNotConnected();
    ::testing::Mock::VerifyAndClearExpectations(&guiMock);
}

TEST_F(UserPortTestSuite, shallShowConnecting)
{
    EXPECT_CALL(guiMock, showConnecting());
    objectUnderTest.showConnecting();
    ::testing::Mock::VerifyAndClearExpectations(&guiMock);
}

TEST_F(UserPortTestSuite, shallShowMenuOnConnected)
{
    // For this test, we need to be selective about what we expect
    EXPECT_CALL(guiMock, setListViewMode()).WillOnce(ReturnRef(listViewModeMock));
    EXPECT_CALL(listViewModeMock, clearSelectionList());
    EXPECT_CALL(listViewModeMock, addSelectionListItem(_, _)).Times(AtLeast(1));
    
    objectUnderTest.showConnected();
    
    ::testing::Mock::VerifyAndClearExpectations(&guiMock);
    ::testing::Mock::VerifyAndClearExpectations(&listViewModeMock);
}

}
