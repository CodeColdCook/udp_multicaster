# 基于sys/socket的Udp广播与组播

## 参考链接

[UDP（udp通信、广播、组播）](https://blog.csdn.net/qq_35883464/article/details/103741461)

## 广播

### 通信流程

#### 服务器端 

服务器端 -> 广播的一端: 

- 创建通信的套接字
              int fd = socket( af_inet, SOCK_DGRAM, 0);

- 设置udp广播属性
                  setsockopt();

- 通信 -> 发送广播数据
                struct sockaddr_in cliaddr;
                cliaddr.sin_port(8888);    // 广播数据发送到客户端的8888端口, 客户端需要绑定该端口
                cliaddr.sin_addr.s_addr -> 初始化一个广播地址
               发送数据: sendto(fd, buf, len, 0, &cliaddr, len);

- 关闭通信的fd
              close(fd);


#### 客户端

客户端

- 创建一个通信的套接字
                  int fd = socket( af_inet, SOCK_DGRAM, 0);

- 如果想接收广播数据, 需要绑定以固定端口(服务器广播数据使用的端口)
                struct sockaddr_in cliaddr;
                cliaddr.sin_port(8888);    
                bind(fd, cliaddr, len);

- 通信
              接收数据: recvfrom

- 关闭通信的文件描述符
              close();


#### 设置广播属性函数：setsockopt

```c++
#include <arpa/inet.h>
int setsockopt(int sockfd, int level, int optname,const void *optval, socklen_t optlen);
  	参数:
  		- sockfd: 文件描述符
  		- level: SOL_SOCKET
  		- optname: SO_BROADCAST
  		- optval: int数值为1, 允许广播
  		- optlen: optval内存大小
```

### 广播代码

[broadcast](./test/broadcast)

## 组播

广播：无论连接到局域网的客户端想不想接收该数据，Server都会给客户端发送该数据。

进而造成客户端上数据的拥塞，因此引出了组播：Server可以将数据包只发送给指定组内的客户端，而不发送给指定组外的客户端。


特点：

可以在internet中进行组播
加入到广播的组织中才可以收到数据

### 组播地址

***\*IP 组播通信必须依赖于 IP 组播地址\****，在 IPv4 中它的范围从  `224.0.0.0` 到 `239.255.255.255`，并被划分为***\*局部链接多播地址、预留多播地址\****和***\*管理权限多播地址\****三类:

组播组可以是永久的也可以是临时的。组播组地址中，有一部分由官方分配的，称为永久组播组。永久组播组保持不变的是它的ip地址，组中的成员构成可以发生变化。永久组播组中成员的数量都可以是任意的，甚至可以为零。那些没有保留下来供永久组播组使用的ip组播地址，可以被临时组播组利用。

- 224.0.0.0～224.0.0.255为预留的组播地址（永久组地址），地址224.0.0.0保留不做分配，其它地址供路由协议使用；
- 224.0.1.0～224.0.1.255是公用组播地址，可以用于Internet；
- 224.0.2.0～238.255.255.255为用户可用的组播地址（临时组地址），全网范围内有效；
- 239.0.0.0～239.255.255.255为本地管理组播地址，仅在特定的本地范围内有效。

### 组播通信流程

#### 服务器端

- 1.创建通信的套接字
              int fd = socket( af_inet, SOCK_DGRAM, 0);
          - 设置udp组播属性
                  setsockopt();

- 2.通信 -> 发送组播数据
                struct sockaddr_in cliaddr;
                cliaddr.sin_port(8888);    // 广播数据发送到客户端的8888端口, 客户端需要绑定该端口
                cliaddr.sin_addr.s_addr -> 初始化一个组播地址
              发送数据: sendto(fd, buf, len, 0, &cliaddr, len);

- 3.关闭通信的fd
              close(fd);

#### 客户端

- 1.创建一个通信的套接字
                  int fd = socket( af_inet, SOCK_DGRAM, 0);

- 2.如果想接收组播数据, 需要绑定以固定端口(服务器组播数据使用的端口)
                  struct sockaddr_in cliaddr;
                cliaddr.sin_port(8888);    
                  bind(fd, cliaddr, len);

- 3.客户端加入到组播网络中
                  setsockopt():

- 4.通信
              接收数据: recvfrom

- 5.关闭通信的文件描述符
              close();

#### 设置组播属性函数：setsockopt

```c++
  int setsockopt(int sockfd, int level, int optname,const void *optval, socklen_t optlen);
  
  // 服务器端 -> 进程组播
  	参数: 
  		- sockfd: 通信的文件描述符
  		- level: IPPROTO_IP
  		- optname: IP_MULTICAST_IF
  		- optval: struct in_addr
          - optlen: optval 的内存大小
  // 客户端 -> 加入到多播组
  	参数:
  		- sockfd: 通信的文件描述符
  		- level: IPPROTO_IP
  		- optname: IP_ADD_MEMBERSHIP
  		- optval: struct ip_mreqn
         - optlen: optval 的内存大小
          
  struct ip_mreqn
  {
  	// 组播组的IP地址.
  	struct in_addr imr_multiaddr; 
  	// 本地某一网络设备接口的IP地址。
  	struct in_addr imr_address;   
  	int   imr_ifindex;   // 网卡编号
  };
```

### 组播代码

[mtlticast](./test/mtlticast)

## 功能封装

udp_multicaster

### 依赖

- GTest

### 编译

```shell
# git@github.com:CodeColdCook/udp_multicaster.git
cd build
cmake .. 
make 
```

### 运行

```shell
# multicaster
cd build
./mtlticast_test_multicaster

# client
./mtlticast_test_client

# server
./mtlticast_test_server
```

