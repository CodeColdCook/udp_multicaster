#include <arpa/inet.h>
#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

TEST(UdpBroadcast, Client) {
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

  // 3. 通信
  while (1) {
    // 接收数据
    char buf[128];
    recvfrom(fd, buf, sizeof(buf), 0, NULL, NULL);
    printf("server say: %s\n", buf);
  }

  close(fd);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
