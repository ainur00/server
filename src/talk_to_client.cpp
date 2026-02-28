#include "src/talk_to_client.hpp"

TalkToClient::TalkToClient(io_context& service)
    : sock_(service), started_(false), already_read_(0) {}
ip::tcp::socket& TalkToClient::Sock() {
    return sock_;
}
int TalkToClient::CoreCount() const {
    return core_count_;
}
void TalkToClient::WriteRequest(const CalcRequest& req) {
    already_read_ = 0;
    std::string msg = std::to_string(req.a) + ";" + std::to_string(req.b) + ";" + std::to_string(req.step) + ";" + std::to_string(req.method) + "\n";
    sock_.write_some(buffer(msg));
}
bool TalkToClient::HasAnswer() {
    if (sock_.available())
        already_read_ += sock_.read_some(buffer(buff_ + already_read_, max_msg_ - already_read_));
    bool found_enter = std::find(buff_, buff_ + already_read_, '\n') < buff_ + already_read_;
    if (found_enter)
        return true;
    return false;
}
double TalkToClient::ReadAnswer() {
    return std::stod(std::string(buff_, already_read_ - 1));
}
void TalkToClient::InitCoreCount() {
    read(sock_, buffer(buff_), boost::bind(&TalkToClient::ReadCoreCountComplete, this, _1, _2));
    core_count_ = std::stoi(std::string(buff_, already_read_));
}
size_t TalkToClient::ReadCoreCountComplete(const boost::system::error_code& err, size_t bytes) {
    if (err)
        return 0;
    already_read_ = bytes;
    bool found = std::find(buff_, buff_ + bytes, '\n') < buff_ + bytes;
    return found ? 0 : 1;
}