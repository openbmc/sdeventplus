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

        // Time sources are oneshot and are based on an absolute time
        // we need to reconfigure the time source to go off again after the
        // configured interval and re-enable it.
        source.set_time(time + std::chrono::seconds{interval});
        source.set_enabled(Enabled::OneShot);
    };
    Time time(event, Clock(event).now(), std::chrono::seconds{1},
              std::move(hbFunc));
    return event.loop();
}
