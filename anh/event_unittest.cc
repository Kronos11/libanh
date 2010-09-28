// Copyright (c) 2010 ANH Studios. All rights reserved.
// Use of this source code is governed by a GPL-style license that can be
// found in the COPYING file.

#include "anh/event.h"

#include <gtest/gtest.h>

using anh::BaseEvent;
using anh::ByteBuffer;
using anh::IEventPtr;
using anh::EventCallback;
using anh::EventType;
using anh::EventPriority;
using anh::EventSubject;

// Wrapping tests in an anonymous namespace prevents potential name conflicts.
namespace {

class MockEvent : public BaseEvent {
public:
    explicit MockEvent(uint64_t subject_id = 0, uint64_t delay_ms = 0) 
        : BaseEvent(subject_id, delay_ms)
    , some_event_val_(0) {}

    explicit MockEvent(uint64_t subject_id, uint64_t delay_ms, EventCallback callback) 
        : BaseEvent(subject_id, delay_ms, callback)
    , some_event_val_(0) {}

    explicit MockEvent(ByteBuffer& in) {
        deserialize(in);
    }

    ~MockEvent() {}

    const EventType& event_type() const { return event_type_; }

    int some_event_val() const { return some_event_val_; }
    void some_event_val(int some_event_val) { some_event_val_ = some_event_val; }

private:
    void onSerialize(ByteBuffer& out) const {
        out << some_event_val_;
    }

    void onDeserialize(ByteBuffer& in) {
        some_event_val_ = in.read<int>();
    }

    bool onConsume(bool handled) const {
        return true;
    }

    static const EventType event_type_;
    int some_event_val_;
};
    
const EventType MockEvent::event_type_ = EventType("mock_event");
    
/*! All events should have a type and a way of returning that type to a caller.
 */
TEST(EventTests, EventHasAnEventType) {
    MockEvent test_event;
    EXPECT_EQ(EventType("mock_event"), test_event.event_type());
}

/*! Code that consumes an event needs a method of determining if a subject has
 * been set yet or not.
 */
TEST(EventTests, EventHasNoSubjectByDefault) {
    MockEvent test_event;
    EXPECT_EQ(EventSubject(0), test_event.subject());
}

/*! An event's subject should be specified at the point of event creation.
 */
TEST(EventTests, CanSetAndRetrieveSubjectForEvent) {
    // Create some subject id number to use for testing and create the MockEvent instance with it.
    EventSubject some_subject_id = 2234;
    MockEvent test_event(some_subject_id);

    // Make sure that the event now has a subject and that it returns the correct value.
    EXPECT_EQ(some_subject_id, test_event.subject());
}

/*! Every event needs to pass through the event dispatcher in order to be 
 * consumed by a listener. The event dispatcher recognizes that not all events
 * are created equal and therefore uses an event's priority as a weight when
 * determining where to place it in the processing queue. By default event's 
 * should have no additional weight.
 */
TEST(EventTests, EventsHaveDefaultPriorityOfZero) {
    MockEvent test_event;

    // Make sure that by default the priority is zero.
    EXPECT_EQ(EventPriority(0), test_event.priority());
}

/*! Setting a priority for an event will most likely not be the responsibility
 * of the source that's generating it, therefore some facility must exist for
 * setting a priority for an event.
 */
TEST(EventTests, CanSetPriorityForEvent) {
    MockEvent test_event;

    // Set a priority value and make sure what we get out is what was put in.
    test_event.priority(27);
    EXPECT_EQ(EventPriority(27), test_event.priority());
}

TEST(EventTests, CanSetTimestampForEvent) {
    uint64_t timestamp = 100;

    // Set a timestamp for the event and make sure the value we get out is what was put in.
    MockEvent test_event;
    test_event.timestamp(timestamp);

    EXPECT_EQ(timestamp, test_event.timestamp());
}

TEST(EventTests, ComparingEventsConsidersTimestamp) {
    // Create two events with differing timestamps.
    EventSubject some_subject_id = 2234;

    MockEvent test_event1(some_subject_id);
    test_event1.timestamp(100);

    MockEvent test_event2(some_subject_id);
    test_event2.timestamp(200);

    EXPECT_TRUE(CompareEventWeightLessThan(test_event1, test_event2));
    EXPECT_TRUE(CompareEventWeightGreaterThan(test_event2, test_event1));
}

TEST(EventTests, ComparingEventsConsidersDelayWithTimestamp) {
    // Create two events with differing timestamps.
    EventSubject some_subject_id = 2234;

    // Create an event with a low timestamp but a long delay
    MockEvent test_event1(some_subject_id, 400);
    test_event1.timestamp(100);

    // Create an event with a future timestamp but a short delay
    MockEvent test_event2(some_subject_id, 100);
    test_event2.timestamp(200);

    EXPECT_TRUE(CompareEventWeightLessThan(test_event2, test_event1));
    EXPECT_TRUE(CompareEventWeightGreaterThan(test_event1, test_event2));
}

TEST(EventTests, ComparingEventsConsidersPriority) {
    // Create two events with differing priorities.
    MockEvent test_event1;
    test_event1.priority(1);

    MockEvent test_event2;
    test_event2.priority(2);

    EXPECT_TRUE(CompareEventWeightLessThan(test_event1, test_event2));
    EXPECT_TRUE(CompareEventWeightGreaterThan(test_event2, test_event1));
}


TEST(EventTests, CanSetCallbackForEvent) {
    std::shared_ptr<int> someval = std::make_shared<int>(0);
    
    // Create an event with a callback that updates our local value.
    MockEvent test_event1(0, 0, [=] {
        *someval = 1;
    });

    test_event1.consume(true);
    
    // Make sure the value was updated.
    EXPECT_EQ(1, *someval);
}

TEST(EventTests, CanDeserializeEventFromBuffer) {
    // Create the buffer with the correct contents.
    ByteBuffer buffer;
    buffer.write<uint32_t>(0xC3CEA198); // This is the swgcrc of "mock_event"
    buffer.write<int>(27); // Some random int value.

    MockEvent test_event(buffer);

    EXPECT_EQ(27, test_event.some_event_val());
}

TEST(EventTests, CanSerializeEventToBuffer) {
    // Create the buffer with the correct contents.
    ByteBuffer buffer;

    MockEvent test_event;
    test_event.some_event_val(27);

    // Read the event into the buffer.
    test_event.serialize(buffer);

    EXPECT_EQ(0xC3CEA198, buffer.read<uint32_t>());
    EXPECT_EQ(27, buffer.read<int>());
}

}  // namespace
