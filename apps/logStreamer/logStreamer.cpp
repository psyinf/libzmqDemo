#include <string>
#include <iostream>

#include <zmq.hpp>
#include <chrono>
#include <thread>
#include <spdlog/spdlog.h>

// own sink
#include <spdlog/sinks/base_sink.h>

#include <spdlog/details/null_mutex.h>

#include <zmq.hpp>

class ZMQSink : public spdlog::sinks::base_sink<spdlog::details::null_mutex>
{
    // Topics are derived by the first n bytes of the message.
    // so we are adding the logger name as a topic
    // we will have two logger flavors:
    // the first will use logger_name::level as the first n byte, followed by a fixed order
    // like this: logger_name::level::timestamp::message. We later might make this configurable
    // second flavor will produce json like this
    // {
    //     "logger": "logger_name",
    //     "level": "level",
    //     "timestamp": "timestamp", ...
    // }
    // so subscribers need to use {"logger":"name","level":"2"} e.g. as topic
public:
    enum class Format
    {
        JSON,
        TEXT
    };

    //
    //     enum class Fields
    //     {
    //         Logger,
    //         Level,
    //         Timestamp,
    //         Message
    //     };

    ZMQSink(std::shared_ptr<zmq::context_t> context, const std::string& endpoint)
      : _context(context)
      , _socket(std::make_shared<zmq::socket_t>(*_context, zmq::socket_type::pub))
    {
        _socket->bind(endpoint);
    }

protected:
    void sink_it_(const spdlog::details::log_msg& msg) override
    {
        _socket->send(zmq::buffer(format(msg)), zmq::send_flags::none);
    }

    std::string format(const spdlog::details::log_msg& msg)
    {
        switch (_format)
        {
        case Format::JSON:
            return format_as_json(msg);
        case Format::TEXT:
            return format_as_plain_text(msg);
        default:
            throw std::runtime_error("Unsupported format");
        }
    }

    std::string format_as_json(const spdlog::details::log_msg& msg)
    {
        return fmt::format(R"({{"logger":"{0}","level":"{1}","time":{2},"msg":"{3}"}})",
                           msg.logger_name,
                           spdlog::level::to_string_view(msg.level),
                           std::chrono::duration_cast<std::chrono::milliseconds>(msg.time.time_since_epoch()).count(),
                           msg.payload);
    }

    std::string format_as_plain_text(const spdlog::details::log_msg& msg)
    {
        return fmt::format("{0}::{1}::{2}::{3}",
                           msg.logger_name,
                           spdlog::level::to_string_view(msg.level),
                           std::chrono::duration_cast<std::chrono::milliseconds>(msg.time.time_since_epoch()).count(),
                           msg.payload);
    }

    void flush_() override {}

private:
    std::shared_ptr<zmq::context_t> _context;
    std::shared_ptr<zmq::socket_t>  _socket;
    Format                          _format{Format::JSON};
};

int main()
{
    auto context_ptr = std::make_shared<zmq::context_t>(1);

    auto zmq_sink = std::make_shared<ZMQSink>(context_ptr, "tcp://localhost:5555");
    // create a logger
    auto logger = std::make_shared<spdlog::logger>("proto1", zmq_sink);
    logger->set_level(spdlog::level::trace);
    auto logger2 = std::make_shared<spdlog::logger>("proto2", zmq_sink);

    zmq_sink->set_level(spdlog::level::trace);
    using namespace std::chrono_literals;

    bool odd_even = true;
    for (auto request_num = 0; request_num < 100; ++request_num)
    {
        auto str = std::format("Message number {}", request_num);
        // send the request to the server
        // send to different loggers
        if (odd_even)
        {
            logger->info(str);
            // send to trace
            logger->trace(str + "_tracelevel");
        }
        else
        {
            logger2->info(str);
            // send to trace
            logger2->error(str + "_errorlevel");
        }
        odd_even = !odd_even;
        std::cout << "Sent " << str << std::endl;
        std::this_thread::sleep_for(1s);
    }

    return 0;
}