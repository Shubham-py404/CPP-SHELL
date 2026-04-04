#include "cell.hpp"


using namespace std ; 

// run command  - g++ cell.cpp -o cell && ./cell <command >

//demo 
// int main(int ac , char** av ){ // argument vector
//      (void) ac ; // ac (argument count) is the number of items typed in the terminal
//      // void ac is to avoid unused variable warning 
//     int status ; 
//     if (fork()==0){ /// returning 0 means we are in child process 
//         execvp(av[1] , av+1) ; // excvp is used for executing a command in linux systems
//     }
//     wait(&status) ; 
//     return EXIT_SUCCESS ; 
// }

// lets use loop to run multiple commands 
// REPL - read , eval , print / exec  , loop ; 

char *  cell_read_line( void  )
{

    // todo - replace buf and size with a string aaa
     char * buf ; 
     size_t bufsize ; 
     p(GREEN "$$> " RST) ;
     if(getline(&buf , &bufsize , stdin) == -1 ){
         buf  = NULL ; 

        if(feof(stdin)){
            p(RED "[EOF]\n" RST) ; 

        }
        else {
        
          p(RED "Getline  failed\n" RST) ; 
        }
     }  
     cout << YELLOW "You entered: " << buf << RST ;
     return buf ; 
}



int main(int ac , char** av ){ // argument vector
    char * line ; 


    while (true){
        // get line

        // getline() //- >  stdio that is used for input and output in c++ (input , string name )
        line = cell_read_line( ) ; 
         if (line == NULL){
             break ; 
         }
        p(line) ; 
         free(line) ; // free the memory allocated by getline
    }
    return EXIT_SUCCESS ; 
}