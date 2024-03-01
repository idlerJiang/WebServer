//
// Created by Fisher on 2024/3/1.
//

#ifndef WEBSERVER_HTTPREQUEST_H
#define WEBSERVER_HTTPREQUEST_H

#include "Buffer.h"
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <regex>

class HttpRequest {
public:
    HttpRequest();

    enum PARSE_STATE {
        REQUEST_LINE,   //正在解析请求首行
        HEADERS,        //头
        BODY,           //体
        FINISH,         //完成
    };

    enum HTTP_CODE {
        NO_REQUEST = 0,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOURSE,
        FORBIDDENT_REQUEST,
        FILE_REQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION,
    };

    bool parse(Buffer &);

    std::string get_method() const;

    std::string get_path() const;

    std::string &get_path();

    std::string get_version() const;

    std::unordered_map<std::string, int> get_post();

    bool is_keep_alive() const;     // 是否保持长连接

private:
    bool parse_request_part(const std::string &line);

    void parse_head_part(const std::string &line);

    void parse_body_part(const std::string &line);

    void parse_path_part();

    void parse_post_part();

    void parse_url_encoded_part();  // 解析表单数据

    PARSE_STATE state;
    std::string method, path, version, body;
    std::unordered_map<std::string, std::string> header;
    std::unordered_map<std::string, int> post;     //post 请求表单数据

    static const std::unordered_set<std::string> DEFAULT_HTML;
};


#endif //WEBSERVER_HTTPREQUEST_H
