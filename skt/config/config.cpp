#include "config.h"
#include "list"
#include "skt/env/env.h"
#include "skt/util/util.h"
#include "skt/log/log.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctime>

namespace skt{

static skt::Logger::ptr g_logger = SKT_LOG_NAME("system");

//Config::ConfigVarMap Config::s_datas;  //static 成员变量的内存既不是在声明类时分配，也不是在创建对象时分配，而是在（类外）初始化时分配。反过来说，没有在类外初始化的 static 成员变量不能使用。
ConfigVarBase::ptr Config::LookupBase(const std::string& name){
    RWMutexType::ReadLock lock(GetMutex());
    auto it = GetDatas().find(name);
    return it == GetDatas().end() ? nullptr : it->second; //s_datas的指针是ConfigVar类型，因此可以调用子类的方法。
}
//"A.B", 10
//A:
//  B: 10
//  C: str

static void ListAllMember(const std::string& prefix, const YAML::Node& node, std::list<std::pair<std::string, const YAML::Node>>& output){
    if(prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyz._012345678") != std::string::npos){
        SKT_LOG_ERROR(SKT_LOG_ROOT()) << "Config invalid name: " << prefix << " : " << node;
        return ;
    }
    output.push_back(std::make_pair(prefix, node));
    if(node.IsMap()){
        for(auto it = node.begin(); it != node.end(); ++it){
            ListAllMember(prefix.empty() ? it->first.Scalar() : prefix + "." + it->first.Scalar(), it->second, output);
        }
    }

}

void Config::LoadFromYaml(const YAML::Node& root){
    std::list<std::pair<std::string, const YAML::Node>> all_nodes;
    ListAllMember("", root, all_nodes);
    for(auto& i : all_nodes){
        std::string key = i.first;
        if(key.empty()){
            continue;
        }

        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        ConfigVarBase::ptr var = LookupBase(key);

        if(var){
            if(i.second.IsScalar()){
                var->fromString(i.second.Scalar());
            }else{
                std::stringstream ss;
                ss << i.second;
                var->fromString(ss.str());
            }
        }
    }
}

//可以用md5来进一步做
static std::map<std::string, uint64_t> s_file2modifytime;
static skt::Mutex s_mutex;

void Config::LoadFromConfDir(const std::string &path) {
    std::string absolute_path = skt::EnvMgr::GetInstance()->getAbsolutePath(path);
    std::vector<std::string> files;
    FSUtil::ListAllFile(files, absolute_path, ".yml");
    for(auto& i : files){
        {
            struct stat st{};
            lstat(i.c_str(), &st);
            skt::Mutex::Lock lock(s_mutex);
            if(s_file2modifytime[i] == (uint64_t)st.st_mtim.tv_sec){
                continue;
            }
            s_file2modifytime[i] = (uint64_t)st.st_mtim.tv_sec;
        }
        try {
            YAML::Node root = YAML::LoadFile(i);
            LoadFromYaml(root);
            SKT_LOG_INFO(g_logger) << "LoadConfFile file=" << i << " ok";
        }catch(...){
            SKT_LOG_ERROR(g_logger) << "LoadConfFile file=" << i << " failed";
        }
    }
}

void Config::Visit(std::function<void(ConfigVarBase::ptr)> cb){
    RWMutexType::ReadLock lock(GetMutex());
    ConfigVarMap& m = GetDatas();
    for(auto it = m.begin(); it != m.end(); ++it){
        cb(it->second);
    }
}
}