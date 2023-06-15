#ifndef LEARNWEB_CONFIG_H
#define LEARNWEB_CONFIG_H

#include "memory"
#include "sstream"
#include "string"
#include "map"
#include "boost/lexical_cast.hpp" // 主要用于数值与字符串的相互转换。boost 的 lexical_cast 能把字符串转成各种 c++ 内置类型
#include "yaml-cpp/yaml.h"
#include "../log/log.h"

namespace skt{
    
class ConfigVarBase{
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;
    ConfigVarBase(const std::string& name, const std::string& description = "")
        :m_name(name)
        ,m_description(description){
            std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
        }
    virtual ~ConfigVarBase() {}

    const std::string& getName() const {return m_name;}
    const std::string& getDescription() const {return m_description;}

    virtual std::string toString() = 0;
    virtual bool fromString(const std::string& val) = 0;

protected:
    std::string m_name;
    std::string m_description;
};

template<class T>
class ConfigVar : public ConfigVarBase{
public:
    typedef std::shared_ptr<ConfigVar> ptr;
    ConfigVar(const std::string& name, 
            const T& default_value, 
            const std::string& description = "")
            :ConfigVarBase(name, description)
            ,m_val(default_value){
    }
    std::string toString() override { //override表示重载了
        try{
            return boost::lexical_cast<std::string>(m_val);
        }catch(std::exception& e){
            SKT_LOG_ERROR(SKT_LOG_ROOT()) << "ConfigVar::toString exception"
                << e.what() << "convert: " << typeid(m_val).name() << " to string";
        }
        return "";
    }

    bool fromString(const std::string& val) override {
        try{
            m_val = boost::lexical_cast<T>(val);
            return true;
        }catch(std::exception& e){
            SKT_LOG_ERROR(SKT_LOG_ROOT()) << "ConfigVar::toString exception"
                << e.what() << "convert: string to " << typeid(m_val).name();
        }
        return false;
    }

    const T getValue() const {return m_val;}
    void setValue(const T& v) {m_val =v;}
private:
    T m_val;
};

//sigleton
class Config{
public:
    typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap;

    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name,
        const T& default_value, const std::string& description = ""){
        auto tmp = Lookup<T>(name);
        if(tmp){
            SKT_LOG_INFO(SKT_LOG_ROOT()) << "Lookup name= " << name << " exist";
            return tmp;
        }

        if(name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._012345678")
                != std::string::npos){
            SKT_LOG_ERROR(SKT_LOG_ROOT()) << "Lookup name invalid " << name;
            throw std::invalid_argument(name);
        }

        typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
        s_datas[name] = v;
        return v;
    }

    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name){
        auto it = s_datas.find(name);
        if(it == s_datas.end()){
            return nullptr;
        }
        //由于放的是基类，找到了需要转换成子类
        return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
    }

    static void LoadFromYaml(const YAML::Node& root);
    static ConfigVarBase::ptr LookupBase(const std::string& name);
private:
    static ConfigVarMap s_datas;
};

}



#endif //LEARNWEB_UTIL_H