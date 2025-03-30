#include "ApplicationEnvironmentFactory.hpp"
#include "ApplicationEnvironment.hpp"

namespace bts {

std::unique_ptr<IApplicationEnvironment>
createApplicationEnvironment(int &argc, char *argv[]) {
  return std::make_unique<ApplicationEnvironment>(argc, argv);
}

} // namespace bts
