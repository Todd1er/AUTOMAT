class Connection {
    enum class State : char { DISCONNECTED, CONNECTING, CONNECTED } state;
public:
    constexpr void process_event(connect const&) {
        switch(state) {
            default: break;
            case State::DISCONNECTED:
                establish(); state = State::CONNECTING; break;
        }
    }
    constexpr void process_event(disconnect const&) {
        switch(state) {
            default: break;
            case State::CONNECTING:
            case State::CONNECTED: close(); state = State::DISCONNECTED; break;
        }
    }
    constexpr void process_event(established const&) {
        if (state == State::CONNECTING) {
            state = State::CONNECTED;
        }
    }
    constexpr void process_event(ping const& event) {
        if (state == State::CONNECTED and is_valid(event)) {
            reset_timeout();
        }
    }
    constexpr void process_event(timeout const&) {
        if (state == State::CONNECTED) {
            establish();
            state = State::CONNECTING;
        }
    }
};
int main() {
    Connection connection{};
    connection.process_event(connect{});
    connection.process_event(established{});
    connection.process_event(ping{});
    connection.process_event(disconnect{});
    connection.process_event(connect{});
    connection.process_event(established{});
    connection.process_event(ping{});
}