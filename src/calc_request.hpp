#pragma once

/*!
    @brief Параметры задачи вычисления интеграла

    Описывает отрезок интегрирования и метод вычисления,
        которые сервер передаёт клиенту для обработки.
*/
struct CalcRequest {
    double a;
    double b;
    double step;
    char method;
};