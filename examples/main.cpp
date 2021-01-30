#include <cstring>
#include <iostream>

#include "../include/ufsm.h"
#include "cmdline.h"

// Precompiler define to get only filename;
#if !defined(__FILENAME__)
#define __FILENAME__                                       \
  (strrchr(__FILE__, '/')    ? strrchr(__FILE__, '/') + 1  \
   : strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 \
                             : __FILE__)
#endif

#define LOG std::cout << "(" << __FILENAME__ << ":" << __LINE__ << ") "

#define MARK_FUNCTION LOG << __PRETTY_FUNCTION__ << std::endl

#define MARK(TYPE)          \
 public:                    \
  TYPE() { MARK_FUNCTION; } \
  virtual ~TYPE() { MARK_FUNCTION; }

class Connector : public ufsm::State<Connector> {
 public:
  struct EvConnect : public ufsm::Event {
    MARK(EvConnect)
  };
  virtual ufsm::Transition Process(const EvConnect &event) {
    MARK_FUNCTION;
    return {};
  }

  struct EvConnectSuccess : public ufsm::Event {
    MARK(EvConnectSuccess)
  };
  virtual ufsm::Transition Process(const EvConnectSuccess &event) {
    MARK_FUNCTION;
    return {};
  }

  struct EvConnectFailure : public ufsm::Event {
    MARK(EvConnectFailure)
  };
  virtual ufsm::Transition Process(const EvConnectFailure &event) {
    MARK_FUNCTION;
    return {};
  }

  struct EvDisconnect : public ufsm::Event {
    MARK(EvDisconnect)
  };
  virtual ufsm::Transition Process(const EvDisconnect &event) {
    MARK_FUNCTION;
    return ufsm::NoTransit{};
  }

  struct EvDisconnectSuccess : public ufsm::Event {
    MARK(EvDisconnectSuccess)
  };
  virtual ufsm::Transition Process(const EvDisconnectSuccess &event) {
    MARK_FUNCTION;
    return ufsm::NoTransit{};
  }
};

class Connecting;
class Disconnecting;
class Connected;
class Disconnected;

class Disconnected : public Connector {
  ufsm::Transition Process(const EvConnect &event) override {
    MARK_FUNCTION;
    return Transit<Connecting>();
  }
  MARK(Disconnected);
};

class Connecting : public Connector {
  ufsm::Transition Process(const EvConnectSuccess &event) override {
    return Transit<Connected>();
  }
  ufsm::Transition Process(const EvConnectFailure &event) override {
    return Transit<Disconnected>();
  }
  MARK(Connecting);
};

class Connected : public Connector {
  ufsm::Transition Process(const EvDisconnect &event) override {
    return Transit<Disconnecting>();
  }
  MARK(Connected);
};

class Disconnecting : public Connector {
  ufsm::Transition Process(const EvDisconnect &event) override {
    return Transit<Disconnected>();
  }
  MARK(Disconnecting);
};

int main() {
  ufsm::Fsm<Connector> connector;
  connector.Initiate<Disconnected>();
  Cmdline cmdline;
  cmdline
      .Add("connect", [&]() { connector.ProcessEvent(Connector::EvConnect{}); })
      .Add("disconnect",
           [&]() { connector.ProcessEvent(Connector::EvDisconnect{}); })
      .Add("connect_success",
           [&]() { connector.ProcessEvent(Connector::EvConnectSuccess{}); })
      .Add("connect_failure",
           [&]() { connector.ProcessEvent(Connector::EvConnectFailure{}); })
      .Add("disconnect_success",
           [&]() { connector.ProcessEvent(Connector::EvDisconnectSuccess{}); });
  std::cout << cmdline.Dump();

  std::string str;
  while (std::cin >> str) {
    if (cmdline.ProcessCmd(str)) {
      std::cout << "Command executed." << std::endl;
    } else {
      std::cout << cmdline.Dump();
    }
  }
}
