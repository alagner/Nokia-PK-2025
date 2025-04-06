#include "ConsoleCommands.hpp"
#include "TestCommands/TestCommands.hpp"
#include "Messages/OutgoingMessage.hpp"
#include "Messages/MessageId.hpp"
#include "Messages/PhoneNumber.hpp"
#include "Logger/ILogger.hpp"
#include <sstream>
#include <stdexcept>

namespace bts
{

ConsoleCommands::ConsoleCommands(IConsole& console,
                                 IApplicationEnvironment &environment,
                                 common::ILogger& logger,
                                 std::shared_ptr<IUeRelay> ueRelay,
                                 SyncGuardPtr syncGuard)
    : syncGuard(syncGuard),
      logger(logger, "[CONSOLE]"),
      console(console),
      environment(environment),
      ueRelay(ueRelay)
{}

ConsoleCommands::~ConsoleCommands()
{}

void ConsoleCommands::start()
{
    SyncLock lock(*syncGuard);
    auto argsArgument = std::placeholders::_1;
    auto streamArgument = std::placeholders::_2;
    console.addCommand("a", "Show address", std::bind(&ConsoleCommands::showAddress, this, argsArgument, streamArgument));
    console.addCommand("s", "Show status", std::bind(&ConsoleCommands::showStatus, this, argsArgument, streamArgument));
    console.addCommand("l", "List attached ue", std::bind(&ConsoleCommands::listAttachedUe, this, argsArgument, streamArgument));
    console.addCloseCommand();
    console.addCommand("call", "Initiate call: call <target_phone> <source_phone>", std::bind(&ConsoleCommands::handleCall, this, argsArgument, streamArgument));
    console.addHelpCommand();
    console.addCommand("t", "Test commands - details in implementation",std::bind(&ConsoleCommands::testCommands, this, argsArgument, streamArgument));
}

void ConsoleCommands::stop()
{
    // do nothing
}

void ConsoleCommands::showAddress(std::string, std::ostream &os)
{
    SyncLock lock(*syncGuard);
    os << "BTS ID: " << environment.getBtsId() << "\n";
    os << "Address: " << environment.getAddress() << "\n";
}

void ConsoleCommands::showStatus(std::string, std::ostream& os)
{
    SyncLock lock(*syncGuard);
    os << "Connections: \n";
    os << " > ue attached: " << ueRelay->countAttached() << "\n";
    os << " > ue not attached: " << ueRelay->countNotAttached() << "\n";
}

void ConsoleCommands::listAttachedUe(std::string, std::ostream& os)
{
    SyncLock lock(*syncGuard);

    os << "attached ue: \n";
    ueRelay->visitAttachedUe([&os, i = 0](IUeConnection const& ue) mutable
    {
        os << "\t#" << ++i << ": " << ue << "\n";
    });
}

void ConsoleCommands::handleCall(std::string args, std::ostream &os)
{
    SyncLock lock(*syncGuard);
    std::istringstream ss(args);
    common::PhoneNumber targetPhoneNumberValue;
    common::PhoneNumber sourcePhoneNumberValue;

    if (!(ss >> targetPhoneNumberValue >> sourcePhoneNumberValue))
    {

        os << "Error: Invalid arguments.\nUsage: call <target_phone> <source_phone>\n";
        logger.logError("Invalid 'call' command arguments: ", args);
        return;
    }

    std::string remaining;
    if (ss >> remaining)
    {
         os << "Error: Too many arguments.\nUsage: call <target_phone> <source_phone>\n";
         logger.logError("Too many 'call' command arguments: ", args);
         return;
    }


    logger.logInfo("Attempting to initiate call from ", sourcePhoneNumberValue, " to ", targetPhoneNumberValue);

    common::OutgoingMessage callMsg(common::MessageId::CallRequest, sourcePhoneNumberValue, targetPhoneNumberValue);

    try
    {
        ueRelay->sendMessage(callMsg.getMessage(), targetPhoneNumberValue);
        os << "CallRequest sent from " << sourcePhoneNumberValue << " to " << targetPhoneNumberValue << "\n";
        logger.logInfo("CallRequest sent to UE: ", targetPhoneNumberValue);
    }
    catch (const std::exception& e)
    {
        os << "Error sending CallRequest: " << e.what() << "\n";
        logger.logError("Failed to send CallRequest to UE ", targetPhoneNumberValue, ": ", e.what());
    }
    catch (...)
    {
        os << "Unknown error sending CallRequest to " << targetPhoneNumberValue << "\n";
        logger.logError("Unknown error sending CallRequest to UE ", targetPhoneNumberValue);
    }
}

void ConsoleCommands::testCommands(std::string args, std::ostream &os)
{
    using common::TestCommands;
    try
    {
        TestCommands testParser(args);
        TestCommands::Parameters parameters{};
        parameters.sendMessage = [this] (BinaryMessage message,
                                         PhoneNumber to)
        {
            SyncLock lock(*syncGuard);
            ueRelay->sendMessage(message, to);
        };
        parameters.printText = [this, &os] (std::string message)
        {
            SyncLock lock(*syncGuard);
            os << message;
        };

        SyncLock lock(*syncGuard);
        testParser.run(parameters);
    }
    catch (std::exception& ex)
    {
        SyncLock lock(*syncGuard);
        os << " test commands syntax error: " << ex.what();
    }
}

}
