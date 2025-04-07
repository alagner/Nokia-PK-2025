#include "Context.hpp"

// Include full definitions needed ONLY for unique_ptr DESTRUCTION
#include "States/BaseState.hpp"
// No other state headers needed here anymore

namespace ue {

// Define constructor
Context::Context(common::ILogger& logger, IBtsPort& bts, IUserPort& user, ITimerPort& timer, Application& app)
    : logger(logger),
      bts(bts),
      user(user),
      timer(timer),
      app(app),
      state(nullptr)
{}

// Define destructor (Requires full BaseState definition included above)
Context::~Context() = default;

// REMOVED: setState definition (moved back to header)
// REMOVED: Explicit template instantiations


} // namespace ue