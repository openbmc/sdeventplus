#include <cstdio>
#include <sdeventplus/event.hpp>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s [file]\n", argv[0]);
        return 1;
    }

    sdeventplus::Event event = sdeventplus::Event::get_default();
    return event.loop();
}
