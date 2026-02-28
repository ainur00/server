#include "src/server.hpp"

/*!
    @brief Создает сервер и запускает его
*/
int main(int argc, char* argv[]) {
    Server srv(8001);
    srv.Run();
}
