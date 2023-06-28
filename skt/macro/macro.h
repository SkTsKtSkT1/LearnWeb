#ifndef LEARNWEB_MACRO_H
#define LEARNWEB_MACRO_H

#include "string.h"
#include "assert.h"
#include "../util/util.h"

#define SKT_ASSERT(x) \
    if(!(x)){         \
        SKT_LOG_ERROR(SKT_LOG_ROOT()) << "ASSERTION: " #x \
        << "\nbacktrace:\n" \
        << skt::BacktraceToString(100, 2, "     ");\
        assert(x);     \
    }

#define SKT_ASSERT2(x, w) \
    if(!(x)){            \
        SKT_LOG_ERROR(SKT_LOG_ROOT()) << "ASSERTION: " #x \
            << "\n" << w \
            << "\nbacktrace:\n" \
            <<skt::BacktraceToString(100, 2, "     "); \
        assert(x);\
    }

#endif
