#ifndef LEARNWEB_MACRO_H
#define LEARNWEB_MACRO_H

#include "string.h"
#include "assert.h"
#include "../util/util.h"

#if defined __GNUC__ || defined __llvm__
#define SKT_LIKELY(x)   __builtin_expect(!!(x), 1)
#define SKT_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define SKT_LIKELY(x)   (x)
#define SKT_UNLIKELY(x) (x)
#endif

#define SKT_ASSERT(x) \
    if(SKT_UNLIKELY(!(x))){         \
        SKT_LOG_ERROR(SKT_LOG_ROOT()) << "ASSERTION: " #x \
        << "\nbacktrace:\n" \
        << skt::BacktraceToString(100, 2, "     ");\
        assert(x);     \
    }

#define SKT_ASSERT2(x, w) \
    if(SKT_UNLIKELY(!(x))){            \
        SKT_LOG_ERROR(SKT_LOG_ROOT()) << "ASSERTION: " #x \
            << "\n" << w \
            << "\nbacktrace:\n" \
            <<skt::BacktraceToString(100, 2, "     "); \
        assert(x);\
    }

#endif
