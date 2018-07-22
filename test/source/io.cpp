#include <cerrno>
#include <functional>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <sdeventplus/event.hpp>
#include <sdeventplus/exception.hpp>
#include <sdeventplus/source/io.hpp>
#include <sdeventplus/test/sdevent.hpp>
#include <systemd/sd-event.h>
#include <type_traits>
#include <utility>

namespace sdeventplus
{
namespace source
{
namespace
{

using testing::DoAll;
using testing::Return;
using testing::SaveArg;
using testing::SetArgPointee;

using UniqueEvent = std::unique_ptr<Event, std::function<void(Event*)>>;

class IOTest : public testing::Test
{
  protected:
    testing::StrictMock<test::SdEventMock> mock;
    sd_event_source* const expected_source =
        reinterpret_cast<sd_event_source*>(1234);
    sd_event* const expected_event = reinterpret_cast<sd_event*>(2345);
    UniqueEvent event = make_event(expected_event);

    UniqueEvent make_event(sd_event* event)
    {
        auto deleter = [this, event](Event* e) {
            EXPECT_CALL(this->mock, sd_event_unref(event))
                .WillOnce(Return(nullptr));
            delete e;
        };
        return UniqueEvent(new Event(event, std::false_type(), &mock), deleter);
    }

    void expect_destruct()
    {
        {
            testing::InSequence sequence;
            EXPECT_CALL(mock, sd_event_source_set_enabled(expected_source,
                                                          SD_EVENT_OFF))
                .WillOnce(Return(0));
            EXPECT_CALL(mock, sd_event_source_unref(expected_source))
                .WillOnce(Return(nullptr));
        }
        EXPECT_CALL(mock, sd_event_unref(expected_event))
            .WillOnce(Return(nullptr));
    }
};

TEST_F(IOTest, ConstructSuccess)
{
    const int fd = 10;
    const uint32_t events = EPOLLIN | EPOLLET;

    EXPECT_CALL(mock, sd_event_ref(expected_event))
        .WillOnce(Return(expected_event));
    sd_event_io_handler_t handler;
    EXPECT_CALL(mock, sd_event_add_io(expected_event, testing::_, fd, events,
                                      testing::_, nullptr))
        .WillOnce(DoAll(SetArgPointee<1>(expected_source), SaveArg<4>(&handler),
                        Return(0)));
    void* userdata;
    EXPECT_CALL(mock, sd_event_source_set_userdata(expected_source, testing::_))
        .WillOnce(DoAll(SaveArg<1>(&userdata), Return(nullptr)));
    int completions = 0;
    int return_fd;
    uint32_t return_revents;
    IO::Callback callback = [&](IO&, int fd, uint32_t revents) {
        return_fd = fd;
        return_revents = revents;
        completions++;
    };
    IO io(*event, fd, events, std::move(callback));
    EXPECT_FALSE(callback);
    EXPECT_EQ(&io, userdata);
    EXPECT_EQ(0, completions);

    EXPECT_EQ(0, handler(nullptr, 5, EPOLLIN, &io));
    EXPECT_EQ(1, completions);
    EXPECT_EQ(5, return_fd);
    EXPECT_EQ(EPOLLIN, return_revents);

    expect_destruct();
}

TEST_F(IOTest, ConstructError)
{
    const int fd = 10;
    const uint32_t events = EPOLLIN | EPOLLET;

    EXPECT_CALL(mock, sd_event_add_io(expected_event, testing::_, fd, events,
                                      testing::_, nullptr))
        .WillOnce(Return(-EINVAL));
    int completions = 0;
    IO::Callback callback = [&completions](IO&, int, uint32_t) {
        completions++;
    };
    EXPECT_THROW(IO(*event, fd, events, std::move(callback)), SdEventError);
    EXPECT_TRUE(callback);
    EXPECT_EQ(0, completions);
}

class IOMethodTest : public IOTest
{
  protected:
    std::unique_ptr<IO> io;

    void SetUp()
    {
        const int fd = 7;
        const int events = EPOLLOUT;

        EXPECT_CALL(mock, sd_event_ref(expected_event))
            .WillOnce(Return(expected_event));
        EXPECT_CALL(mock, sd_event_add_io(expected_event, testing::_, fd,
                                          events, testing::_, nullptr))
            .WillOnce(DoAll(SetArgPointee<1>(expected_source), Return(0)));
        EXPECT_CALL(mock,
                    sd_event_source_set_userdata(expected_source, testing::_))
            .WillOnce(Return(nullptr));
        io =
            std::make_unique<IO>(*event, fd, events, [](IO&, int, uint32_t) {});
    }

