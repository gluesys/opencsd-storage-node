#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>
//#include<string.h>

#include <cstdio>
#include <string>
#include <iostream>

#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"


using namespace ROCKSDB_NAMESPACE;

std::string kDBPath = "/tmp/rocksdb_simple_example";

int rocksdbPut(char* data, size_t size) {

  DB* db;
  Options options;
  // Optimize RocksDB. This is the easiest way to get RocksDB to perform well
  options.IncreaseParallelism();
  options.OptimizeLevelStyleCompaction();
  // create the DB if it's not already present
  options.create_if_missing = true;

  std::cout << "check0" << std::endl;
  // open DB
  Status s = DB::Open(options, kDBPath, &db);
  assert(s.ok());
  std::cout << "check1" << std::endl;

  // Put key-value
  // s = db->Put(WriteOptions(), "key1", "value");
  Slice s_value(data,size);

  s = db->Put(WriteOptions(), "key2", s_value);
  assert(s.ok());
  std::string value;
  // get value
  s = db->Get(ReadOptions(), "key2", &value);
  assert(s.ok());

  for (int i=0; i< size; i++) {
    assert(value.c_str()[i] == data[i]);
  }

  std::cout << "check2" << std::endl;


  Iterator* it = db->NewIterator(ReadOptions());

  for (it->SeekToFirst(); it->Valid(); it->Next()) {
      std::cout << it->key().data() << " / " << it->value().data() << std::endl;

  }

  db->Flush(FlushOptions());

  delete it;
  delete db;

  return 0;
}

int rocksdbGet() {
  DB* db;
  Options options;
  // Optimize RocksDB. This is the easiest way to get RocksDB to perform well
  options.IncreaseParallelism();
  options.OptimizeLevelStyleCompaction();
  // create the DB if it's not already present
  options.create_if_missing = true;

  std::cout << "check0" << std::endl;
  // open DB
  Status s = DB::Open(options, kDBPath, &db);
  assert(s.ok());
  std::cout << "check1" << std::endl;


  std::vector<std::string> values;
  std::vector<Status> status_list = db->MultiGet(
      ReadOptions(),
      std::vector<Slice>({Slice("aaa"), Slice("ccc"), Slice("eee"),
                          Slice("ggg"), Slice("iii"), Slice("kkk")}),
      &values);


  return 0;
}

void rocksdbPutTest(char* data, size_t size) {
  DB* db;
  Options options;
  // Optimize RocksDB. This is the easiest way to get RocksDB to perform well
  options.IncreaseParallelism();
  options.OptimizeLevelStyleCompaction();
  // create the DB if it's not already present
  options.create_if_missing = true;

  std::cout << "check0" << std::endl;
  // open DB
  Status s = DB::Open(options, kDBPath, &db);
  assert(s.ok());
  std::cout << "check1" << std::endl;

  // Put key-value
  // s = db->Put(WriteOptions(), "key1", "value");
  Slice s_value(data,size);

  s = db->Put(WriteOptions(), "keti_rocksdb_01", s_value);
  assert(s.ok());
  std::string value;
  // get value
  s = db->Get(ReadOptions(), "keti_rocksdb_02", &value);
  assert(s.ok());

  for (int i=0; i< size; i++) {
    assert(value.c_str()[i] == data[i]);
  }

  std::cout << "check2" << std::endl;


  Iterator* it = db->NewIterator(ReadOptions());

  for (it->SeekToFirst(); it->Valid(); it->Next()) {
      std::cout << it->key().data() << " / " << it->value().data() << std::endl;

  }

  db->Flush(FlushOptions());

  delete it;
  delete db;
}

int main(int argc, char* argv[])
{
    int serv_sock;
    int clint_sock;

    struct sockaddr_in serv_addr;
    struct sockaddr_in clint_addr;
    socklen_t clnt_addr_size;

    if(argc != 2)
    {
        printf("%s <port>\n", argv[0]);
        exit(1);
    }
    serv_sock = socket(PF_INET, SOCK_STREAM,0); //1번
    if(serv_sock == -1)
        printf("socket error\n");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock,(struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) //2번
        printf("bind error\n");
    if(listen(serv_sock,5)==-1) //3번
        printf("listen error\n");

    while(1) {

        clnt_addr_size = sizeof(clint_addr);
        clint_sock = accept(serv_sock,(struct sockaddr*)&clint_addr,&clnt_addr_size); //4번
        if(clint_sock == -1)
                printf("accept error\n");

        //printf("accept\n");
        //while(1) {
        char operation[1];
        int str_len;

         if ((str_len = read(clint_sock, operation, 1 )) == -1 ) { //5번
                printf("read error");
                exit(1);
        }
        printf("Write(w)/Read(r): %c\n", operation[0]);



        if (operation[0] == 'w') {
            int MAX_CHAR = 1024;
            char message[MAX_CHAR];


            // read
            if ((str_len = read(clint_sock, message, sizeof(message) )) == -1 ) { //5번
                    printf("read error");
                    exit(1);
            }
            printf("str_len = %d\n", str_len);

            printf("message = ");
            //message[str_len] = '\0';
            for (int i=0; i< str_len; i++) {
                    printf("%c", message[i]);
            }
            printf("\n");


            // Put
            int rc = rocksdbPut(message, str_len);

            // Send

            if (rc == 0) {
                const char msg[20] = "success";
                if ((str_len = send(clint_sock, msg, sizeof(msg), 0)) == -1) {
                    printf("send error");
                    exit(1);
                }

            } else {
                const char msg[20] = "failed";
                if ((str_len = send(clint_sock, msg, sizeof(msg), 0)) == -1) {
                    printf("send error");
                    exit(1);
                }
            }
        }
        else if (operation[0] == 'r') {
            // Put
            int rc = rocksdbGet();

            // Send

            if (rc == 0) {
                const char msg[20] = "success";
                if ((str_len = send(clint_sock, msg, sizeof(msg), 0)) == -1) {
                    printf("send error");
                    exit(1);
                }

            } else {
                const char msg[20] = "failed";
                if ((str_len = send(clint_sock, msg, sizeof(msg), 0)) == -1) {
                    printf("send error");
                    exit(1);
                }
            }
        }



        // if(str_len != MAX_CHAR)
        //         break;
        //}

        //char msg[] = "hello this is server!\n";
        //write(clint_sock, msg, sizeof(msg));

    }
    close(serv_sock); //6번
    close(clint_sock);
    return 0;
}
