#include "skt/uri/uri.h"
#include "iostream"

int main(int argc, char** argv){
    skt::Uri::ptr uri = skt::Uri::Create("http://www.sylar.top/test/uri?id=100&name=sylar#frg");
    std::cout << uri->toString() << '\n';
    auto addr = uri->createAddress();
    std::cout << *addr << '\n';
    return 0;
}
