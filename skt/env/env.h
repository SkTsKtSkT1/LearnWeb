#ifndef LEARNWEB_ENV_H
#define LEARNWEB_ENV_H


#include <vector>
#include <map>
#include "skt/singleton/singleton.h"
#include "skt/thread/thread.h"
#include <stdlib.h>

namespace skt{ 

class Env{
public:
    typedef RWMutex RWMutexType;
    bool init(int argc, char** argv);

    void add(const std::string& key, const std::string& val);
    bool has(const std::string& key);
    void del(const std::string& key);
    std::string get(const std::string& key, const std::string& default_value = "");
    void addHelp(const std::string& key, const std::string& desc);
    void removeHelp(const std::string& key);
    void printHelp();

    const std::string& getExe() const { return m_exe;}
    const std::string& getCwd() const { return m_cwd;}

    std::string getEnv(const std::string& key, const std::string& default_value = "");
    bool setEnv(const std::string& key, const std::string& val);

    std::string getAbsolutePath(const std::string& path) const;
private:
    RWMutexType m_mutex;
    std::map<std::string, std::string> m_args;
    std::vector<std::pair<std::string, std::string>> m_helps;

    std::string m_program;
    std::string m_exe;
    std::string m_cwd;
};

typedef skt::Singleton<Env> EnvMgr;

}
#endif
