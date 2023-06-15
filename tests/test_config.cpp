#include "../skt/config/config.h"
#include "../skt/log/log.h"
#include "yaml-cpp/yaml.h"
#include "vector"
#include "string"

skt::ConfigVar<int>::ptr g_int_value_config = 
    skt::Config::Lookup("system.port", (int)8080, "system port");

skt::ConfigVar<float>::ptr g_float_value_config = 
    skt::Config::Lookup("system.value", (float)10.2f, "system port");

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

    YAML::Node root = YAML::LoadFile("/home/skt/skt/LearnWeb/bin/conf/log.yml");
    skt::Config::LoadFromYaml(root);

    SKT_LOG_INFO(SKT_LOG_ROOT()) << "after: " << g_int_value_config->getValue();
    SKT_LOG_INFO(SKT_LOG_ROOT()) << "after: " << g_float_value_config->toString();
}

int main(int argc, char** argv){
    //test_yaml();
    test_config();
    return 0;
}