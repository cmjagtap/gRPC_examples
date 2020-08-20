#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <grpcpp/grpcpp.h>
#include "myStream.grpc.pb.h"
#include <thread>         // std::this_thread::sleep_for
#include <chrono>  
using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientWriter;
using grpc::Status;
using userPackage::FileUpload;
using userPackage::FileContent;
using userPackage::FileAck;

#define BUFF_SIZE 2048

class MyClient 
{

 private:
    std::unique_ptr<FileUpload::Stub> stub_;

 public:
  
  MyClient(std::shared_ptr<Channel> channel) : stub_(FileUpload::NewStub(channel)) {}

    void sendFileToServer(std::string filename)
    {
     char *buffer = new char[BUFF_SIZE];
    
     ClientContext context;
     FileAck fileack;
     FileContent filecontent;
     
     unsigned int client_connection_timeout = 5;
     std::chrono::system_clock::time_point deadline = std::chrono::system_clock::now() + std::chrono::seconds(client_connection_timeout);
     //context.set_deadline(deadline);
    
    std::unique_ptr<ClientWriter<FileContent> > writer(stub_->Store(&context, &fileack));
    filecontent.clear_content();
    filecontent.set_content(filename);
    writer->Write(filecontent); //sending file name to server

    // context.TryCancel();

    std::ifstream fileStream(filename, std::ios::binary);

    while (fileStream)
    {   
      filecontent.clear_content();
      fileStream.read(buffer,BUFF_SIZE);
      filecontent.set_content(std::string(buffer, BUFF_SIZE));
      writer->Write(filecontent);
    }
     
     writer->WritesDone();
     Status status = writer->Finish();
         delete[] buffer;


      if (status.ok())
      {
        std::cout << "Store rpc succeeded." << std::endl;
        std::cout << "Server Reply : " << fileack.message() <<std::endl;
  
      } 
      else 
      {
        std::cout << "Store rpc failed." << std::endl;
        std::cout << status.error_code() << ": " << status.error_message()<< std::endl;

      }
      delete[] buffer;
    }

};


int main(int argc, char** argv)
 {
  if(argc < 2)
  {
    std::cout << "Usage: ./exe  file"<<std::endl;
    exit(1);
  }

  std::string address = "127.0.0.1:5000";

  MyClient client(grpc::CreateChannel(address, grpc::InsecureChannelCredentials()));
  
  std::cout << "Client connected to : " << address << std::endl;
  
  client.sendFileToServer(argv[1]);


  return 0;
}
