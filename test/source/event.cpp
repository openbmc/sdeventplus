#include <cerrno>
#include <functional>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <sdeventplus/event.hpp>
#include <sdeventplus/exception.hpp>
#include <sdeventplus/source/event.hpp>
#include <sdeventplus/test/sdevent.hpp>
#include <systemd/sd-event.h>
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

using UniqueEvent = std::unique_ptr<sdeventplus::Event,
                                    std::function<void(sdeventplus::Event*)>>;

class EventTest : public testing::Test
{
  protected:
    testing::StrictMock<test::SdEventMock> mock;
    sd_event_source* const expected_source =
        reinterpret_cast<sd_event_source*>(1234);
    sd_event* const expected_event = reinterpret_cast<sd_event*>(2345);
    UniqueEvent event = make_event(expected_event);

    UniqueEvent make_event(sd_event* event)
    {
        auto deleter = [this, event](sdeventplus::Event* e) {
            EXPECT_CALL(this->mock, sd_event_unref(event))
                .WillOnce(Return(nullptr));
            delete e;
        };
        return UniqueEvent(
            new sdeventplus::Event(event, std::false_type(), &mock), deleter);
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

TEST_F(EventTest, DeferConstruct)
{
    EXPECT_CALL(mock, sd_event_ref(expected_event))
        .WillOnce(Return(expected_event));
    void* userdata;
    EXPECT_CALL(mock, sd_event_source_set_userdata(expected_source, testing::_))
        .WillOnce(DoAll(SaveArg<1>(&userdata), Return(nullptr)));
    sd_event_handler_t handler;
    EXPECT_CALL(mock, sd_event_add_defer(expected_event, testing::_, testing::_,
                                         nullptr))
        .WillOnce(DoAll(SetArgPointee<1>(expected_source), SaveArg<2>(&handler),
                        Return(0)));
    int completions = 0;
    Event::Callback callback = [&completions](Event&) { completions++; };
    Defer defer(*event, std::move(callback));
    EXPECT_EQ(&defer, userdata);
    EXPECT_FALSE(callback);
    EXPECT_EQ(0, completions);

    EXPECT_EQ(0, handler(nullptr, &defer));
    EXPECT_EQ(1, completions);

    expect_destruct();
}

TEST_F(EventTest, PostConstruct)
{
    EXPECT_CALL(mock, sd_event_ref(expected_event))
        .WillOnce(Return(expected_event));
    void* userdata;
    EXPECT_CALL(mock, sd_event_source_set_userdata(expected_source, testing::_))
        .WillOnce(DoAll(SaveArg<1>(&userdata), Return(nullptr)));
    sd_event_handler_t handler;
    EXPECT_CALL(mock, sd_event_add_post(expected_event, testing::_, testing::_,
                                        nullptr))
        .WillOnce(DoAll(SetArgPointee<1>(expected_source), SaveArg<2>(&handler),
                        Return(0)));
    int completions = 0;
    Event::Callback callback = [&completions](Event&) { completions++; };
    Post post(*event, std::move(callback));
    EXPECT_EQ(&post, userdata);
    EXPECT_FALSE(callback);
    EXPECT_EQ(0, completions);

    EXPECT_EQ(0, handler(nullptr, &post));
    EXPECT_EQ(1, completions);

    expect_destruct();
}

TEST_F(EventTest, ExitConstruct)
{
    EXPECT_CALL(mock, sd_event_ref(expected_event))
        .WillOnce(Return(expected_event));
    void* userdata;
    EXPECT_CALL(mock, sd_event_source_set_userdata(expected_source, testing::_))
        .WillOnce(DoAll(SaveArg<1>(&userdata), Return(nullptr)));
    sd_event_handler_t handler;
    EXPECT_CALL(mock, sd_event_add_exit(expected_event, testing::_, testing::_,
                                        nullptr))
        .WillOnce(DoAll(SetArgPointee<1>(expected_source), SaveArg<2>(&handler),
                        Return(0)));
    int completions = 0;
    Event::Callback callback = [&completions](Event&) { completions++; };
    Exit exit(*event, std::move(callback));
    EXPECT_EQ(&exit, userdata);
    EXPECT_FALSE(callback);
    EXPECT_EQ(0, completions);

    EXPECT_EQ(0, handler(nullptr, &exit));
    EXPECT_EQ(1, completions);

    expect_destruct();
}

TEST_F(EventTest, ConstructFailure)
{
    EXPECT_CALL(mock, sd_event_add_defer(expected_event, testing::_, testing::_,
                                         nullptr))
        .WillOnce(Return(-EINVAL));
    int completions = 0;
    Event::Callback callback = [&completions](Event&) { completions++; };
    EXPECT_THROW(Defer(*event, std::move(callback)), SdEventError);
    EXPECT_TRUE(callback);
    EXPECT_EQ(0, completions);
}

} // namespace
} // namespace source
} // namespace sdeventplus
