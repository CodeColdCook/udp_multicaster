/**
 * @file udp_multicaster.h
 * @author ZhouWl (zhouwenlong@bdcatek.com)
 * @brief
 * @version 0.1
 * @date 2023-03-22
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include <arpa/inet.h>
#include <gtest/gtest.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <functional>
#include <stdexcept>

// 最大消息长度
#define MaxMsgSize 65535

/// @brief 组播参数，发出地址/端口与接收地址/端口相同时会接收自己发出的消息
typedef struct UdpMulticastConfigS {
  bool debug = false;
  // 组播发出地址，sendto this addr
  std::string multi_addr;
  // 组播发送端口 sendto this port
  int multi_port;

  // 组播接收地址， recvfrom this addr
  std::string multi_recv_addr;
  // 组播接收端口, recvfrom this port
  int multi_recv_port;
} UdpMulticastConfig;

// 消息反馈函数定义
typedef std::function<void(char* msg, int size)> MulticastMsgCallback;

/**
 * @brief 用于接收/发送组播消息
 *  SetMulticastMsgCallback ——> RunMulticastReceiver（）
 *
 */
class UdpMulticaster {
 public:
  typedef std::shared_ptr<UdpMulticaster> Ptr;
  /**
   * @brief Construct a new Udp Multicaster object
   *
   * @param config
   */
  UdpMulticaster(UdpMulticastConfig config);

  /**
   * @brief Destroy the Udp Multicaster object
   *
   */
  ~UdpMulticaster() {
    close(fd_);
    close(fd_client_);
  }

  /**
   * @brief 发送消息至 cliaddr_
   *
   * @param msg
   * @param size 与msg对应的消息长度
   * @return int
   */
  int SendMsg(const char* msg, size_t size);

  /**
   * @brief Set the Multicast Msg Callback object
   * 在RunMulticastReceiver（）中被调用
   *
   * @param callback
   */
  inline void SetMulticastMsgCallback(MulticastMsgCallback callback) {
    msg_cb_ = callback;
  }

  /**
   * @brief 结束接收，关闭线程
   *
   */
  inline void StopMulticastReceiver() { receiver_running_flag_ = false; }

  /**
   * @brief 开启接收线程，需先进行SetMulticastMsgCallback（）
   *
   */
  void RunMulticastReceiver();

 private:
  // 组播参数
  UdpMulticastConfig config_;
  // 接收线程运行标志
  bool receiver_running_flag_;
  // 发送socket
  int fd_;
  // 发送的目标地址
  struct sockaddr_in cliaddr_;

  // 接收socket
  int fd_client_;
  // 接收地址
  struct sockaddr_in recv_addr_;
  // 接收消息的回调函数
  MulticastMsgCallback msg_cb_;
};