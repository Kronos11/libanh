// Copyright (c) 2010 ANH Studios. All rights reserved.
// Use of this source code is governed by a GPL-style license that can be
// found in the COPYING file.

#ifndef UTILITIES_ACTIVE_OBJECT_H_
#define UTILITIES_ACTIVE_OBJECT_H_

#include <functional>

#include <boost/thread.hpp>
#include <tbb/concurrent_queue.h>

/// The utilities namespace hosts a number of useful utility classes intended
/// to be used and reused in domain specific classes.
namespace utilities {

/**
 * There are many times when it makes sense to break an object off and run it
 * concurrently while the rest of the application runs. The ActiveObject is a
 * reusable facility that encourages the encapsulation of data by using asynchronus
 * messages to process requests in a private thread. This implementation is based
 * on a design discussed by Herb Sutter.
 *
 * Note that using an ActiveObject can result in up to 100% of the otherwise unused cpu
 * being consumed. This is intentional as ActiveObjects should primarily be targeted at
 * uses where being highly response is a priority.
 *
 * @see http://www.drdobbs.com/go-parallel/article/showArticle.jhtml?articleID=225700095
 */
class ActiveObject {
public:
    /// Messages are implemented as std::function to allow maximum flexibility for
    /// how a message can be created with support for functions, functors, class members,
    /// and most importantly lambdas.
    typedef std::function<void()> Message;

public:
    /// Default constructor kicks off the private thread that listens for incoming messages.
    ActiveObject();

    /// Default destructor sends an end message and waits for the private thread to complete.
    ~ActiveObject();

    /**
     * Sends a message to be handled by the ActiveObject's private thread.
     *
     * \param message The message to process on the private thread.
     */
    void Send(Message message);

private:
    /// Runs the ActiveObject's message loop until an end message is received.
    void Run();

    tbb::concurrent_queue<Message> message_queue_;
    boost::thread thread_;
    boost::condition_variable condition_;
    boost::mutex mutex_;

    bool done_;
};

}  // namespace utilities

#endif  // UTILITIES_ACTIVE_OBJECT_H_
