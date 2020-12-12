#include <iostream>
#include <ctime>
#include <mutex>
#include <thread>

#include "ErrnoExcept.hpp"
#include "HttpService.hpp"

std::mutex cout_mutex;

std::string create_response(const http::GET_request & req){
    std::string resp("HTTP/1.1 200 OK\r\n");
    time_t seconds = time(NULL);
    tm *timeinfo = localtime(&seconds);
    resp.append("Date: " + std::string(asctime(timeinfo)));
    resp[resp.length() - 1] = '\r';
    resp.append("\n");
    resp.append("Content-Type: text\r\n");
    std::string content;
    for(auto it = req.begin(); it != req.end(); ++it){
        content.append("\"");
        content.append(it->first);
        content.append("\": \"");
        content.append(it->second);
        content.append("\"\n");
    }
    content.append("\r\n");
    resp.append("Content-Length: " + std::to_string(content.length()) + "\r\n");
    if(req.get_value_of("Connection") == "Keep-Alive"){
        resp.append("Connection: Keep-Alive\r\n");
    }
    resp.append("\r\n");
    resp.append(content);
    return resp;
}

struct TestServiceListener : net::IServiceListener
{
    void onRequest(net::BufferedConnection & buf_connection, const http::GET_request & req) override{
        std::string response;
        response = create_response(req);
        buf_connection.write_to_buf(response);
        //std::lock_guard<std::mutex> cout_locker(cout_mutex);
        cout_mutex.lock();
        std::cout << "request processed on thread " << std::this_thread::get_id() << std::endl << std::flush;
        //cout_locker.~lock_guard();
        cout_mutex.unlock();
        buf_connection.update_events(net::EPollEvents::WRONLY);
    }
};

int main(){
    try{
    TestServiceListener listener;
    http::HttpService service(&listener);
    service.open("127.0.0.1", 8080, 1024);
    service.run(4, 1024, 20000);
    }catch(net::ErrnoExcept & e){
        std::cout << e.get_extended_message() << std::endl;
    }catch(std::exception & e){
        std::cout << "some exception" << std::endl;
        std::cout << "Message : " << e.what() << std::endl;
    }
    return 0;
}