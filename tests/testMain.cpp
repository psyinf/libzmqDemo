#include <catch2/catch_session.hpp>

// artificial main to put all tests into one binary
int main(int argc, char* argv[])
{
    // your setup ...

    int result = Catch::Session().run(argc, argv);

    // your clean-up...

    return result;
}