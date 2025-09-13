//
// Created by kirill on 12/28/24.
//

#ifndef SERVER_COROUTINE_H
#define SERVER_COROUTINE_H

#include <tasks/Awaitable.h>
#include <system/SystemContext.h>

template<typename T>
class AFTest : public usub::uvent::detail::AwaitableFrameBase<std::suspend_never> {
public:
    AFTest() noexcept = default;

    ~AFTest() = default;

    void unhandled_exception() {
        this->exception_ = std::current_exception();
    }

    usub::uvent::task::Awaitable<T, AFTest<T>> get_return_object() {
        this->coro_ = std::coroutine_handle<AFTest<T>>::from_promise(*this);
        return usub::uvent::task::Awaitable<T, AFTest<T>>{this};
    }

    void return_void() {
        this->has_result_ = true;
    }

    T get() {
        if (this->exception_) std::rethrow_exception(this->exception_);
        return std::move(*std::launder(reinterpret_cast<T *>(&this->result_)));
    }

    void await_suspend(std::coroutine_handle<> h) override {
        this->prev_ = static_cast<std::coroutine_handle<void>>(h);
    }

private:
    bool has_result_ = false;
};


#endif//SERVER_COROUTINE_H
