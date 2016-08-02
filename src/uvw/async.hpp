#pragma once


#include <utility>
#include <memory>
#include <uv.h>
#include "event.hpp"
#include "handle.hpp"
#include "util.hpp"


namespace uvw {


/**
 * @brief Trigger event.
 *
 * It will be emitted by the AsyncHandle according with its functionalities.
 */
struct AsyncEvent: Event<AsyncEvent> { };


/**
 * @brief The AsyncHandle handle.
 *
 * Async handles allow the user to _wakeup_ the event loop and get an event
 * emitted from another thread.
 */
class AsyncHandle final: public Handle<AsyncHandle, uv_async_t> {
    static void sendCallback(uv_async_t *handle) {
        AsyncHandle &async = *(static_cast<AsyncHandle*>(handle->data));
        async.publish(AsyncEvent{});
    }

    using Handle::Handle;

public:
    /**
     * @brief Creates a new async handle.
     * @param args A pointer to the loop from which the handle generated.
     * @return A pointer to the newly created handle.
     */
    template<typename... Args>
    static std::shared_ptr<AsyncHandle> create(Args&&... args) {
        return std::shared_ptr<AsyncHandle>{new AsyncHandle{std::forward<Args>(args)...}};
    }

    /**
     * @brief Initializes the handle.
     *
     * Unlike other handle initialization functions, it immediately starts the
     * handle.
     *
     * @return True in case of success, false otherwise.
     */
    bool init() {
        return initialize<uv_async_t>(&uv_async_init, &sendCallback);
    }

    /**
     * @brief Wakeups the event loop and emits the AsyncEvent event.
     *
     * It’s safe to call this function from any thread.<br/>
     * An AsyncEvent event will be emitted on the loop thread.
     *
     * See the official
     * [documentation](http://docs.libuv.org/en/v1.x/async.html#c.uv_async_send)
     * for further details.
     */
    void send() {
        invoke(&uv_async_send, get<uv_async_t>());
    }
};


}
