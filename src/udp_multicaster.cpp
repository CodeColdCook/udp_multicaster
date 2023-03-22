#include "udp_multicaster.h"

UdpMulticaster::UdpMulticaster(UdpMulticastConfig config)
    : config_(config), receiver_running_flag_(false) {
  // init server, send msg to multicast
  fd_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd_ == -1) {
    perror("socket");
    return;
  }

  // 设置组播属性
  struct in_addr imr_multiaddr;
  // 初始化组播地址
  inet_pton(AF_INET, config_.multi_addr.c_str(), &imr_multiaddr.s_addr);
  setsockopt(fd_, IPPROTO_IP, IP_MULTICAST_IF, &imr_multiaddr,
             sizeof(imr_multiaddr));

  // 将数据发送给客户端, 使用广播地址和固定端口
  // 初始化客户端的地址信息
  cliaddr_.sin_family = AF_INET;
  cliaddr_.sin_port = htons(config_.multi_port);  // 客户端也需要绑定这端口
  inet_pton(AF_INET, config_.multi_addr.c_str(), &cliaddr_.sin_addr.s_addr);

  // init client, receive msg from multicast
  fd_client_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd_client_ == -1) {
    perror("socket");
    return;
  }
  recv_addr_.sin_family = AF_INET;
  recv_addr_.sin_port = htons(config_.multi_recv_port);
  recv_addr_.sin_addr.s_addr = INADDR_ANY;
  int ret = bind(fd_client_, (struct sockaddr*)&recv_addr_, sizeof(recv_addr_));
  if (ret == -1) {
    perror("bind");
    exit(0);
  }
  // inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr.s_addr);
  //  加入到组播
  struct ip_mreqn op;
  op.imr_address.s_addr = INADDR_ANY;  // 本地地址
  inet_pton(AF_INET, config_.multi_recv_addr.c_str(), &op.imr_multiaddr.s_addr);
  op.imr_ifindex = if_nametoindex("ens33");

  setsockopt(fd_client_, IPPROTO_IP, IP_ADD_MEMBERSHIP, &op, sizeof(op));
}

int UdpMulticaster::SendMsg(const char* msg, size_t size) {
  return sendto(fd_, msg, size, 0, (struct sockaddr*)&cliaddr_,
                sizeof(cliaddr_));
}

void UdpMulticaster::RunMulticastReceiver() {
  printf("Run MulticastReceiver\n");
  receiver_running_flag_ = true;
  // msg_cb_ = callback;
  char msg[MaxMsgSize];
  while (receiver_running_flag_) {
    int recv_size = recvfrom(fd_client_, msg, MaxMsgSize, 0, NULL, NULL);
    if (recv_size != -1) {
      if (msg_cb_ == nullptr && config_.debug)
        printf("callback function is not set\n");
      else
        msg_cb_(msg, recv_size);
    } else
      printf("receive error\n");
  }
  printf("Stop MulticastReceiver\n");
}