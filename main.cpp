#include "Club.h"

int main(int argc, char* argv[]){

    std::string path;
    if(argc == 2){
        path = argv[1];
    }
    else{
        std::cerr << "no file was given\n";
        return 0;
    }

    Club club(path);

    club.start();
    
    return 0;
}