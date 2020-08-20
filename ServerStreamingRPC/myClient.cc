#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include "myStream.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::Status;
using userPackage::UserName;
using userPackage::ContactInfo;
using userPackage::Greeter;

class MyClient 
{
 private:
  std::unique_ptr<Greeter::Stub> stub_;

 public:
  
  MyClient(std::shared_ptr<Channel> channel) : stub_(Greeter::NewStub(channel)) {}
  
  void requestPhonebook()
  {
    UserName request;
    ContactInfo phone;

    request.set_name("contactList");
    std::cout << "Requested contactList\n\n";

    ClientContext context;

    std::unique_ptr<ClientReader<ContactInfo> > reader(stub_->getPhoneBook(&context, request));

      std::cout << " Name \t Contact Number\n";
      while(reader->Read(&phone))
      {
        std::cout << " " << phone.name();
        std::cout << "\t" << phone.contactnumber() <<std::endl;
      }
      Status status = reader->Finish();
     
      if (status.ok())
      {
        std::cout << "Request phonebook rpc succeeded." << std::endl;
        } 
      else 
      {
        std::cout << "Request phonebookrpc failed." << std::endl;
        std::cout << status.error_code() << ": " << status.error_message()<< std::endl;
      }
    }
 };

int main(int argc, char** argv)
 {
  
   std::string address = "127.0.0.1:5000";

  MyClient client(grpc::CreateChannel(address, grpc::InsecureChannelCredentials()));
  
  std::cout << "Client connected to : " << address << std::endl;
  
  client.requestPhonebook();
  


  return 0;
}
