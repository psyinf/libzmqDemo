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
    zmq::socket_t socket{context, zmq::socket_type::sub};
    
    socket.connect("tcp://localhost:5555");
    socket.set(zmq::sockopt::subscribe, "");
    
    for (;;) 
    {
        zmq::message_t request;

        // receive a request from client
        socket.recv(request, zmq::recv_flags::none);
        std::cout << "Received " << request.to_string() << std::endl;

    }

    return 0;
}