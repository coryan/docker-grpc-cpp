# A Minimal Docker image with a C++ gRPC server.

This project shows how to create a minimal Docker image containing a C++ gRPC
service.

## Requirements

### Operating System

This demo is intended for Linux systems only. It may work on other operating
systems using some form of virtual machine, but I have not tested it.

### Docker

You must have [Docker][docker-link] installed on your workstation. I have only
tested the Docker files with 18.09, but any version greater than 17.05 should
work.

## Create devtools Docker image

First we create a Docker image with all the development tools necessary to
compile the gRPC server. While it is possible to install all these development
tools in your workstation, a Docker image makes the remaining steps easier to
reproduce.

This step may take a few minutes, as it builds gRPC and other dependencies:

```bash
sudo docker build -t grpc-cpp-devtools:latest -f tools/Dockerfile.devtools tools
```

## Create the server Docker image

Once the development tools image is created we can use it to create a Docker
image with a C++ gRPC server:

```bash
sudo docker build -t grpc-cpp-echo:latest -f examples/echo/Dockerfile.server .
```

Note that this image is relatively small:

```bash
sudo docker image ls grpc-cpp-echo:latest
```

```console
REPOSITORY          TAG                 IMAGE ID            CREATED             SIZE
grpc-cpp-echo       latest              04d95e5adaa6        4 minutes ago       14.6MB
```

## Run the server in the Docker image

Use `docker run` to start a container using this image. You may want to detach
from the image using the `-d` option and capture its id so you can terminate it
later:

```bash
ID=$(sudo docker run -d -P grpc-cpp-echo:latest /r/echo_server)
```

Note the mapping of port 7000 to the localhost to ease testing.

## Use the client to test connectivity

The image also contains a small client to demonstrate connecting to it:

```bash
ADDRESS=$(sudo docker port "${ID}" 7000)
sudo docker run --network=host grpc-cpp-echo:latest /r/echo_client --address "${ADDRESS}"
```

## Terminate the container

```bash
sudo docker kill "${ID}"
```

[docker-link]: https://www.docker.com/
