#pragma once

struct OptionalBool {
    enum State { Unset, True, False };
    State state;
    
    OptionalBool() : state(Unset) {}
    explicit OptionalBool(bool value) : state(value ? True : False) {}
    
    bool operator==(const OptionalBool& other) const { return state == other.state; }
    bool operator!=(const OptionalBool& other) const { return state != other.state; }
    bool isSet() const { return state != Unset; }
    bool getValue() const { return state == True; }
};
