#include "../skt/config/config.h"
#include "../skt/log/log.h"
#include "yaml-cpp/yaml.h"
#include "vector"
#include "string"

skt::ConfigVar<int>::ptr g_int_value_config = 
    skt::Config::Lookup("system.port", (int)8080, "system port");
    
skt::ConfigVar<float>::ptr g_int_valuex_config = 
    skt::Config::Lookup("system.port", (float)8080, "system port");

skt::ConfigVar<float>::ptr g_float_value_config = 
    skt::Config::Lookup("system.value", (float)10.2f, "system value");

skt::ConfigVar<std::vector<int>>::ptr g_int_vec_value_config = 
    skt::Config::Lookup("system.int_vec", std::vector<int>{1,2}, "system int vec");

skt::ConfigVar<std::list<int>>::ptr g_int_list_value_config = 
    skt::Config::Lookup("system.int_list", std::list<int>{1,2}, "system int list");

skt::ConfigVar<std::set<int>>::ptr g_int_set_value_config = 
    skt::Config::Lookup("system.int_set", std::set<int>{1,2}, "system int set");

skt::ConfigVar<std::unordered_set<int>>::ptr g_int_uset_value_config = 
    skt::Config::Lookup("system.int_uset", std::unordered_set<int>{1,2}, "system int uset");

skt::ConfigVar<std::map<std::string, int>>::ptr g_str_int_map_value_config = 
    skt::Config::Lookup("system.str_int_map", std::map<std::string, int>{{"k",2}}, "system str int map");

skt::ConfigVar<std::unordered_map<std::string, int>>::ptr g_str_int_umap_value_config = 
    skt::Config::Lookup("system.str_int_umap", std::unordered_map<std::string, int>{{"p",2}}, "system str int umap");

void print_yaml(const YAML::Node& node, int level){
    if(node.IsScalar()){
        SKT_LOG_INFO(SKT_LOG_ROOT()) << std::string(level * 4, ' ') << node.Scalar() << " - " << node.Tag() << " - " << level;
    }else if(node.IsNull()){
        SKT_LOG_INFO(SKT_LOG_ROOT()) << std::string(level * 4, ' ') << "NULL - " << " - " << node.Tag() << " - " << level;
    }else if(node.IsMap()){
        for(auto it = node.begin(); it != node.end(); ++it){
            SKT_LOG_INFO(SKT_LOG_ROOT()) << std::string(level * 4, ' ') <<  it->first << " - " << it->second.Tag() << " - " << level;
            print_yaml(it->second, level + 1);
        }
    }else if(node.IsSequence()){
        for(size_t i = 0; i < node.size(); ++i){
            SKT_LOG_INFO(SKT_LOG_ROOT()) << std::string(level * 4, ' ') <<  i << " - " << node[i].Tag() << " - " << level;
            print_yaml(node[i], level + 1);
        }
    }
}


void test_yaml(){
    YAML::Node root = YAML::LoadFile("/home/skt/skt/LearnWeb/bin/conf/log.yml");
    print_yaml(root, 0);
    SKT_LOG_INFO(SKT_LOG_ROOT()) << root.Scalar();
}

void test_config(){
    SKT_LOG_INFO(SKT_LOG_ROOT()) << "before: " << g_int_value_config->getValue();
    SKT_LOG_INFO(SKT_LOG_ROOT()) << "before: " << g_float_value_config->toString();
    // auto v = g_int_vec_value_config->getValue();
    // for(auto& i : v){
    //     SKT_LOG_INFO(SKT_LOG_ROOT()) << "before int_vec: " << i;
    // }
#define XX(g_var, name, prefix) \
    { \
        auto& v = g_var->getValue(); \
        for(auto& i : v){ \
            SKT_LOG_INFO(SKT_LOG_ROOT()) << #prefix " " #name ": " << i; \
        } \
        SKT_LOG_INFO(SKT_LOG_ROOT()) << #prefix " " #name " yaml: " << g_var->toString(); \
    }

#define XX_MAP(g_var, name, prefix) \
    { \
        auto& v = g_var->getValue(); \
        for(auto& i : v){ \
            SKT_LOG_INFO(SKT_LOG_ROOT()) << #prefix " " #name ": { " << i.first << " - " << i.second << "}"; \
        } \
        SKT_LOG_INFO(SKT_LOG_ROOT()) << #prefix " " #name " yaml: " << g_var->toString(); \
    }

    XX(g_int_vec_value_config, int_vec, before);
    XX(g_int_list_value_config, int_list, before);
    XX(g_int_set_value_config, int_set, before);
    XX(g_int_uset_value_config, int_uset, before);
    XX_MAP(g_str_int_map_value_config, str_int_map, before);
    XX_MAP(g_str_int_umap_value_config, str_int_umap, before);

    YAML::Node root = YAML::LoadFile("/home/skt/skt/LearnWeb/bin/conf/log.yml");
    skt::Config::LoadFromYaml(root);

    SKT_LOG_INFO(SKT_LOG_ROOT()) << "after: " << g_int_value_config->getValue();
    SKT_LOG_INFO(SKT_LOG_ROOT()) << "after: " << g_float_value_config->toString();
    

    // v = g_int_vec_value_config->getValue();
    // for(auto& i : v){
    //     SKT_LOG_INFO(SKT_LOG_ROOT()) << "after int_vec: " << i;
    // }
    XX(g_int_vec_value_config, int_vec, after);
    XX(g_int_list_value_config, int_list, after);
    XX(g_int_set_value_config, int_set, after);
    XX(g_int_uset_value_config, int_uset, after);
    XX_MAP(g_str_int_map_value_config, str_int_map, after);
    XX_MAP(g_str_int_umap_value_config, str_int_umap, after);

}

class Person{
public:
    Person() {};
    std::string m_name;
    int m_age = 0;
    bool m_sex = 0;
    std::string toString() const {
        std::stringstream ss;
        ss<<"[Person name =" << m_name << " age = " << m_age << " sex = " << m_sex << "]";
        return ss.str();
    }
};

namespace skt{

template<>
class LexicalCast<std::string, Person> {
public:
    Person operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        Person p;
        p.m_name = node["name"].as<std::string>();
        p.m_age = node["age"].as<int>();
        p.m_sex = node["sex"].as<bool>();
        return p;
    }
};

template<>
class LexicalCast<Person, std::string> {
public:
    std::string operator()(const Person& p) {
        YAML::Node node;
        node["name"] = p.m_name;
        node["age"] = p.m_age;
        node["sex"] = p.m_sex;
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

}



skt::ConfigVar<Person>::ptr g_person = 
    skt::Config::Lookup("class.person", Person(), "system person");

void test_class(){
    SKT_LOG_INFO(SKT_LOG_ROOT()) << "before: " << g_person->getValue().toString() << " - " << g_person->toString();
    
    YAML::Node root = YAML::LoadFile("/home/skt/skt/LearnWeb/bin/conf/test.yml");
    skt::Config::LoadFromYaml(root);

    SKT_LOG_INFO(SKT_LOG_ROOT()) << "after: " << g_person->getValue().toString() << " - " << g_person->toString();
}   

int main(int argc, char** argv){
    //test_yaml();
    //test_config();
    test_class();
    return 0;
}