
#include "Door.h"
#include <cpp-state-machine-framework/utils/typestring.hpp>

using namespace TypeStringLiteralExploder;

template <typename T>
void debug(T&&)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}


int test1() {   
    static_assert(std::is_trivially_copyable_v<Door>);

    Door door{ClosedState{}, OpenState{}, LockedState{0}, EventsEmittingState{}};

    door.handle(LockEvent{1234});
    door.handle(UnlockEvent{2});
    door.handle(UnlockEvent{1234});
    door.handle(OpenEvent{});
    door.handle(OpenEvent{});
    // door.handle(OpenEvent{});

    // door.handle(OpenEvent{});

    // door.handle(CloseEvent{});
    // door.handle(OpenEvent{});
    // door.handle(CloseEvent{});
    // door.handle(CloseEvent{});


    // door.handle(LockEvent{42}, UnlockEvent{42});
    return 1;
}

int test2() {   
    static_assert(std::is_trivially_copyable_v<BlockedDoor>);
    
    return 1;
}

int main()
{
    test1();
    return 0;
}
