# LearnWeb
![last-commit](https://img.shields.io/github/last-commit/SkTsKtSkT1/LearnWeb)&emsp;![actions](https://img.shields.io/github/actions/workflow/status/SkTsKtSkT1/LearnWeb/actions.yml)&emsp;![codesize](https://img.shields.io/github/languages/code-size/SkTsKtSkT1/LearnWeb)&emsp;![codepercent](https://img.shields.io/github/languages/top/SkTsKtSkT1/LearnWeb)

https://www.bilibili.com/video/BV184411s7qF
## 开发环境
Ubuntu 20.04 LTS

gcc 0.4.0

cmake

## 项目路径
bin -- 二进制输出

build -- 中间文件路径

cmake -- cmake函数文件夹

CMakeLists.txt -- cmake的定义文件

lib -- 库的输出文件

Makefile 

skt -- 源码路径

test -- 测试代码路径

## 日志系统
1）
    Log4J
    
    Logger(定义日志类别)
        |
        |------Formatter(日志格式)
        |
    Appender(日志输出地方)

## 配置系统

Config --> Yaml（https://www.runoob.com/w3cnote/yaml-intro.html）

- Boost required (sudo apt install libboost-all-dev -y)

yaml-cpp  github
mkdir build && cd build && camke .. && sudo make install

```cpp
YAML::Node node = YAML::LoadFile(filename)
node.IsMap()
for(auto it = node.begin(); it != onde.end(); ++it){
}

node.IsSequence()
for(size_t i = 0; i < node.size(); ++i){

}

node.IsScalar()
```

配置系统原则： 约定优先于配置
```cpp
template<T, FromStr, ToStr>
class ConfigVar;

template<F, T>
LexicalCast;

//STL偏特化，目前支持Vector, list, set, map, unordered_map, unordered_set
//map/unordered_map 支持 key = std::string
// Config::Lookup(key), key一样，类型不同，不会报错，need 处理。
```

自定义类型，需要实现skt::LexicalCast的偏特化，实现之后就可以支持Config解析自定义类型，且自定义类型可以与常规stl容器一起使用。

配置的事件机制：当一个配置项发生修改的时候，可以反向通知相应的代码，回调。

# 日志系统整合配置系统
```yaml
logs:
    - name: root
      level: (debug, info, warn, error, fatal)
      formatter: '%d%T%p%T%t%m%N'
      appender: 
        - type: (StdoutLogAppender, FileLogAppender)
          level: (debug,...) #appender's level
          file: /logs/xxx.log
```

```cpp
  skt::Logger g_logger = 
  skt::LoggerMgr::GetInstance()->getLogger(name);
  SKT_LOG_INFO(g_logger) << "xxx log";
```

```cpp
static Logger::ptr g_log = SKT_LOG_NAME("system");//prev:m_root, cur:m_system->m_root，当logger的appenders为空，使用root写logger
```

```cpp
//定义LogDefine and LogAppenderDefine， 偏特化LexicalCast，
//实现日志配置解析
```

## 协程库封装

## socket函数库

## http协议开发

## 分布协议

## 推荐系统