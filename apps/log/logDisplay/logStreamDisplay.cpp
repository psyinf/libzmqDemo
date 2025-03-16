#include <zmq.hpp>
#include <chrono>
#include <thread>
#include <iostream>
#include <CLI/CLI.hpp>
#include <nlohmann/json.hpp>
#include <format>
#include <print>

int main(int argc, char** argv)
try
{
    std::string logger_name;
    std::string logger_level;

    CLI::App app{"Log Stream Display"};

    auto logger_option = app.add_option("-l,--logger", logger_name, "Logger name to display")->default_str("");
    app.add_option("-v,--level", logger_level, "Level to display")->needs(logger_option);

    CLI11_PARSE(app, argc, argv);

    using namespace std::chrono_literals;

    // initialize the zmq context with a single IO thread
    zmq::context_t context{1};

    // construct a REP (reply) socket and bind to interface
    zmq::socket_t socket{context, zmq::socket_type::sub};

    socket.connect("tcp://localhost:5555");
    if (logger_name.empty())
    {
        socket.set(zmq::sockopt::subscribe, "");
        std::cout << "Subscribed to all" << std::endl;
    }
    else
    {
        auto str = std::format(R"("logger":"{}")", logger_name);
        if (!logger_level.empty()) { str += std::format(R"(,"level":"{}")", logger_level); }
        socket.set(zmq::sockopt::subscribe, "{" + str);
        std::cout << "Subscribed to " << str << std::endl;
    }
    // socket.set(zmq::sockopt::subscribe, R"({"logger":"zmq_logger","level":"debug")");

    for (;;)
    {
        zmq::message_t request;

        // receive a request from client
        socket.recv(request, zmq::recv_flags::none);
        // nicefy using fmt
        nlohmann::json j = nlohmann::json::parse(request.to_string());
        std::cout << j.dump(4) << std::endl;
        auto str = std::format("[{}] [{}] [{}] {}",
                               j["time"].get<int64_t>(),
                               j["logger"].get<std::string>(),
                               j["level"].get<std::string>(),
                               j["msg"].get<std::string>());
        // TODO timestamp formatting, level coloring
        std::puts(str.c_str());
    }

    return 0;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
    return 1;
}
catch (...)
{
    std::cerr << "Unknown exception" << std::endl;
    return 2;
}