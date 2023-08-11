#include "skt/env/env.h"
#include "skt/log/log.h"
#include <iostream>
static skt::Logger::ptr g_logger = SKT_LOG_ROOT();

//todo:进入main前解析参数
struct A{
    A(){
        std::ifstream ifs("/proc/" + std::to_string(getpid()) + "/cmdline", std::ios::binary);
        std::string content;
        content.resize(4096);
        ifs.read(&content[0], content.size());
        content.resize(ifs.gcount());
        for(size_t i = 0; i < content.size(); ++i){
            std::cout << i << "-" << content[i] << " - " << (int)content[i] << std::endl;
        }
        std::cout << content << std::endl;
    }
};

A a;

int main(int argc, char** argv){
    std::cout << "argc=" << argc << std::endl;
    skt::EnvMgr::GetInstance()->addHelp("s", "start with the terminal");
    skt::EnvMgr::GetInstance()->addHelp("d", "run as daemon");
    skt::EnvMgr::GetInstance()->addHelp("p", "print help");
    if(!skt::EnvMgr::GetInstance()->init(argc,argv)){
        skt::EnvMgr::GetInstance()->printHelp();
    }

    std::cout << "exe=" << skt::EnvMgr::GetInstance()->getExe() << std::endl;
    std::cout << "cwd=" << skt::EnvMgr::GetInstance()->getCwd() << std::endl;

    std::cout << "path=" << skt::EnvMgr::GetInstance()->getEnv("PATH", "XXX") << std::endl;
    std::cout << "test=" << skt::EnvMgr::GetInstance()->getEnv("test", "XXX") << std::endl;

    std::cout << "set env " << skt::EnvMgr::GetInstance()->setEnv("test", "YY") << std::endl;
    std::cout << "test=" << skt::EnvMgr::GetInstance()->getEnv("test", "XXX") << std::endl;
    if(skt::EnvMgr::GetInstance()->has("p")){
        skt::EnvMgr::GetInstance()->printHelp();
    }
    return 0;
}