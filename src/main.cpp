#include "command.hpp"

int main(int argv, char* argc[]) {
    acm::CommandManager app(argv == 2 && acm::Camp(argc[1], "-d") == 0);
    app.Run();
    return 0;
}