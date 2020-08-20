#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include "myStream.grpc.pb.h"

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;
using userPackage::UserName;
using userPackage::UserAddress;
using userPackage::Greeter;


class MyClient 
{
 private:
  std::unique_ptr<Greeter::Stub> stub_;

 public:
  
  MyClient(std::shared_ptr<Channel> channel) : stub_(Greeter::NewStub(channel)) {}

  std::string requestServer() 
  {
    UserName request;
    UserAddress reply;
    request.set_name("CmHacker");

    ClientContext context;
   // grpc_connectivity_state GetState(context.READY);
  
  //std::Cout << "State responce " << WaitForStateChange(context.IDLE, 2);


    Status status = stub_->SayHello(&context, request, &reply);

    if (status.ok())
     {
      return reply.message();
    } 
    else
     {
      std::cout << status.error_code() << ": " << status.error_message()<< std::endl;
      return "RPC failed";
    }
  }
};
  
 
int main(int argc, char** argv)
 {
   std::string address = "127.0.0.1:5000";

  MyClient client(grpc::CreateChannel(address, grpc::InsecureChannelCredentials()));
  
  std::cout << "Client connected to : " << address << std::endl;
  
  std::cout << "Server Reply : " << client.requestServer() << std::endl;
  
  return 0;
}
