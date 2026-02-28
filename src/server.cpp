#include "src/server.hpp"

Server::Server(int port)
    : port_(port), all_core_count_(0), method_('b') {}
void Server::Run() {
    boost::thread_group threads;
    threads.create_thread(boost::bind(&Server::AcceptThread, this));
    threads.create_thread(boost::bind(&Server::HandleRequestsThread, this));
    threads.join_all();
}
size_t Server::SendReqsToClients(double a, double b, double step) {
    boost::recursive_mutex::scoped_lock lk(cs_);
    double to_start = a;
    for (array::iterator it = clients_.begin(), e = clients_.end(); it != e; ++it) {
        double part = double((*it)->CoreCount()) / all_core_count_;
        double cl_a = to_start, cl_b = to_start + (b - a) * part;
        to_start = cl_b;
        (*it)->WriteRequest(CalcRequest{cl_a, cl_b, step, method_});
        std::cout << "очередному клиенту отправлена задача: " << cl_a << ' ' << cl_b << '\n';
    }
    return clients_.size();
}
double Server::GetRequestAnswer(size_t active_clients_count) {
    std::vector<double> answers(active_clients_count, std::nan("not calculated"));
    size_t calculated_count = 0;
    while (calculated_count < active_clients_count) {
        boost::this_thread::sleep(millisec(1));
        boost::recursive_mutex::scoped_lock lk(cs_);
        for (size_t i = 0; i < active_clients_count; ++i) {
            if (std::isnan(answers[i]) && clients_[i]->HasAnswer()) {
                answers[i] = clients_[i]->ReadAnswer();
                ++calculated_count;
            }
        }
    }
    // result
    double ans = 0;
    for (double ai : answers)
        ans += ai;
    return ans;
}
void Server::HandleRequestsThread() {
    while (true) {
        double a, b, step;
        std::cin >> a >> b >> step;
        size_t active_clients_count = SendReqsToClients(a, b, step);
        std::cout << "приближенное значение интеграла: " << GetRequestAnswer(active_clients_count) << '\n';
    }
}
void Server::AcceptThread() {
    ip::tcp::acceptor acceptor(service_, ip::tcp::endpoint(ip::tcp::v4(), port_));
    while (true) {
        client_ptr new_(new TalkToClient(service_));
        acceptor.accept(new_->Sock());
        new_->InitCoreCount();
        boost::recursive_mutex::scoped_lock lk(cs_);
        all_core_count_ += new_->CoreCount();
        clients_.push_back(new_);
        std::cout << "подключение нового клиента с количеством ядер: " << new_->CoreCount() << '\n';
    }
}