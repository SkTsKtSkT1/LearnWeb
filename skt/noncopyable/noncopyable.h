#ifndef LEARNWEB_NONCOPYABLE_H
#define LEARNWEB_NONCOPYABLE_H

namespace skt{
class Noncopyable{
public:
    Noncopyable() = default;
    ~Noncopyable() = default;
    Noncopyable(const Noncopyable&) = delete;
    Noncopyable& operator=(const Noncopyable&) =delete;
};
}

#endif
