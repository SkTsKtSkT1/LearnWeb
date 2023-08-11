#include "skt/application/application.h"


int main(int argc, char** argv){
    skt::Application app;
    if(app.init(argc, argv)){
        return app.run();
    }
    return 0;
}

