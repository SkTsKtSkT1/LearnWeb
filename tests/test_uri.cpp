#include "skt/uri/uri.h"
#include "iostream"

int main(int argc, char** argv){
    //skt::Uri::ptr uri = skt::Uri::Create("http://www.sylar.top/test/uri?id=100&name=sylar#frg");
    skt::Uri::ptr uri = skt::Uri::Create("http://www.sylar.top/test/中文/uri?id=100&name=sylar#frg中文");
    std::cout << uri->toString() << std::endl;
    auto addr = uri->createAddress();
    std::cout << *addr << std::endl;
    return 0;
}
