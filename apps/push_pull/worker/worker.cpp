#include <string>
#include <iostream>

#include <chrono>
#include <thread>

#include <zmq.hpp>

int main()
{
    // initialize the zmq context with a single IO thread
    zmq::context_t context{1};

    // connect to producer
    zmq::socket_t socket{context, zmq::socket_type::pull};
    socket.connect("tcp://localhost:5555");

    bool stopped{};

    while (!stopped)
    {
        zmq::message_t request;
        socket.recv(request, zmq::recv_flags::none);
        // process the request
        std::string message = request.to_string();
        std::cout << "Received: " << message << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}