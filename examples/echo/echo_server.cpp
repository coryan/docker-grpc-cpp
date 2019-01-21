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
#include <grpcpp/grpcpp.h>
#include <functional>
#include <iostream>

class EchoImpl : public Echo::Repeater::Service {
 public:
  grpc::Status Ping(
      grpc::ServerContext* context,
      Echo::EchoRequest const* request,
      Echo::EchoResponse* response) override {
    response->set_text(request->text());
    return grpc::Status::OK;
  }
};

int main(int argc, char* argv[]) try {
  namespace po = boost::program_options;
  po::options_description desc("gRPC Server Options");
  desc.add_options()
      ("address", po::value<std::string>()->default_value("0.0.0.0:7000"));

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
  po::notify(vm);

  EchoImpl impl;
  grpc::ServerBuilder builder;
  auto address = vm["address"].as<std::string>();
  builder.AddListeningPort(address, grpc::InsecureServerCredentials());
  builder.RegisterService(&impl);

  auto server = builder.BuildAndStart();
  std::cout << "Server listening on " << address << std::endl;

  server->Wait();

  return 0;
} catch (std::exception const& ex) {
  std::cerr << "Standard exception caught: " << ex.what() << std::endl;
  return 1;
}
