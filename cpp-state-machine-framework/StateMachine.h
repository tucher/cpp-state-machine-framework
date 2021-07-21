#pragma once

#include <cpp-state-machine-framework/types/Types.h>

#include <tuple>
#include <variant>

#include <cpp-state-machine-framework/utils/tuple_type_id_extractor.h>
#include <cpp-state-machine-framework/utils/tuple.h>
#include <iostream>
template <typename InitState, typename LoggerT, typename... States>
class StateMachine
{
    using StatesTupleT = tuple<States...>;
    StatesTupleT states;
    std::size_t currentStateIndex = TypeIndex<InitState, StatesTupleT>::value;
    template<typename T>
    using StateIDGetter = TupleTypeIDExtractor::type_id_ts<T, std::tuple<States...>>;

    static constexpr char const * GetStateID(std::size_t stateIndex, const StatesTupleT & tpl) {
        return visit_at(tpl, stateIndex, [](const auto & st){
            using StateT = std::decay_t<decltype(st)>;
            return StateIDGetter<StateT>::c_str();
        });        
    }
public:
    StateMachine() = default;

    StateMachine(States... _states)
        : states{std::move(_states)...}
    {
        LoggerT::log("Starting from state", GetStateID(currentStateIndex, states));
    }

    template <typename State>
    State& transitionTo()
    {
        LoggerT::log("State changed from", GetStateID(currentStateIndex, states), "to", StateIDGetter<State>::c_str());
        State& state = std::get<State>(states);
        currentStateIndex = TypeIndex<State, StatesTupleT>::value;
        return state;
    }

    void handle()
    {
    }

    template <typename Event, typename ... RestEventTypes>
    void handle(const Event& event, const RestEventTypes & ... restEvents)
    {
        handleBy(event, *this);
        handle(restEvents...);
    }
    template <typename ... EventTypes, std::size_t ... Is>
    void handle(const tuple<EventTypes...> & eventTuple, std::index_sequence<Is...>)
    {
        handle(std::get<Is>(eventTuple)...);
    }

    template <typename Action, typename Event, typename Machine, typename State>
    static void processStateHandleResult(Action & action, State & state, Event & event, Machine & machine) {
        action.execute(machine, state, event);
    }

    template <typename Action, typename Event, typename Machine, typename State, typename ... EventTypes>
    static void processStateHandleResult(std::pair<Action  , tuple<EventTypes...>> actionAndEvents, State & state, Event & event, Machine & machine) {
        auto & [action, events] = actionAndEvents;
        action.execute(machine, state, event);
        machine.handle(events, std::index_sequence_for<EventTypes...>{});
    }


    template <typename Event, typename Machine>
    void handleBy(const Event& event, Machine& machine)
    {
        auto passEventToState = [&machine, &event] (auto & state) {
            auto handleResult = state.handle(event);
            processStateHandleResult(handleResult, state, event, machine);
            // action.execute(machine, state, event);
        };

        visit_at(states, currentStateIndex, passEventToState);
    }

    constexpr static Types<States...> getStateTypes() { return {}; }
};