    void TearDown()
    {
        expect_destruct();
        io.reset();
    }
};

TEST_F(IOMethodTest, GetFdSuccess)
{
    const int fd = 5;
    EXPECT_CALL(mock, sd_event_source_get_io_fd(expected_source))
        .WillOnce(Return(fd));
    EXPECT_EQ(fd, io->get_fd());
}

TEST_F(IOMethodTest, GetFdError)
{
    EXPECT_CALL(mock, sd_event_source_get_io_fd(expected_source))
        .WillOnce(Return(-EINVAL));
    EXPECT_THROW(io->get_fd(), SdEventError);
}

TEST_F(IOMethodTest, SetFdSuccess)
{
    const int fd = 5;
    EXPECT_CALL(mock, sd_event_source_set_io_fd(expected_source, fd))
        .WillOnce(Return(0));
    io->set_fd(fd);
}

TEST_F(IOMethodTest, SetFdError)
{
    const int fd = 3;
    EXPECT_CALL(mock, sd_event_source_set_io_fd(expected_source, fd))
        .WillOnce(Return(-EINVAL));
    EXPECT_THROW(io->set_fd(fd), SdEventError);
}

TEST_F(IOMethodTest, GetFdOwnSuccess)
{
    EXPECT_CALL(mock, sd_event_source_get_io_fd_own(expected_source))
        .WillOnce(Return(3));
    EXPECT_TRUE(io->get_fd_own());
    EXPECT_CALL(mock, sd_event_source_get_io_fd_own(expected_source))
        .WillOnce(Return(0));
    EXPECT_FALSE(io->get_fd_own());
}

TEST_F(IOMethodTest, GetFdOwnError)
{
    EXPECT_CALL(mock, sd_event_source_get_io_fd_own(expected_source))
        .WillOnce(Return(-EINVAL));
    EXPECT_THROW(io->get_fd_own(), SdEventError);
}

TEST_F(IOMethodTest, SetFdOwnSuccess)
{
    EXPECT_CALL(mock, sd_event_source_set_io_fd_own(expected_source, true))
        .WillOnce(Return(0));
    io->set_fd_own(true);
}

TEST_F(IOMethodTest, SetFdOwnError)
{
    EXPECT_CALL(mock, sd_event_source_set_io_fd_own(expected_source, false))
        .WillOnce(Return(-EINVAL));
    EXPECT_THROW(io->set_fd_own(false), SdEventError);
}

TEST_F(IOMethodTest, GetEventsSuccess)
{
    const uint32_t events = EPOLLIN | EPOLLOUT;
    EXPECT_CALL(mock,
                sd_event_source_get_io_events(expected_source, testing::_))
        .WillOnce(DoAll(SetArgPointee<1>(events), Return(0)));
    EXPECT_EQ(events, io->get_events());
}

TEST_F(IOMethodTest, GetEventsError)
{
    EXPECT_CALL(mock,
                sd_event_source_get_io_events(expected_source, testing::_))
        .WillOnce(Return(-EINVAL));
    EXPECT_THROW(io->get_events(), SdEventError);
}

TEST_F(IOMethodTest, SetEventsSuccess)
{
    const uint32_t events = EPOLLIN | EPOLLOUT;
    EXPECT_CALL(mock, sd_event_source_set_io_events(expected_source, events))
        .WillOnce(Return(0));
    io->set_events(events);
}

TEST_F(IOMethodTest, SetEventsError)
{
    const uint32_t events = EPOLLIN | EPOLLOUT;
    EXPECT_CALL(mock, sd_event_source_set_io_events(expected_source, events))
        .WillOnce(Return(-EINVAL));
    EXPECT_THROW(io->set_events(events), SdEventError);
}

TEST_F(IOMethodTest, GetREventsSuccess)
{
    const uint32_t revents = EPOLLOUT;
    EXPECT_CALL(mock,
                sd_event_source_get_io_revents(expected_source, testing::_))
        .WillOnce(DoAll(SetArgPointee<1>(revents), Return(0)));
    EXPECT_EQ(revents, io->get_revents());
}

TEST_F(IOMethodTest, GetREventsError)
{
    EXPECT_CALL(mock,
                sd_event_source_get_io_revents(expected_source, testing::_))
        .WillOnce(Return(-EINVAL));
    EXPECT_THROW(io->get_revents(), SdEventError);
}

} // namespace
} // namespace source
} // namespace sdeventplus
