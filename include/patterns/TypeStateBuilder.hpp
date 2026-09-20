#pragma once

struct Data {
  std::string host;
  unsigned short port = 0;
};

struct HasHost {};
struct HasPort {};

template <typename... States>
class RequestBuilder {
  Data data;

public:
  RequestBuilder() = default;
  RequestBuilder(Data data) : data(std::move(data)) {}

  // setHost: checks if HasPort is already in States...
  auto setHost(std::string host) {
    data.host = std::move(host);

    if constexpr ((std::is_same_v<States, HasPort> || ...)) {
      // Both host and port are set -> transition to complete state
      return RequestBuilder<HasHost, HasPort>(std::move(data));
    } else {
      return RequestBuilder<HasHost>(std::move(data));
    }
  }

  // setPort: checks if HasHost is already in States...
  auto setPort(unsigned short port) {
    data.port = port;

    if constexpr ((std::is_same_v<States, HasHost> || ...)) {
      // Both host and port are set -> transition to complete state
      return RequestBuilder<HasHost, HasPort>(std::move(data));
    } else {
      return RequestBuilder<HasPort>(std::move(data));
    }
  }

  // send() is ONLY compiled if BOTH HasHost and HasPort are present in States...
  void send() const requires ((std::is_same_v<States, HasHost> || ...) &&
                              (std::is_same_v<States, HasPort> || ...)) {
    std::println("Requesting {}:{}", data.host, data.port);
  }
};