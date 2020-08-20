
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include "myStream.grpc.pb.h"


using grpc::Server;
using grpc::ServerBuilder;
using grpc::Status;
using grpc::ServerContext;
using grpc::StatusCode;
using userPackage::UserName;
using userPackage::UserAddress;
using userPackage::Greeter;


class MyService final : public Greeter::Service 
{
private:

  Status SayHello(ServerContext* context, const UserName* request, UserAddress* reply) override 
  {
    std::cout << "Got request from client \n";

    reply->set_message("\nHello "  + request->name() + " Adress is : Satana");
    
    return Status::OK;
  }
	
};

void RunServer() 
{
  std::string server_address("127.0.0.1:5000");
  MyService service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
 
  std::unique_ptr<Server> server(builder.BuildAndStart());

  std::cout << "Server listening on " << server_address << std::endl;

  server->Wait();
}

int main(int argc, char** argv) 
{
  RunServer();
  return 0;
}
