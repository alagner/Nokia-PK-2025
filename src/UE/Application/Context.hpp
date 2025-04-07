#pragma once

#include <memory>   // For unique_ptr declaration
#include <utility>  // For std::forward

// Forward declarations
namespace common { class ILogger; }
namespace ue {
    class IBtsPort;
    class IUserPort;
    class ITimerPort;
    class Application;
    class BaseState; // Forward declare BaseState
}

namespace ue {

struct Context
{
    common::ILogger& logger;
    IBtsPort& bts;
    IUserPort& user;
    ITimerPort& timer;
    Application& app;

    std::unique_ptr<BaseState> state;

    // Constructor declared (defined in Context.cpp)
    Context(common::ILogger& logger, IBtsPort& bts, IUserPort& user, ITimerPort& timer, Application& app);

    // Destructor declared (defined in Context.cpp)
    ~Context();

    // Rely on implicit compiler-generated rules for copy/move.
    Context(Context&&) noexcept = default; // Keep move constructor default
    Context& operator=(Context&&) noexcept = delete; // Keep assignment deleted
    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;

    // setState TEMPLATE DEFINITION moved back here
    template<typename State, typename... Args>
    void setState(Args&&... args)
    {
        // Requires full definition of State type (which includes BaseState)
        // BaseState itself only needs Context forward declaration at this point
        state = std::make_unique<State>(*this, std::forward<Args>(args)...);
    }
};

} // namespace ue