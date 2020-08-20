#include <sqlite3.h>
#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include "myStream.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientWriter;
using grpc::Status;
using userPackage::Greeter;
using userPackage::StudentInfo;
using userPackage::StudentSummary;

std::vector< StudentInfo > studentList;

class MyClient 
{

 private:
  std::unique_ptr<Greeter::Stub> stub_;

 public:
  
  MyClient(std::shared_ptr<Channel> channel) : stub_(Greeter::NewStub(channel)) {}

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

    void sendStudentInfo()
    {
      StudentSummary summary;

      ClientContext context;
      getSQLData();

      std::unique_ptr<ClientWriter<StudentInfo> > writer(stub_->StudentRecord(&context, &summary));
       
       for (const StudentInfo& f : studentList) 
        {
          if (!writer->Write(f)) 
          {
            break;
          }
        }
      writer->WritesDone();
      Status status = writer->Finish();

      if (status.ok())
      {
        std::cout << "Student record rpc succeeded." << std::endl;
        std::cout << "Server Reply : " << summary.message() <<std::endl;
  
      } 
      else 
      {
        std::cout << "Student rpc failed." << std::endl;
        std::cout << status.error_code() << ": " << status.error_message()<< std::endl;

      }
    }

};


int main(int argc, char** argv)
 {
  std::string address = "127.0.0.1:5000";


  MyClient client(grpc::CreateChannel(address, grpc::InsecureChannelCredentials()));
  
  std::cout << "Client connected to : " << address << std::endl;
  
  client.sendStudentInfo();


  return 0;
}
