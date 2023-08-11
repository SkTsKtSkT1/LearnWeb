#include "skt/application/application.h"

#include "skt/config/config.h"
#include "skt/log/log.h"
#include "skt/env//env.h"
#include "skt/daemon/daemon.h"
#include <unistd.h>

namespace skt{

static skt::Logger::ptr g_logger = SKT_LOG_NAME("system");

Application* Application::s_instance = nullptr;

static skt::ConfigVar<std::string>::ptr g_server_work_path =
        skt::Config::Lookup("server.work_path"
                ,std::string("/home/skt/CLionProjects/LearnWeb")
                , "server work path");

static skt::ConfigVar<std::string>::ptr g_server_pid_file =
        skt::Config::Lookup("server.pid_file"
                ,std::string("skt.pid")
                , "server pid file");

struct HttpServerConf{
    std::vector<std::string> address;
    int keepalive = 0;
    int timeout = 1000 * 2 * 60;
    std::string name;

    bool isValie() const{
        return !address.empty();
    }

    bool operator==(const HttpServerConf& oth) const{
        return address == oth.address && keepalive == oth.keepalive
            && timeout == oth.timeout && name == oth.name;
    }
};

template<>
class LexicalCast<std::string, HttpServerConf>{
public:
    HttpServerConf operator()(const std::string &v){
        YAML::Node node = YAML::Load(v);
        HttpServerConf conf;
        conf.keepalive = node["keepalive"].as<int>(conf.keepalive);
        conf.timeout = node["timeout"].as<int>(conf.timeout);
        conf.name = node["name"].as<std::string>(conf.name);
        if(node["address"].IsDefined()){
            for(size_t i = 0; i < node["address"].size(); ++i){
                conf.address.push_back(node["address"][i].as<std::string>());
            }
        }
        return conf;
    }
};

template<>
class LexicalCast<HttpServerConf, std::string>{
public:
    std::string operator()(const HttpServerConf& conf){
        YAML::Node node;
        node["name"] = conf.name;
        node["keepalive"] = conf.keepalive;
        node["timeout"] = conf.timeout;
        for(auto& i : conf.address){
            node["address"].push_back(i);
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

static skt::ConfigVar<std::vector<HttpServerConf>>::ptr g_http_servers_conf =
        skt::Config::Lookup("http_servers", std::vector<HttpServerConf>(), "http server config");

Application::Application() {
    s_instance = this;
}

bool Application::init(int argc, char **argv) {
    m_argc = argc;
    m_argv = argv;

    skt::EnvMgr::GetInstance()->addHelp("s", "start with the terminal");
    skt::EnvMgr::GetInstance()->addHelp("d", "run as daemon");
    skt::EnvMgr::GetInstance()->addHelp("c", "conf path default: ./conf");
    skt::EnvMgr::GetInstance()->addHelp("p", "print help");

    bool is_print_help = false;
    //没有初始化成功
    if(!skt::EnvMgr::GetInstance()->init(argc, argv)){
        is_print_help = true;
    }
    if(skt::EnvMgr::GetInstance()->has("p")){
        is_print_help = true;
    }

    std::string conf_path = skt::EnvMgr::GetInstance()->getAbsolutePath(
            skt::EnvMgr::GetInstance()->get("c", "conf")
            );
    SKT_LOG_INFO(g_logger) << "load conf path:" << conf_path;
    skt::Config::LoadFromConfDir(conf_path);

    if(is_print_help) {
        skt::EnvMgr::GetInstance()->printHelp();
        return false;
    }

    int run_type = 0;
    if(skt::EnvMgr::GetInstance()->has("s")){
        run_type = 1;
    }
    if(skt::EnvMgr::GetInstance()->has("d")){
        run_type = 2;
    }
    if(run_type == 0){
        skt::EnvMgr::GetInstance()->printHelp();
        return false;
    }

    std::string pid_file = g_server_work_path->getValue() + "/" + g_server_pid_file->getValue();
    if(skt::FSUtil::IsRunningPidfile(pid_file)){
        SKT_LOG_ERROR(g_logger) << "server is running:" << pid_file;
        return false;
    }

    if(!skt::FSUtil::Mkdir(g_server_work_path->getValue())){
        SKT_LOG_FATAL(g_logger) << "create work path [" << g_server_work_path->getValue()
                                  << " errno=" << errno << " errstr=" << strerror(errno);
        return false;
    }
    return true;
}




bool Application::run() {
    bool is_daemon = skt::EnvMgr::GetInstance()->has("d");
    return start_daemon(m_argc, m_argv, std::bind(&Application::main, this, std::placeholders::_1, std::placeholders::_2), is_daemon);
}


bool Application::getServer(const std::string &type, std::vector<TcpServer::ptr> &svrs) {
    auto it = m_servers.find(type);
    if(it == m_servers.end()){
        return false;
    }
    svrs = it->second;
    return true;
}

void Application::listAllServer(std::map<std::string, std::vector<TcpServer::ptr>> &servers) {
    servers = m_servers;
}


int Application::main(int argc, char **argv) {
    SKT_LOG_INFO(g_logger) << "main";
    std::string pid_file = g_server_work_path->getValue() + "/" + g_server_pid_file->getValue();
    std::ofstream ofs(pid_file);
    if(!ofs){
        SKT_LOG_ERROR(g_logger) << "open pidfile " << pid_file << " failed";
        return false;
    }
    ofs << getpid();

    auto http_confs = g_http_servers_conf->getValue();
    for(auto& i : http_confs){
        SKT_LOG_INFO(g_logger) << LexicalCast<HttpServerConf, std::string>()(i);
    }

    skt::IOManager iom(1);
    iom.schedule(std::bind(&Application::run_fiber, this));
    iom.stop();
    return 0;
}

int Application::run_fiber() {
    auto http_confs = g_http_servers_conf->getValue();
    for(auto& i : http_confs){
        SKT_LOG_INFO(g_logger) << LexicalCast<HttpServerConf, std::string>()(i);
        std::vector<skt::Address::ptr> address;
        for(auto& a : i.address) {
            size_t pos = a.find(":");
            if (pos == std::string::npos) {
                SKT_LOG_ERROR(g_logger) << "invalid address: " << a;
                continue;
            }
            auto addr = skt::Address::LookupAny(a);
            if (addr) {
                address.push_back(addr);
                continue;
            }
            std::vector<std::pair<Address::ptr, uint32_t>> result;
            if (!skt::Address::GetInterfaceAddresses(result, a.substr(0, pos))) {
                SKT_LOG_ERROR(g_logger) << "invalid address: " << a;
                continue;
            }
            for (auto &x: result) {
                auto ipaddr = std::dynamic_pointer_cast<IPAddress>(x.first);
                if (ipaddr) {
                    ipaddr->setPort(atoi(a.substr(pos + 1).c_str()));
                }
                address.push_back(ipaddr);
            }
        }
        skt::http::HttpServer::ptr server(new skt::http::HttpServer(i.keepalive));
        std::vector<Address::ptr> fails;
        if(!server->bind(address, fails)) {
            for (auto &x: fails) {
                SKT_LOG_ERROR(g_logger) << "bind address fail:" << *x;
            }
            _exit(0);
        }
        if(!i.name.empty()){
            server->setName(i.name);
        }
        server->start();
        m_httpservers.push_back(server);
    }
    return 0;
}

}