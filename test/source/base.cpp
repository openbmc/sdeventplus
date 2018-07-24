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

    using Base::get_prepare;
};

class BaseTest : public testing::Test
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

    // Using a unique_ptr to make sure we don't get any superfluous moves or
    // copies.
    std::unique_ptr<BaseImpl> make_base(const Event& event,
                                        sd_event_source* source)
    {
        EXPECT_CALL(mock, sd_event_ref(event.get()))
            .WillOnce(Return(event.get()));
        void* userdata;
        EXPECT_CALL(mock, sd_event_source_set_userdata(source, testing::_))
            .WillOnce(DoAll(SaveArg<1>(&userdata), Return(nullptr)));
        auto ret = std::make_unique<BaseImpl>(event, source, std::false_type());
        EXPECT_EQ(ret.get(), userdata);
        EXPECT_EQ(source, ret->get());
        EXPECT_NE(&event, &ret->get_event());
        EXPECT_EQ(event.get(), ret->get_event().get());
        EXPECT_FALSE(ret->get_prepare());
        return ret;
    }

    void set_prepare_placeholder(BaseImpl& base)
    {
        EXPECT_CALL(mock, sd_event_source_set_prepare(base.get(), testing::_))
            .WillOnce(Return(0));
        base.set_prepare([](Base&) {});
        EXPECT_TRUE(base.get_prepare());
    }

    void empty_base(BaseImpl&& other)
    {
        void* userdata;
        EXPECT_CALL(mock, sd_event_source_set_userdata(other.get(), testing::_))
            .WillOnce(DoAll(SaveArg<1>(&userdata), Return(nullptr)));
        BaseImpl mover(std::move(other));
        EXPECT_EQ(&mover, userdata);
        EXPECT_EQ(nullptr, other.get());
        EXPECT_EQ(nullptr, other.get_event().get());
        EXPECT_FALSE(other.get_prepare());

        expect_base_destruct(mover.get_event(), mover.get());
    }

    void expect_base_destruct(const Event& event, sd_event_source* source)
    {
        {
            testing::InSequence seq;
            EXPECT_CALL(mock, sd_event_source_set_enabled(source, SD_EVENT_OFF))
                .WillOnce(Return(0));
            EXPECT_CALL(mock, sd_event_source_unref(source))
                .WillOnce(Return(nullptr));
        }
        EXPECT_CALL(mock, sd_event_unref(event.get()))
            .WillOnce(Return(nullptr));
    }
};

TEST_F(BaseTest, NewBaseRef)
{
    EXPECT_CALL(mock, sd_event_ref(expected_event))
        .WillOnce(Return(expected_event));
    EXPECT_CALL(mock, sd_event_source_ref(expected_source))
        .WillOnce(Return(expected_source));
    void* userdata;
    EXPECT_CALL(mock, sd_event_source_set_userdata(expected_source, testing::_))
        .WillOnce(DoAll(SaveArg<1>(&userdata), Return(nullptr)));
    BaseImpl source(*event, expected_source);
    EXPECT_EQ(&source, userdata);
    EXPECT_EQ(expected_source, source.get());
    EXPECT_NE(event.get(), &source.get_event());
    EXPECT_EQ(expected_event, source.get_event().get());
    EXPECT_FALSE(source.get_prepare());

    expect_base_destruct(*event, expected_source);
}

TEST_F(BaseTest, NewBaseNoRef)
{
    EXPECT_CALL(mock, sd_event_ref(expected_event))
        .WillOnce(Return(expected_event));
    void* userdata;
    EXPECT_CALL(mock, sd_event_source_set_userdata(expected_source, testing::_))
        .WillOnce(DoAll(SaveArg<1>(&userdata), Return(nullptr)));
    BaseImpl source(*event, expected_source, std::false_type());
    EXPECT_EQ(&source, userdata);
    EXPECT_EQ(expected_source, source.get());
    EXPECT_NE(event.get(), &source.get_event());
    EXPECT_EQ(expected_event, source.get_event().get());
    EXPECT_FALSE(source.get_prepare());

    expect_base_destruct(*event, expected_source);
}

TEST_F(BaseTest, MoveConstruct)
{
    std::unique_ptr<BaseImpl> source1 = make_base(*event, expected_source);
    set_prepare_placeholder(*source1);

    void* userdata;
    EXPECT_CALL(mock, sd_event_source_set_userdata(expected_source, testing::_))
        .WillOnce(DoAll(SaveArg<1>(&userdata), Return(nullptr)));
    BaseImpl source2(std::move(*source1));
    EXPECT_EQ(&source2, userdata);
    EXPECT_EQ(nullptr, source1->get());
    EXPECT_EQ(nullptr, source1->get_event().get());
    EXPECT_FALSE(source1->get_prepare());
    EXPECT_EQ(expected_source, source2.get());
    EXPECT_EQ(expected_event, source2.get_event().get());
    EXPECT_TRUE(source2.get_prepare());

    expect_base_destruct(*event, expected_source);
}

