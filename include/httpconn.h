#include "myutils/common.h"
#include <string>


namespace muduo{
namespace net{
class TcpConnection;
}
}

enum class HTTP_CODE
{
    NO_REQUEST,
    GET_REQUEST,
    BAD_REQUEST,
    NO_RESOURCE,
    FORBIDDEN_REQUEST,
    FILE_REQUEST,
    INTERNAL_ERROR,
    CLOSED_CONNECTION
};

class HttpConnection{

public:
    HttpConnection(muduo::net::TcpConnection* conn);

    ~HttpConnection();

    HTTP_CODE parseFromString(const std::string& buf);

    int parseFromLine();

private:

    muduo::net::TcpConnection* conn_;
};