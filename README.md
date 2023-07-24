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
mkdir build && cd build && cmake .. && sudo make install

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

## 日志系统整合配置系统
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
  skt::Logger g_logger = skt::LoggerMgr::GetInstance()->getLogger(name);
  SKT_LOG_INFO(g_logger) << "xxx log";
```

```cpp
static Logger::ptr g_log = SKT_LOG_NAME("system");//prev:m_root, cur:m_system->m_root，当logger的appenders为空，使用root写logger
```

```cpp
//定义LogDefine and LogAppenderDefine， 偏特化LexicalCast，
//实现日志配置解析
```

```cpp
```

遗留问题：
1. -[x] appender定义的formatter读取yaml时，没有被初始化
2. -[X] 去掉额外的调试日志
3. -[X] 文件名的问题(输出相对路径) redefine cmake的__FILE__

P20 for review the log and config.

# 线程库

Thread，Mutex, Pthread

pthread pthread_create

互斥量 mutex 

信号量 semaphore

和log来整合, Logger, Appender,

Spinlock 替换 Mutex, 写文件，周期性的reopen()，防止文件被删除感知不到文件变化造成的问题（磁盘上被占用但是无法记录下来）

Config, RWMutex

## 协程库封装
协程是用户态，线程是系统切换。

定义协程接口  ucontext_t macro
```
Fiber::GetThis() -> main_fiber
Thread -> main_fiber <-------> sub_fiber
              ^
              |
              v
            sub_fiber
```

协程调度模块Schedule
```
       1  -  N     1   -  M
schedule --> thread ----> fiber
1.线程池，分配一组线程
2.协程调度器，将协程指定到相应的线程上去执行
3.

N : M

m_threads 
<function<void()>, fiber, thread> m_fibers

schedule(func/fiber)

start()
stop()
run()

1.设置当前线程的scheduler
2.设置当前 线程的run、fiber
3.协程调度循环while(true)
    1.协程消息队列是否有任务
    2.无任务执行，执行idle
```

``` 
IOManager(epoll) ---> Schedule
    |
    |
    v
   idle(epoll_wait)
   
   信号量
PutMessage(msg,) 信号量+1, single()

message_queue
   |
   | ---- Thread
   | ---- Thread
         wait() -> 信号量-1 RecvMessage(msg,)
         
异步IO，等待数据返回。 epoll_wait

epoll_create epoll_ctl epoll_wait
```

```
Timer -> addTimer() --> cancel()

获取当前的定时器触发离现在的时间差

返回当前需要触发的定时器.etc
```

```
                [Fiber]                 [Timer]
                   ^ N                     ^
                   |                       |
                   | 1                     |
                [Thread]             [TimerManager]
                   ^ M                     ^
                   |                       |
                   | 1                     |
                [Scheduler] <---- [IOManager(epoll)]
```

## Hook
sleep

usleep

socket 相关(socket, connect, accept)

io相关(read, write, send, recv, ...)

fd相关(fnctl, ioctl, ...)

## socket函数库
               [UnixAddress]
                    |
                -------                     |-[IPV4Address]
                |Adress| --- [IPAddress] ---|
                -------                     |-[IPV6Address]
                    |
                    |
               ----------
               | Socket |
               ----------
                
## http协议开发

## 分布协议

## 推荐系统