TEST_F(BaseTest, MoveAssignSelf)
{
    std::unique_ptr<BaseImpl> source1 = make_base(*event, expected_source);
    set_prepare_placeholder(*source1);

    *source1 = std::move(*source1);
    EXPECT_EQ(expected_source, source1->get());
    EXPECT_EQ(expected_event, source1->get_event().get());
    EXPECT_TRUE(source1->get_prepare());

    expect_base_destruct(*event, expected_source);
}

TEST_F(BaseTest, MoveAssignEmpty)
{
    std::unique_ptr<BaseImpl> source1 = make_base(*event, expected_source);
    set_prepare_placeholder(*source1);

    std::unique_ptr<BaseImpl> source2 = make_base(*event, expected_source);
    empty_base(std::move(*source2));

    {
        void* userdata;
        EXPECT_CALL(mock,
                    sd_event_source_set_userdata(expected_source, testing::_))
            .WillOnce(DoAll(SaveArg<1>(&userdata), Return(nullptr)));
        *source2 = std::move(*source1);
        EXPECT_EQ(source2.get(), userdata);
    }
    EXPECT_EQ(nullptr, source1->get());
    EXPECT_EQ(nullptr, source1->get_event().get());
    EXPECT_FALSE(source1->get_prepare());
    EXPECT_EQ(expected_source, source2->get());
    EXPECT_EQ(expected_event, source2->get_event().get());
    EXPECT_TRUE(source2->get_prepare());

    // Make sure source1 is deleted to ensure it isn't holding a reference
    source1.reset();
    expect_base_destruct(*event, expected_source);
}

TEST_F(BaseTest, MoveAssignExisting)
{
    sd_event_source* const expected_source2 =
        reinterpret_cast<sd_event_source*>(3456);
    sd_event* const expected_event2 = reinterpret_cast<sd_event*>(4567);

    std::unique_ptr<BaseImpl> source1 = make_base(*event, expected_source);
    set_prepare_placeholder(*source1);

    UniqueEvent event2 = make_event(expected_event2);
    std::unique_ptr<BaseImpl> source2 = make_base(*event2, expected_source2);

    {
        expect_base_destruct(*event2, expected_source2);
        void* userdata;
        EXPECT_CALL(mock,
                    sd_event_source_set_userdata(expected_source, testing::_))
            .WillOnce(DoAll(SaveArg<1>(&userdata), Return(nullptr)));
        *source2 = std::move(*source1);
        EXPECT_EQ(source2.get(), userdata);
    }
    EXPECT_EQ(nullptr, source1->get());
    EXPECT_EQ(nullptr, source1->get_event().get());
    EXPECT_FALSE(source1->get_prepare());
    EXPECT_EQ(expected_source, source2->get());
    EXPECT_EQ(expected_event, source2->get_event().get());
    EXPECT_TRUE(source2->get_prepare());

    // Make sure source1 is deleted to ensure it isn't holding a reference
    source1.reset();
    expect_base_destruct(*event, expected_source);
}

class BaseMethodTest : public BaseTest
{
  protected:
    std::unique_ptr<BaseImpl> base = make_base(*event, expected_source);

    void TearDown()
    {
        expect_base_destruct(base->get_event(), base->get());
        base.reset();
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

TEST_F(BaseMethodTest, SetPrepareError)
{
    EXPECT_CALL(mock, sd_event_source_set_prepare(expected_source, testing::_))
        .WillOnce(Return(0));
    base->set_prepare(std::move([](Base&) {}));
    EXPECT_TRUE(base->get_prepare());

    Base::Callback callback = [](Base&) {};
    EXPECT_CALL(mock, sd_event_source_set_prepare(expected_source, testing::_))
        .WillOnce(Return(-EINVAL));
    EXPECT_THROW(base->set_prepare(std::move(callback)), SdEventError);
    EXPECT_FALSE(base->get_prepare());
    EXPECT_TRUE(callback);
}

TEST_F(BaseMethodTest, SetPrepareNull)
{
    EXPECT_CALL(mock, sd_event_source_set_prepare(expected_source, testing::_))
        .WillOnce(Return(0));
    base->set_prepare(std::move([](Base&) {}));
    EXPECT_TRUE(base->get_prepare());

    EXPECT_CALL(mock, sd_event_source_set_prepare(expected_source, nullptr))
        .WillOnce(Return(0));
    base->set_prepare(nullptr);
    EXPECT_FALSE(base->get_prepare());
}

TEST_F(BaseMethodTest, GetPendingSuccess)
{
    EXPECT_CALL(mock, sd_event_source_get_pending(expected_source))
        .WillOnce(Return(0));
    EXPECT_FALSE(base->get_pending());
    EXPECT_CALL(mock, sd_event_source_get_pending(expected_source))
        .WillOnce(Return(4));
    EXPECT_TRUE(base->get_pending());
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
    EXPECT_EQ(Enabled::On, base->get_enabled());
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
    base->set_enabled(Enabled::On);
}

TEST_F(BaseMethodTest, SetEnabledError)
{
    EXPECT_CALL(mock,
                sd_event_source_set_enabled(expected_source, SD_EVENT_ONESHOT))
        .WillOnce(Return(-EINVAL));
    EXPECT_THROW(base->set_enabled(Enabled::OneShot), SdEventError);
}

} // namespace
} // namespace source
} // namespace sdeventplus
