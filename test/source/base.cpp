#include <cerrno>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <sdeventplus/event.hpp>
#include <sdeventplus/exception.hpp>
#include <sdeventplus/internal/sdevent.hpp>
#include <sdeventplus/source/base.hpp>
#include <sdeventplus/test/sdevent.hpp>
#include <string>
#include <system_error>
#include <systemd/sd-event.h>
#include <type_traits>

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

class BaseImpl : public Base
{
  public:
    BaseImpl(const Event& event, sd_event_source* source) : Base(event, source)
    {
    }
    BaseImpl(const Event& event, sd_event_source* source, std::false_type) :
        Base(event, source, std::false_type())
    {
    }
};

class BaseTest : public testing::Test
{
  protected:
    testing::StrictMock<test::SdEventMock> mock;
    sd_event_source* expected_source = reinterpret_cast<sd_event_source*>(1234);
    sd_event* expected_event = reinterpret_cast<sd_event*>(2345);
    std::unique_ptr<Event> event;

    virtual void SetUp()
    {
        event =
            std::make_unique<Event>(expected_event, std::false_type(), &mock);
    }
    virtual void TearDown()
    {
        EXPECT_CALL(mock, sd_event_unref(expected_event))
            .WillOnce(Return(nullptr));
        event.reset();
    }
};

TEST_F(BaseTest, NewBaseRef)
{
    EXPECT_CALL(mock, sd_event_ref(expected_event))
        .WillOnce(Return(expected_event));
    EXPECT_CALL(mock, sd_event_source_ref(expected_source))
        .WillOnce(Return(expected_source));
    BaseImpl source(*event, expected_source);
    EXPECT_EQ(expected_source, source.get());
    EXPECT_NE(event.get(), &source.get_event());
    EXPECT_EQ(expected_event, source.get_event().get());
    EXPECT_FALSE(source.get_prepare());

    {
        testing::InSequence seq;
        EXPECT_CALL(mock,
                    sd_event_source_set_enabled(expected_source, SD_EVENT_OFF))
            .WillOnce(Return(0));
        EXPECT_CALL(mock, sd_event_source_unref(expected_source))
            .WillOnce(Return(nullptr));
    }
    EXPECT_CALL(mock, sd_event_unref(expected_event)).WillOnce(Return(nullptr));
}

TEST_F(BaseTest, NewBaseNoRef)
{
    EXPECT_CALL(mock, sd_event_ref(expected_event))
        .WillOnce(Return(expected_event));
    BaseImpl source(*event, expected_source, std::false_type());
    EXPECT_EQ(expected_source, source.get());
    EXPECT_NE(event.get(), &source.get_event());
    EXPECT_EQ(expected_event, source.get_event().get());
    EXPECT_FALSE(source.get_prepare());

    {
        testing::InSequence seq;
        EXPECT_CALL(mock,
                    sd_event_source_set_enabled(expected_source, SD_EVENT_OFF))
            .WillOnce(Return(0));
        EXPECT_CALL(mock, sd_event_source_unref(expected_source))
            .WillOnce(Return(nullptr));
    }
    EXPECT_CALL(mock, sd_event_unref(expected_event)).WillOnce(Return(nullptr));
}

TEST_F(BaseTest, NoSource)
{
    EXPECT_CALL(mock, sd_event_ref(expected_event))
        .WillOnce(Return(expected_event));
    BaseImpl source(*event, nullptr, std::false_type());
    EXPECT_EQ(nullptr, source.get());
    EXPECT_NE(event.get(), &source.get_event());
    EXPECT_EQ(expected_event, source.get_event().get());
    EXPECT_FALSE(source.get_prepare());

    EXPECT_CALL(mock, sd_event_source_unref(nullptr)).WillOnce(Return(nullptr));
    EXPECT_CALL(mock, sd_event_unref(expected_event)).WillOnce(Return(nullptr));
}

class BaseMethodTest : public BaseTest
{
  protected:
    std::unique_ptr<BaseImpl> base;

    void SetUp() override
    {
        BaseTest::SetUp();
        EXPECT_CALL(mock, sd_event_ref(expected_event))
            .WillOnce(Return(expected_event));
        base = std::make_unique<BaseImpl>(*event, expected_source,
                                          std::false_type());
    }

