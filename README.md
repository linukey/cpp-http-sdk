# WebServer

#### 一个极其简单、轻量级的 http-server，使用 c++ Boost.Asio 网络库开发。使用tcp协议封装实现http协议。
#### 你可以使用此http-server，通过继承WebServer类，在router方法里面实现自己的业务端路由，来快速搭建一个本地可用的http-server，和外界进行http协议通信，而不必关心http协议请求和响应的具体实现，通过查看example快速了解。

#### use:
#### 1. cd WebServer
#### 2. sh build.sh 
#### 3. cd own_business
#### 4. 在 own_business 目录中的main.cpp里面，实现自己的业务逻辑

#### 开发环境:
#### 1. centos 6
#### 2. gcc version 4.8.2 (GCC)
#### 3. boost 1.6
