#include <iostream>
#include <cpprest/http_listener.h>
using namespace std;
using namespace web::http;
using namespace web::http::experimental::listener;

int main(){
    http_listener listener(U("http://0.0.0.0:8181"));        //Server URL, Port 지정.
    listener.open().then([&listener](){cout << (U("\n start!!\n"));}).wait();    //Server open
    listener.support(methods::GET, [](http_request req){                        //support() 함수를 통해 GET방식 지정
            req.reply(status_codes::OK, U("hello world"));                        //Lamda방식으로 간단하게 구현.
            });
   
    while(true);
    listener.close();
    return 0;
}