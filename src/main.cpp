#include "command.hpp"
#include "utility.hpp"
#ifdef ENABLE_WEB
#include "server.hpp"
#endif // ENABLE_WEB

int main(int argv, char* argc[]) {
#ifdef ENABLE_WEB
    if (argv == 2 && acm::Camp(argc[1], "-s") == 0) {
        acm::MyWebServer app;
        app.Run();
    } else {
        acm::CommandManager app(argv == 2 && acm::Camp(argc[1], "-d") == 0);
        app.Run();
    }
#else
    acm::CommandManager app(argv == 2 && acm::Camp(argc[1], "-d") == 0);
    app.Run();
#endif // ENABLE_WEB
    return 0;
}