#include <cstdint>

#include <cpp-state-machine-framework/StateMachine.h>
#include <cpp-state-machine-framework/actions/Nothing.h>
#include <cpp-state-machine-framework/actions/TransitionTo.h>
#include <cpp-state-machine-framework/actions/Maybe.h>
#include <cpp-state-machine-framework/actions/ByDefault.h>
#include <cpp-state-machine-framework/actions/On.h>
#include <cpp-state-machine-framework/actions/Will.h>

#include "logger.h"

#include <cpp-state-machine-framework/utils/typestring.hpp>
using namespace TypeStringLiteralExploder;

struct OpenEvent
{
};

struct CloseEvent
{
};

struct LockEvent
{
    uint32_t newKey;    
};

struct UnlockEvent
{
    uint32_t key;    
};

struct ClosedState;
struct OpenState;
class ClashedState: public Will<ByDefault<Nothing>,
                    On<OpenEvent, TransitionTo<OpenState>>
                        >{
    public:
    using NameTS = TS("ClashedState");

};

class LockedState;

struct ClosedState : public Will<ByDefault<Nothing>,
                                 On<LockEvent, TransitionTo<LockedState>>,
                                 On<OpenEvent, TransitionTo<OpenState>>>
{
    using NameTS = TS("ClosedState");

    // using Will::handle;
    // OneOf<TransitionTo<OpenState>, TransitionTo<ClashedState>> handle(const OpenEvent& e)
    // {
    //     return TransitionTo<ClashedState>{};
    // }
};
struct EventsEmittingState ;

struct OpenState : public Will<ByDefault<Nothing>,
                               On<CloseEvent, TransitionTo<ClosedState>>>
{
    using NameTS = TS("OpenState");
    using Will::handle;

    TransitionTo<EventsEmittingState> handle(const OpenEvent& e) {
        return {};
    }
};

struct EventsEmittingState 
{
    using NameTS = TS("EventsEmittingState");

    template<typename Event>
    std::pair<TransitionTo<OpenState>, tuple<CloseEvent, OpenEvent, CloseEvent, OpenEvent>> handle(const Event& ) {
        return {};
    }
};


class BlockedState: public ByDefault<Nothing> {

};

class LockedState : public ByDefault<Nothing>
{
public:
    using NameTS = TS("LockedState");
    using ByDefault::handle;

    LockedState() {}
    LockedState(uint32_t key)
        : key(key)
    {
    }

    void onEnter(const LockEvent& e)
    {
        key = e.newKey;
    }

    Maybe<TransitionTo<ClosedState>> handle(const UnlockEvent& e)
    {
        if (e.key == key) {
            return TransitionTo<ClosedState>{};
        }
        return Nothing{};
    }

private:
    uint32_t key = 0;
};


using Door = StateMachine<ClosedState, LoggerStdOut, ClosedState, OpenState, LockedState, EventsEmittingState>;
using BlockedDoor = StateMachine<BlockedState, LoggerStdOut, BlockedState>;
