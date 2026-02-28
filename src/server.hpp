#pragma once

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <iostream>

#include "src/talk_to_client.hpp"
#include "src/calc_request.hpp"

typedef boost::shared_ptr<TalkToClient> client_ptr;
typedef std::vector<client_ptr> array;

using namespace boost::asio;
using namespace boost::posix_time;

/*!
    @brief Управляет подключениями клиентов и распределением задач

    Принимает входящие TCP подключения, собирает информацию о вычислительных
        мощностях клиентов и распределяет задачу на вычисление интеграла
        пропорционально количеству ядер CPU на каждом клиенте.
*/
class Server {
public:
    /*!
        @brief Конструктор, инициализирующий сервер на заданном порту
        @param port Порт, на котором сервер будет принимать подключения
    */
    Server(int port);

    /*!
        @brief Запускает сервер — принимает подключения и обрабатывает запросы
    */
    void Run();
private:
    /*!
        @brief Поток принятия новых подключений
    */
    void AcceptThread();

    /*!
        @brief Поток отправки запросов и получения ответов
    */
    void HandleRequestsThread();

    /*!
        @brief Собирает результаты вычислений от всех клиентов и суммирует
        @param active_clients_count Количество клиентов участвующих в вычислении
        @return Приближенное значение интеграла
    */
    double GetRequestAnswer(size_t active_clients_count);

    /*!
        @brief Рассылает задачу на вычисление всем подключённым клиентам
            пропорционально количеству ядер CPU на каждом из них
        @param a Левая граница интегрирования
        @param b Правая граница интегрирования
        @param step Шаг интегрирования
        @return Количество клиентов которым была отправлена задача
    */
    size_t SendReqsToClients(double a, double b, double step);
private:
    int port_; 
    io_context service_;
    array clients_;
    int all_core_count_;
    boost::recursive_mutex cs_;
    char method_;
};