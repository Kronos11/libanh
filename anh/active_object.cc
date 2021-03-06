// Copyright (c) 2010 ANH Studios. All rights reserved.
// Use of this source code is governed by a GPL-style license that can be
// found in the COPYING file.

#include "anh/active_object.h"

using boost::thread;

/// The anh namespace hosts a number of useful utility classes intended
/// to be used and reused in domain specific classes.
namespace anh {

ActiveObject::ActiveObject() : done_(false) {
    thread_ = std::move(thread([=] { run(); }));
}

ActiveObject::~ActiveObject() {
    send([&] { done_ = true; });
    thread_.join();
}

void ActiveObject::send(Message message) {
    message_queue_.push(message);
    condition_.notify_one();
}

void ActiveObject::run() {
    // Create a lock and the container we're going to use inside the main
    // loop. Note that the lock here is immediately unlocked as soon as the
    // main loop is entered by the condition variable.
    Message message;
    boost::unique_lock<boost::mutex> lock(mutex_);

    while (! done_) {
        // Use a condition variable to halt processing on this thread until
        // there is something to do, or if the specified timeout is reached,
        // or spuriously activated by the OS.
        if (condition_.timed_wait(
            lock, 
            boost::get_system_time() + boost::posix_time::milliseconds(1),
            [this, &message] { return message_queue_.try_pop(message); })) 
        {
            // If a message was successfully popped then process it.
            message();
        }
    }
}

}  // namespace anh
