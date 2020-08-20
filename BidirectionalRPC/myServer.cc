
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
using grpc::ServerReaderWriter;
using grpc::Status;
using grpc::StatusCode;
using userPackage::Greeter;
using userPackage::StudentInfo;
using userPackage::StudentResult;

std::vector< StudentInfo > studentList;


class MyService final : public Greeter::Service 
{
private:

  static int callback(void *data, int argc, char **argv, char **azColName)
  {
      StudentInfo s1; 
      s1.set_name(argv[0]);
      s1.set_stdclass(argv[1]); 
      s1.set_address(argv[2]);   
      studentList.push_back(s1);
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

      sql = "SELECT * from studentinfo";

      rc = sqlite3_exec(db, sql, callback, NULL, &zErrMsg);
   
      if( rc != SQLITE_OK ) 
      {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
      } 
      sqlite3_close(db);

  }

    Status getStudentResults(ServerContext* context, ServerReaderWriter<StudentInfo, StudentInfo>* stream) 
    {
     
    StudentInfo stdinfo;
    
    getSQLData();

    std::cout << "\n\n Student info received from client is \n";
    std::cout<<" Name \t Class \t Address \n" << std::endl;

    int i = 0;
    while (stream->Read(&stdinfo))
    {
    	  std::cout << " " << stdinfo.name();
        std::cout << "\t" << stdinfo.stdclass();
        std::cout << "\t" << stdinfo.address()	 <<std::endl;
        stream->Write(studentList[i++]);
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
