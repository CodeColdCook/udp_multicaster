#include <gtest/gtest.h>

#include <thread>

#include "udp_multicaster.h"
#include "nmea_parser.h"

NmeaParser paser;

void MsgCallback(char* msg, int size) {
  printf("server say: %s\n", msg);
  printf("re_size: %d\n", size);
  double time = 1920.56;
  NmeaType r = paser.ParseNmeaMessage(msg, time);
  if(r == NmeaType::NMEA_NULL)
    printf("not NMEA message\n");
  else{
    printf("receive a NMEA message of type: %d", (int)r);
  }
}

// TEST(UdpMulticaster, Server) {
//   UdpMulticastConfig config;
//   config.multi_addr = "224.100.100.10";
//   config.multi_port = 20842;
//   UdpMulticaster multicaster(config);
//   int num = 0;
//   // 3. 通信
//   while (num < 10) {
//     // 接收数据
//     char buf[128];
//     // 发送数据
//     sprintf(buf, "hello, client...%d\n ", num++);
//     int r = multicaster.SendMsg(buf, strlen(buf) + 1);
//     EXPECT_TRUE(r > -1);
//     printf("组播的数据: %s\n", buf);
//     sleep(1);
//   }
// }

TEST(UdpMulticaster, Client) {
  UdpMulticastConfig config;
  config.multi_addr = "224.100.100.10";
  config.multi_port = 20842;
  config.multi_recv_addr = "224.100.100.10";
  config.multi_recv_port = 20842;
  UdpMulticaster multicaster(config);
  multicaster.SetMulticastMsgCallback(
      std::bind(&MsgCallback, std::placeholders::_1, std::placeholders::_2));
  std::thread receive_thread;
  receive_thread =
      std::thread(&UdpMulticaster::RunMulticastReceiver, &multicaster);
  receive_thread.join();
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