    void TearDown() override
    {
        {
            testing::InSequence seq;
            EXPECT_CALL(mock, sd_event_source_set_enabled(expected_source,
                                                          SD_EVENT_OFF))
                .WillOnce(Return(0));
            EXPECT_CALL(mock, sd_event_source_unref(expected_source))
                .WillOnce(Return(nullptr));
        }
        EXPECT_CALL(mock, sd_event_unref(expected_event))
            .WillOnce(Return(nullptr));
        base.reset();
        BaseTest::TearDown();
    }
};

TEST_F(BaseMethodTest, GetDescriptionSuccess)
{
    const char* expected = "test_desc";
    EXPECT_CALL(mock,
                sd_event_source_get_description(expected_source, testing::_))
        .WillOnce(DoAll(SetArgPointee<1>(expected), Return(0)));
    // Intentionally comparing pointers to make sure no copying is happening
    EXPECT_EQ(expected, base->get_description());
}

TEST_F(BaseMethodTest, GetDescriptionError)
{
    EXPECT_CALL(mock,
                sd_event_source_get_description(expected_source, testing::_))
        .WillOnce(Return(-EINVAL));
    EXPECT_THROW(base->get_description(), SdEventError);
}

TEST_F(BaseMethodTest, SetDescriptionSuccess)
{
    const char* expected = "test desc";
    // Intentionally comparing pointers to make sure no copying is happening
    EXPECT_CALL(mock,
                sd_event_source_set_description(expected_source, expected))
        .WillOnce(Return(0));
    base->set_description(expected);
}

TEST_F(BaseMethodTest, SetDescriptionError)
{
    const char* expected = "test desc";
    // Intentionally comparing pointers to make sure no copying is happening
    EXPECT_CALL(mock,
                sd_event_source_set_description(expected_source, expected))
        .WillOnce(Return(-EINVAL));
    EXPECT_THROW(base->set_description(expected), SdEventError);
}

TEST_F(BaseMethodTest, SetPrepareCallback)
{
    bool completed = false;
    Base::Callback callback = [&completed](Base&) { completed = true; };
    sd_event_handler_t event_handler;
    EXPECT_CALL(mock, sd_event_source_set_prepare(expected_source, testing::_))
        .WillOnce(DoAll(SaveArg<1>(&event_handler), Return(0)));
    base->set_prepare(std::move(callback));
    EXPECT_TRUE(base->get_prepare());
    EXPECT_FALSE(callback);
    EXPECT_FALSE(completed);

    EXPECT_EQ(0, event_handler(nullptr, base.get()));
    EXPECT_TRUE(completed);
}

TEST_F(BaseMethodTest, SetPrepareCallbackNoUserdata)
{
    Base::Callback callback = [](Base&) {};
    sd_event_handler_t event_handler;
    EXPECT_CALL(mock, sd_event_source_set_prepare(expected_source, testing::_))
        .WillOnce(DoAll(SaveArg<1>(&event_handler), Return(0)));
    base->set_prepare(std::move(callback));
    EXPECT_TRUE(base->get_prepare());
    EXPECT_FALSE(callback);

    EXPECT_EQ(-EINVAL, event_handler(nullptr, nullptr));
}

TEST_F(BaseMethodTest, SetPrepareNull)
{
    EXPECT_CALL(mock, sd_event_source_set_prepare(expected_source, nullptr))
        .WillOnce(Return(0));
    base->set_prepare(nullptr);
    EXPECT_EQ(-ENOSYS, base->prepareCallback());
}

TEST_F(BaseMethodTest, SetPrepareSystemError)
{
    Base::Callback callback = [](Base&) {
        throw std::system_error(EBUSY, std::generic_category());
    };
    EXPECT_CALL(mock, sd_event_source_set_prepare(expected_source, testing::_))
        .WillOnce(Return(0));
    base->set_prepare(std::move(callback));
    EXPECT_TRUE(base->get_prepare());
    EXPECT_FALSE(callback);
    EXPECT_EQ(-EBUSY, base->prepareCallback());
}

