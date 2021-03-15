#include <mtf/coroutine/impl.hpp>

namespace mtf::coroutine::impl {

Coroutine* Coroutine::current_coroutine_{nullptr};

Coroutine::Coroutine(Routine routine, context::StackView stack)
    : routine_(std::move(routine)),
      is_completed_(false),
      prev_coroutine_(nullptr) {
  coroutine_context_.Setup(stack, Coroutine::Trampoline);
}

void Coroutine::Resume() {
  prev_coroutine_ = current_coroutine_;
  current_coroutine_ = this;

  caller_context_.SwitchTo(coroutine_context_);
}

void Coroutine::Suspend() {
  Coroutine* suspended = current_coroutine_;
  current_coroutine_ = current_coroutine_->prev_coroutine_;
  suspended->coroutine_context_.SwitchTo(suspended->caller_context_);
}

bool Coroutine::IsCompleted() const {
  return is_completed_;
}

void Coroutine::Trampoline() {
  current_coroutine_->coroutine_context_.AfterStart();
  current_coroutine_->routine_();
  current_coroutine_->is_completed_ = true;

  Coroutine* ended_coroutine = current_coroutine_;
  current_coroutine_ = current_coroutine_->prev_coroutine_;

  ended_coroutine->coroutine_context_.SwitchTo(
      ended_coroutine->caller_context_);
  std::abort();
}

}  // namespace mtf::coroutine::impl