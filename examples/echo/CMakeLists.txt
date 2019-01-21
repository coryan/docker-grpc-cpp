# Copyright 2019 Carlos O'Ryan
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.

find_package(Boost REQUIRED COMPONENTS program_options)
find_package(Threads REQUIRED)
find_package(gRPC CONFIG REQUIRED)

include(CompileProtos)
list(APPEND PROTOBUF_IMPORT_DIRS
        "${CMAKE_CURRENT_BINARY_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}")

# Sometimes (this happens often with vcpkg) protobuf is installed in a non-
# standard directory. We need to find out where, and then add that directory to
# the search path for protos.
find_path(PROTO_INCLUDE_DIR google/protobuf/descriptor.proto)
if (PROTO_INCLUDE_DIR)
    list(INSERT PROTOBUF_IMPORT_DIRS 0 "${PROTO_INCLUDE_DIR}")
endif ()

protobuf_generate_cpp(
        PROTO_SOURCES
        PROTO_HDRS
        echo.proto)
grpc_generate_cpp(
        GRPCPP_SOURCES
        GRPCPP_HDRS
        echo.proto)
# Create a library with the generated files from the relevant protos.
add_library(echo_protos EXCLUDE_FROM_ALL
        ${PROTO_SOURCES}
        ${PROTO_HDRS}
        ${GRPCPP_SOURCES}
        ${GRPCPP_HDRS})
target_link_libraries(echo_protos
        gRPC::grpc++
        gRPC::grpc
        protobuf::libprotobuf)
target_include_directories(echo_protos
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
               $<INSTALL_INTERFACE:include>)

add_executable(echo_server echo_server.cpp)
target_link_libraries(echo_server
        PUBLIC Boost::program_options echo_protos gRPC::grpc++)

add_executable(echo_client echo_client.cpp)
target_link_libraries(echo_client
        PUBLIC Boost::program_options echo_protos gRPC::grpc++)
