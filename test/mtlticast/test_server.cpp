#include <arpa/inet.h>
#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

TEST(UDPMulticast, Server) {
  // 1. 创建通信的套接字
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd == -1) {
    perror("socket");
    exit(0);
  }

  // 设置组播属性
  struct in_addr imr_multiaddr;
  // 初始化组播地址
  inet_pton(AF_INET, "224.100.100.10", &imr_multiaddr.s_addr);
  setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, &imr_multiaddr,
             sizeof(imr_multiaddr));

  // 将数据发送给客户端, 使用广播地址和固定端口
  // 初始化客户端的地址信息
  struct sockaddr_in cliaddr;
  cliaddr.sin_family = AF_INET;
  cliaddr.sin_port = htons(20842);  // 客户端也需要绑定这端口
  inet_pton(AF_INET, "224.100.100.10", &cliaddr.sin_addr.s_addr);

  int num = 0;
  // 3. 通信
  while (1) {
    // 接收数据
    char buf[128];
    // 发送数据
    sprintf(buf, "hello, client...%d\n ", num++);
    sendto(fd, buf, strlen(buf) + 1, 0, (struct sockaddr *)&cliaddr,
           sizeof(cliaddr));
    printf("组播的数据: %s\n", buf);
    sleep(1);
  }
  close(fd);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
