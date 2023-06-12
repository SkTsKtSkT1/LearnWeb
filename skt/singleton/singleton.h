#ifndef LEARNWEB_SINGLETONG_H
#define LEARNWEB_SINGLETONG_H
#include "memory"
//单例模式的封装
//不在Log中实现单例模式，是因为分布式多线程等 可能是一个线程一个单例
namespace skt{
template<class T, class X = void, int N = 0> //N 实例个数
class Singleton{
public:
    static T* GetInstance(){
        static T v;
        return &v;
    }
};

template<class T, class X = void, int N = 0>
class SingletonPtr{
public:
    static std::shared_ptr<T> GetInstance(){
        static std::shared_ptr<T> v(new T);
        return v;
    }
};

}


#endif //LEARNWEB_SINGLETONG_H