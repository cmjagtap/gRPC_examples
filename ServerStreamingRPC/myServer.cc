
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <sqlite3.h>

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include "myStream.grpc.pb.h"


using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::Status;
using grpc::StatusCode;
using userPackage::UserName;
using userPackage::ContactInfo;
using userPackage::Greeter;

std::vector< ContactInfo > contactList;

class MyService final : public Greeter::Service 
{
private:
   
   static int callback(void *data, int argc, char **argv, char **azColName)
    { 
   	  ContactInfo contact;
    	  contact.set_name(argv[0]);
   	  contact.set_contactnumber(argv[1]);
   	  contactList.push_back(contact);	  
   	  return 0;
     }
   void getSQLData()
    {
  	 sqlite3 *db;
 	 int rc;
   	 char *sql;
   	 char *zErrMsg = 0;
   	 rc = sqlite3_open("streaming.db", &db);
   	if( rc )
   	{
           std::cout << "Can't open database: %s\n" << sqlite3_errmsg(db);
   	}
   	else
   	{
           std::cout<< "Connected to db"<<std::endl;
   	}
      	sql = "SELECT * from phonebook";

   	rc = sqlite3_exec(db, sql, callback, NULL, &zErrMsg);
   
   	if( rc != SQLITE_OK ) 
   	{
      		fprintf(stderr, "SQL error: %s\n", zErrMsg);
      		sqlite3_free(zErrMsg);
   	} 
   	      sqlite3_close(db);

   }
    
   Status getPhoneBook(ServerContext* context,const UserName* request, ServerWriter<ContactInfo>* writer) override 
   {
   
    getSQLData();

    std::cout << "Got Streaming request for " << request->name();
    
    for (const ContactInfo& f : contactList) 
    {
        if(!writer->Write(f))
        {
        	break;
        }
    }
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
