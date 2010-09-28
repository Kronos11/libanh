// Copyright (c) 2010 ANH Studios. All rights reserved.
// Use of this source code is governed by a GPL-style license that can be
// found in the COPYING file.

#ifndef ANH_EVENT_DISPATCHER_H_
#define ANH_EVENT_DISPATCHER_H_

#include <cstdint>
#include <functional>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <vector>

#include <boost/thread.hpp>

#include "anh/active_object.h"
#include "anh/event.h"

/// The anh namespace hosts a number of useful utility classes intended
/// to be used and reused in domain specific classes.
namespace anh {

typedef std::function<bool (IEventPtr)> EventListenerCallback;

// Use a HashString as the basis for EventListenerType's.
typedef HashString EventListenerType;

// Most of the time spent processing events will be iterating over a collection
// of callbacks to notify about an event so we want to make use of std::list, however,
// very infrequently we also want to be able to add and remove listeners and need a
// way to identify them in the list so a std::pair is used as the node.
typedef std::pair<EventListenerType, EventListenerCallback> EventListener;
typedef std::list<EventListener> EventListenerList;
typedef std::map<EventType, EventListenerList> EventListenerMap;
typedef std::set<EventType> EventTypeSet;
typedef std::priority_queue<IEventPtr, std::vector<IEventPtr>, CompareEventWeightLessThanPredicate> EventQueue;

/*! \brief The event dispatcher is a facility for triggering events and passing messages
 * between different "modules" of code that may or may not be running on separate
 * processes or even separate physical machines.
 */
class EventDispatcher {
public:
    EventDispatcher();
    explicit EventDispatcher(uint64_t current_time);
    ~EventDispatcher();

    /**
     * Connects an event listener to an event.
     *
     * \param event_type The event type to check for connected listeners.
     * \param listener The listener interested in the event specified.
     */
    void connect(const EventType& event_type, EventListener listener);

    /**
     * Disconnects an event listener from an event.
     *
     * \param event_type The event type to disconnect from.
     * \param event_listener_type The type of the event listener being disconnected.
     */
    void disconnect(const EventType& event_type, const EventListenerType& event_listener_type);

    /**
     * Disconnects an event listener from all events.
     *
     * \param event_listener_type The type of the event listener being disconnected.
     */
    void disconnectFromAll(const EventListenerType& event_listener_type);

    /**
     * Gets all of the listeners connected to a specific event type.
     *
     * \param event_type The event type to check for connected listeners.
     * \return A list of the connected listeners to the specified event.
     */
    boost::unique_future<std::vector<EventListener>> getListeners(const EventType& event_type);

    /**
     * Gets a list of all of the registered events.
     *
     * \returns A list of all the registered events.
     */
    boost::unique_future<std::vector<EventType>> getRegisteredEvents();

    /**
     * Notifies all interested listeners asynchronously that an event has occurred.
     *
     * \param triggered_event The triggered event to be delivered.
     */
    void notify(IEventPtr triggered_event);

    /**
     * Delivers an event immediately to all interested listeners.
     *
     * \param triggered_event The triggered event to be delivered.
     */
    boost::unique_future<bool> deliver(IEventPtr triggered_event);

    /**
     * A check to see if there are any events waiting to be processed.
     *
     * \returns Returns true if their are events waiting, false if not.
     */
    boost::unique_future<bool> hasEvents();

    /**
     * Processes all queued events.
     */
    boost::unique_future<bool> tick(uint64_t new_timestep);

    /**
     * Returns the current timestep as provided by the most recent call to Tick.
     *
     * \returns The current timestep.
     */
    boost::unique_future<uint64_t> current_timestep();

private:
    /// Disable the default copy constructor.
    EventDispatcher(const EventDispatcher&);

    /// Disable the default assignment operator.
    EventDispatcher& operator=(const EventDispatcher&);

    bool validateEventType_(const EventType& event_type) const;
    bool validateEventListenerType_(const EventListenerType& event_listener_type) const;
    bool addEventType_(const EventType& event_type);
    void disconnect_(const EventType& event_type, const EventListenerType& event_listener_type);
    bool deliver_(IEventPtr triggered_event);

    EventTypeSet event_type_set_;

    EventListenerMap event_listener_map_;

    uint64_t current_timestep_;
    
    // Uses a double buffered queue to prevent events that generate events from creating
    // an infinite loop.

    enum ClassConstants
    {
        kNumQueues = 2
    };

    EventQueue event_queue_[kNumQueues];
    int active_queue_;
    
    ActiveObject active_;
};

}  // namespace anh

#endif  // ANH_EVENT_DISPATCHER_H_
