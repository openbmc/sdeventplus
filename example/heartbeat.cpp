#include <chrono>
#include <cstdio>
#include <sdeventplus/clock.hpp>
#include <sdeventplus/event.hpp>
#include <sdeventplus/source/time.hpp>
#include <string>
#include <utility>

using sdeventplus::Event;
using sdeventplus::source::Enabled;

constexpr auto clockId = sdeventplus::ClockId::RealTime;
using Clock = sdeventplus::Clock<clockId>;
using Time = sdeventplus::source::Time<clockId>;

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
    auto hbFunc = [interval](Time& source, Time::TimePoint time) {
        printf("Beat\n");
        source.set_time(time + std::chrono::seconds{interval});
    };
    Time time(event, Clock(event).now(), std::chrono::seconds{1},
              std::move(hbFunc));
    time.set_enabled(Enabled::On);
    return event.loop();
}
