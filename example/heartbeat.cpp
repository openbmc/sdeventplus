#include <chrono>
#include <cstdio>
#include <sdeventplus/clock.hpp>
#include <sdeventplus/event.hpp>
#include <sdeventplus/source/time.hpp>
#include <string>
#include <utility>

using sdeventplus::Clock;
using sdeventplus::ClockId;
using sdeventplus::Event;
using sdeventplus::source::Time;

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
    auto hbFunc = [interval](Time<ClockId::RealTime>& source,
                             Time<ClockId::RealTime>::TimePoint time) {
        printf("Beat\n");
        source.set_time(time + std::chrono::seconds{interval});
    };
    Time<ClockId::RealTime> time(event, Clock<ClockId::RealTime>(event).now(),
                                 std::chrono::seconds{1}, std::move(hbFunc));
    time.set_enabled(sdeventplus::source::Enabled::On);
    return event.loop();
}
