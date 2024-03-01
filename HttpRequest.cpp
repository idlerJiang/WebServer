//
// Created by Fisher on 2024/3/1.
//

#include "HttpRequest.h"

HttpRequest::HttpRequest() {
    method = "";
    path = "";
    version = "";
    body = "";
    state = REQUEST_LINE;
    header.clear();
    post.clear();
}

bool HttpRequest::parse(Buffer &buffer) {
    const char CRLF[] = "\r\n";
    if (buffer.readable_bytes() <= 0) return false;
    while (buffer.readable_bytes() && state != FINISH) {
        const char *lineEnd = std::search(buffer.peek(), buffer.get_write_position(), CRLF, CRLF + 2);
        std::string line(buffer.peek(), lineEnd); //封装在line里
        switch (state) {
            case REQUEST_LINE:
                if (!parse_request_part(line))
                    return false;
                parse_path_part();
                break;
            case HEADERS:
                parse_head_part(line);
                if (buffer.readable_bytes() <= 2)
                    state = FINISH;

                break;
            case BODY:
                parse_body_part(line);
                break;
            default:
                break;
        }
        if (lineEnd == buffer.get_write_position()) break;
        buffer.move_read_position_till(lineEnd + 2);
    }
    std::cout << "解析结果 method_:" << method << " path_:" << path << " version_:" << version << std::endl;
    return true;
}

std::string HttpRequest::get_method() const {
    return method;
}

std::string HttpRequest::get_path() const {
    return path;
}

std::string &HttpRequest::get_path() {
    return path;
}

std::string HttpRequest::get_version() const {
    return version;
}

std::unordered_map<std::string, int> HttpRequest::get_post() {
    return post;
}

bool HttpRequest::is_keep_alive() const {
    if (header.count("Connection") == 1)
        return header.find("Connection")->second == "keep-alive" && version == "1.1";
    return false;
}

bool HttpRequest::parse_request_part(const std::string &line) {
    std::regex patten("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
    std::smatch subMatch;
    if (regex_match(line, subMatch, patten)) {   // 匹配成功就保存在subMatch里面
        method = subMatch[1];   // GET
        path = subMatch[2];     // /
        version = subMatch[3];  // 1.1
        state = HEADERS;        // 状态改变 解析头
        return true;
    }
    std::cout << "RequestLine Error" << std::endl;
    return false;
}

void HttpRequest::parse_head_part(const std::string &line) {
    std::regex patten("^([^:]*): ?(.*)$");   //前面是键 后面是值
    std::smatch subMatch;
    if (regex_match(line, subMatch, patten)) {
        header[subMatch[1]] = subMatch[2];
    } else {    // 没匹配成功（数据是 回车换行） 就改变状态为 解析请求体
        state = BODY;
    }
}

void HttpRequest::parse_body_part(const std::string &line) {
    body = line;
    parse_post_part();
    state = FINISH;
    std::cout << "Body:" << line.c_str() << " len:" << line.size() << std::endl;
}

void HttpRequest::parse_path_part() {
    if (path == "/") {
        path = "/index.html";
    } else {
        for (auto &item: DEFAULT_HTML) {
            if (item == path) {
                path += ".html";
                break;
            }
        }
    }
}

void HttpRequest::parse_post_part() {
    if (method == "POST"   /*&& header_["Content-Type"] == "application/x-www-form-urlencoded"*/) {
        std::cout << "解析POST请求" << std::endl;
        parse_url_encoded_part();
        //int len = std::stoi(header_["Content-Length"]);
        //cout<<len<<endl;
        path = "/CGI/compute_.html";
    }
}

void HttpRequest::parse_url_encoded_part() {
    if (body.size() == 0) { return; }
    std::string key;
    int value;
    int n = body.size();
    int i = 0, j = 0;
    for (; i < n; i++) {
        char ch = body[i];
        switch (ch) {
            case '=':
                key = body.substr(j, i - j);
                j = i + 1;
                break;
            case '&':
                value = stoi(body.substr(j, i - j));
                j = i + 1;
                post[key] = value;
                break;
            default:
                break;
        }
    }
    assert(j <= i);
    if (j < i) {
        value = stoi(body.substr(j, i - j));
        post[key] = value;
    }
}
