/**
 * A simple example of a repeating timer that prints out a message for
 * each timer expiration.
 */

#include <chrono>
#include <cstdio>
#include <sdeventplus/clock.hpp>
#include <sdeventplus/event.hpp>
#include <sdeventplus/utility/timer.hpp>
#include <string>

using sdeventplus::Clock;
using sdeventplus::ClockId;
using sdeventplus::Event;

constexpr auto clockId = ClockId::RealTime;
using Timer = sdeventplus::utility::Timer<clockId>;

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s [seconds]\n", argv[0]);
        return 1;
    }

    unsigned interval = std::stoul(argv[1]);
    fprintf(stderr, "Beating every %u seconds\n", interval);

    auto event = Event::get_default();
    Timer timer(event, [](Timer&) { printf("Beat\n"); },
                std::chrono::seconds{interval});
    return event.loop();
}
