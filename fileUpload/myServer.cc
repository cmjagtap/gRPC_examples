
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <fstream>

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
using userPackage::FileUpload;
using userPackage::FileContent;
using userPackage::FileAck;
using namespace std;

class MyService final : public FileUpload::Service 
{
private:

  Status Store(ServerContext* context, ServerReader<FileContent>* reader, FileAck* fileack) override
  {
    FileContent filecontent;
    std::cout << "Client started file upload " << std::endl;

    reader->Read(&filecontent);
    ofstream myfile;
    std::string filename;
    std::cout<< "Receving file : " << filecontent.content() <<std::endl;
    
    filename = "uploadedFile/" + filecontent.content();

    myfile.open (filename,std::ios::binary);

     while (reader->Read(&filecontent))
     {
        myfile << filecontent.content();
     }
    
    if (context->IsCancelled())
    {
      std::cout << "Client has cancelled request or deadline exceeded\n";
      return Status(StatusCode::CANCELLED, "Deadline exceeded or Client cancelled");
    }
   
    myfile.close();
    std::cout << "Data succfully written into file"<<std::endl;
    fileack->set_message("FileContent received at Server side ");

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