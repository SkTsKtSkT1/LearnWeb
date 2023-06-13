#ifndef LEARNWEB_CONFIG_H
#define LEARNWEB_CONFIG_H

#include "memory"
#include "sstream"
#include "string"
#include "boost/lexical_cast.hpp" // 主要用于数值与字符串的相互转换。boost 的 lexical_cast 能把字符串转成各种 c++ 内置类型

namespace skt{
    
class ConfigVarBase{
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;
    ConfigVarBase(const std::string& name, const std::string& description = "")
        :m_name(name)
        ,m_description(description){
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

}



#endif //LEARNWEB_UTIL_H