#pragma once
#include <sdeventplus/exception.hpp>
#include <stdplus/util/cexec.hpp>

#define SDEVENTPLUS_CHECK(msg, expr)                                           \
    CHECK_RET(expr, [&](int ret) { throw SdEventError(ret, (msg)); })
