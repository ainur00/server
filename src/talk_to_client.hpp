#pragma once

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include "src/calc_request.hpp"

using namespace boost::asio;

/*!
    @brief Представляет подключённого клиента и управляет обменом данными с ним

    Хранит сокет соединения, информацию о вычислительных мощностях клиента.
        Наследует enable_shared_from_this для возможности копирования.
*/
class TalkToClient : public boost::enable_shared_from_this<TalkToClient> {
public:
    /*!
        @brief Конструктор, инициализирующий сокет на основе переданного io_context
        @param service Ссылка на io_context для создания сокета
    */
    TalkToClient(io_context& service);

    /*!
        @brief Отправляет клиенту задачу на вычисление интеграла
        @param req Объект с параметрами задачи
    */
    void WriteRequest(const CalcRequest& req);

    /*!
        @brief Пытается дочитать сообщение и проверяет, готов ли ответ от клиента
        @return true если клиент прислал результат вычислений
    */
    bool HasAnswer();

    /*!
        @brief Читает результат вычислений от клиента из буфера
        @return Вычисленное клиентом значение интеграла
    */
    double ReadAnswer();

    /*!
        @brief Читает от клиента количество доступных ядер CPU и сохраняет его
    */
    void InitCoreCount();
private:
    /*!
        @brief Completion-функция для чтения количества ядер,
            определяет когда сообщение прочитано полностью
        @param err Код ошибки
        @param bytes Количество байт прочитанных на данный момент
        @return Количество байт которое ещё нужно прочитать, 0 если сообщение полное
    */
    size_t ReadCoreCountComplete(const boost::system::error_code&, size_t);
public:
    /*!
        @brief Возвращает ссылку на сокет соединения с клиентом
        @return Ссылка на TCP сокет
    */
    ip::tcp::socket& Sock();

    /*!
        @brief Возвращает количество ядер CPU на стороне клиента
        @return Количество доступных ядер
    */
    int CoreCount() const;
private:
    enum { max_msg_ = 1024 };
    ip::tcp::socket sock_;
    int already_read_;
    char buff_[max_msg_];
    bool started_;
    int core_count_;
};