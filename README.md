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

Config --> Yaml

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

STL偏特化，目前支持Vector
```
## 协程库封装

## socket函数库

## http协议开发

## 分布协议

## 推荐系统