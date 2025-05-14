#pragma once

#include "BaseState.hpp"
#include <optional>
#include "Context.hpp"
#include "SmsStorage.hpp"

namespace ue {

class ComposeSmsState : public BaseState {
public:
    explicit ComposeSmsState(Context& context);

    void handleUiAction(std::optional<std::size_t> selectedIndex) override;
    void handleUiBack() override;
    void handleDisconnected() override;
    void handleMessageReceive(common::PhoneNumber from, std::string text) override;
    void handleMessageSentResult(common::PhoneNumber to, bool success) override;

private:
    void validateAndSendSms();
    bool inputsAreValid(const common::PhoneNumber& recipient, const std::string& body) const;
    void onIncomingSms(common::PhoneNumber from, const std::string& body);
};

}