TEST_F(BaseMethodTest, SetPrepareUnknownException)
{
    Base::Callback callback = [](Base&) { throw static_cast<int>(1); };
    EXPECT_CALL(mock, sd_event_source_set_prepare(expected_source, testing::_))
        .WillOnce(Return(0));
    base->set_prepare(std::move(callback));
    EXPECT_TRUE(base->get_prepare());
    EXPECT_FALSE(callback);
    EXPECT_EQ(-ENOSYS, base->prepareCallback());
}

TEST_F(BaseMethodTest, SetPrepareError)
{
    Base::Callback callback = [](Base&) {};
    EXPECT_CALL(mock, sd_event_source_set_prepare(expected_source, testing::_))
        .WillOnce(Return(-EINVAL));
    EXPECT_THROW(base->set_prepare(std::move(callback)), SdEventError);
    EXPECT_FALSE(base->get_prepare());
    EXPECT_TRUE(callback);
    EXPECT_EQ(-ENOSYS, base->prepareCallback());
}

TEST_F(BaseMethodTest, GetPendingSuccess)
{
    EXPECT_CALL(mock, sd_event_source_get_pending(expected_source))
        .WillOnce(Return(0));
    EXPECT_EQ(0, base->get_pending());
    EXPECT_CALL(mock, sd_event_source_get_pending(expected_source))
        .WillOnce(Return(4));
    EXPECT_EQ(4, base->get_pending());
}

TEST_F(BaseMethodTest, GetPendingError)
{
    EXPECT_CALL(mock, sd_event_source_get_pending(expected_source))
        .WillOnce(Return(-EINVAL));
    EXPECT_THROW(base->get_pending(), SdEventError);
}

TEST_F(BaseMethodTest, GetPrioritySuccess)
{
    EXPECT_CALL(mock, sd_event_source_get_priority(expected_source, testing::_))
        .WillOnce(DoAll(SetArgPointee<1>(1024), Return(0)));
    EXPECT_EQ(1024, base->get_priority());
}

TEST_F(BaseMethodTest, GetPriorityError)
{
    EXPECT_CALL(mock, sd_event_source_get_priority(expected_source, testing::_))
        .WillOnce(Return(-EINVAL));
    EXPECT_THROW(base->get_priority(), SdEventError);
}

TEST_F(BaseMethodTest, SetPrioritySuccess)
{
    EXPECT_CALL(mock, sd_event_source_set_priority(expected_source, 1024))
        .WillOnce(Return(0));
    base->set_priority(1024);
}

TEST_F(BaseMethodTest, SetPriorityError)
{
    EXPECT_CALL(mock, sd_event_source_set_priority(expected_source, 1024))
        .WillOnce(Return(-EINVAL));
    EXPECT_THROW(base->set_priority(1024), SdEventError);
}

TEST_F(BaseMethodTest, GetEnabledSuccess)
{
    EXPECT_CALL(mock, sd_event_source_get_enabled(expected_source, testing::_))
        .WillOnce(DoAll(SetArgPointee<1>(SD_EVENT_ON), Return(0)));
    EXPECT_EQ(SD_EVENT_ON, base->get_enabled());
}

TEST_F(BaseMethodTest, GetEnabledError)
{
    EXPECT_CALL(mock, sd_event_source_get_enabled(expected_source, testing::_))
        .WillOnce(Return(-EINVAL));
    EXPECT_THROW(base->get_enabled(), SdEventError);
}

TEST_F(BaseMethodTest, SetEnabledSuccess)
{
    EXPECT_CALL(mock, sd_event_source_set_enabled(expected_source, SD_EVENT_ON))
        .WillOnce(Return(0));
    base->set_enabled(SD_EVENT_ON);
}

TEST_F(BaseMethodTest, SetEnabledError)
{
    EXPECT_CALL(mock, sd_event_source_set_enabled(expected_source, SD_EVENT_ON))
        .WillOnce(Return(-EINVAL));
    EXPECT_THROW(base->set_enabled(SD_EVENT_ON), SdEventError);
}

} // namespace
} // namespace source
} // namespace sdeventplus
