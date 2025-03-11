#include <string>
#include <iostream>

#include <zmq.hpp>
#include <chrono>
#include <thread>

int main()
{
    using namespace std::chrono_literals;
    // initialize the zmq context with a single IO thread
    zmq::context_t context{1};

    // construct a REQ (request) socket and connect to interface
    zmq::socket_t socket{context, zmq::socket_type::pub};
    socket.bind("tcp://localhost:5555");

    // set up some static data to send


    for (auto request_num = 0; request_num < 100; ++request_num)
    {
        auto str = std::format("Hello {}", request_num);
        // send the request to the server
        socket.send(zmq::buffer(str), zmq::send_flags::none);
        

        std::cout << "Sent " << str << std::endl;
        std::this_thread::sleep_for(1s);
    }

    return 0;
}