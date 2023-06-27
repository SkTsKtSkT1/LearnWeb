#include "skt/skt.h"
#include "unistd.h"
#include "skt/log/log.h"

skt::Logger::ptr g_logger = SKT_LOG_ROOT();

int count = 0;
//skt::RWMutex s_mutex;
skt::Mutex s_mutex;
void func1(){
    SKT_LOG_INFO(g_logger) << "name: " << skt::Thread::GetName()
                            << " this.name: " << skt::Thread::GetThis()->getName()
                            << " id: " << skt::GetThreadId()
                            << " this.id: " << skt::Thread::GetThis()->getId();
    for(int i = 0; i < 100000; ++i){
        //skt::RWMutex::WriteLock lock(s_mutex); //创建加锁，析构自动解锁
        skt::Mutex::Lock lock(s_mutex);
        ++count;//这是一个写操作
    }
}

void func2(){
    while(true){
        SKT_LOG_INFO(g_logger) << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    }
}

void func3(){
    while(true){
        SKT_LOG_INFO(g_logger) << "==========================================";
    }
}

int main(){
    SKT_LOG_INFO(g_logger) << "thread test begin";
    YAML::Node root = YAML::LoadFile("/home/skt/CLionProjects/LearnWeb/bin/conf/log2.yml");
    skt::Config::LoadFromYaml(root);

    std::vector<skt::Thread::ptr> thrs;
    for(int i = 0; i < 2; ++i){
        skt::Thread::ptr thr(new skt::Thread(&func2, "name_" + std::to_string(i)));
        skt::Thread::ptr thr2(new skt::Thread(&func3, "name_" + std::to_string(i * 2 + 1)));
        thrs.push_back(thr);
        thrs.push_back(thr2);
    }

    for(size_t i = 0; i < thrs.size() ; ++i){
        thrs[i]->join();
    }

    SKT_LOG_INFO(g_logger) << "thread test end";
    SKT_LOG_INFO(g_logger) << "count=" << count;

    return 0;
}