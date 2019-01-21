// Copyright 2019 Carlos O'Ryan
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <echo.grpc.pb.h>
#include <boost/program_options.hpp>
#include <chrono>
#include <functional>
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <thread>

int main(int argc, char* argv[]) try {
  namespace po = boost::program_options;
  using namespace std::chrono_literals;

  po::options_description desc("gRPC Server Options");
  desc.add_options()("address", po::value<std::string>()->default_value("0.0.0.0:7000"));

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
  po::notify(vm);

  auto address = vm["address"].as<std::string>();
  auto channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());

  auto repeater = Echo::Repeater::NewStub(channel);

  for (int i = 0; i != 10; ++i) {
    std::string msg = "ping-" + std::to_string(i);
    grpc::ClientContext context;
    Echo::EchoRequest request;
    request.set_text(std::move(msg));
    Echo::EchoResponse response;
    auto status = repeater->Ping(&context, request, &response);
    if (not status.ok()) {
      std::cerr << "Error in request " << i << " " << status.error_message() << " [" << status.error_code() << "]"
                << std::endl;
      continue;
    }
    std::cout << "Response " << response.text() << std::endl;
    std::this_thread::sleep_for(100ms);
  }

  return 0;
} catch (std::exception const& ex) {
  std::cerr << "Standard exception caught: " << ex.what() << std::endl;
  return 1;
}
