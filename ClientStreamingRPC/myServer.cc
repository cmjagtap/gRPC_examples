
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
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::Status;
using grpc::StatusCode;
using userPackage::Greeter;
using userPackage::StudentInfo;
using userPackage::StudentSummary;

class MyService final : public Greeter::Service 
{
private:

 
  Status StudentRecord(ServerContext* context, ServerReader<StudentInfo>* reader, StudentSummary* summary) override
  {
    StudentInfo stdinfo;

    std::cout << "\n\n Student info received from client is \n";
    std::cout<<" Name \t Class \t Address \n" << std::endl;

    while (reader->Read(&stdinfo))
    {
    	std::cout << " " << stdinfo.name();
        std::cout << "\t" << stdinfo.stdclass();
        std::cout << "\t" << stdinfo.address()	 <<std::endl;
    }

    summary->set_message("Records received at Server side ");
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
