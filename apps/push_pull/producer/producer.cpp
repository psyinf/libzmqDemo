#include <zmq.hpp>
#include <chrono>
#include <thread>
#include <iostream>

int main()
{
    using namespace std::chrono_literals;

    // initialize the zmq context with a single IO thread
    zmq::context_t context{1};

    // construct a REP (reply) socket and bind to interface
    zmq::socket_t socket{context, zmq::socket_type::push};
    socket.bind("tcp://*:5555");

    // prepare some static data for responses
    const std::string data{"World"};
    using namespace std::chrono_literals;
    for (unsigned i = 0; i < 1000; ++i)
    {
        auto message = std::string("Workload ") + std::to_string(i);
        // send work
        socket.send(zmq::buffer(message), zmq::send_flags::none);
        std::cout << "Sent Workload " << i << std::endl;
        std::this_thread::sleep_for(100ms);
    }

    return 0;
}