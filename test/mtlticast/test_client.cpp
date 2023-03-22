#include <arpa/inet.h>
#include <gtest/gtest.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

TEST(UDPMulticast, Client) {
  // 1. 创建通信的套接字
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd == -1) {
    perror("socket");
    exit(0);
  }

  // 2. 通信的fd绑定本地的IP和端口
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(20842);
  addr.sin_addr.s_addr = INADDR_ANY;
  int ret = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
  if (ret == -1) {
    perror("bind");
    exit(0);
  }
  // inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr.s_addr);
  //  加入到组播
  struct ip_mreqn op;
  op.imr_address.s_addr = INADDR_ANY;  // 本地地址
  inet_pton(AF_INET, "224.100.100.10", &op.imr_multiaddr.s_addr);
  op.imr_ifindex = if_nametoindex("ens33");

  setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &op, sizeof(op));
  // 3. 通信
  while (1) {
    // 接收数据
    char buf[128];
    int re_size = recvfrom(fd, buf, sizeof(buf), 0, NULL, NULL);
    printf("server say: %s\n", buf);
    printf("re_size: %d\n", re_size);
  }
  close(fd);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
