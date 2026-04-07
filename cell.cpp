#include "cell.hpp"


using namespace std ; 



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

// char *  cell_read_line( void  )
// {

//     // todo - replace buf and size with a string aaa
//      char * buf = NULL ; 
//      size_t bufsize =0  ; 
//      char cwd[BUFSIZ] ;         
 
//      Getcwd(cwd , sizeof(cwd)) ; 
     
//      p(C <<  cwd << " $$> " << RST)  ; 
         


//      if(getline(&buf , &bufsize , stdin) == -1 ){
//          buf  = NULL ; 

//         if(feof(stdin)){
//             p(RED "\n" RST) ; 

//         }
//         else {
        
//           p(RED "Getline  failed\n" RST) ; 
//         }
//      }  
//      return buf ; 
// }

string cell_read_line(){
    char  cwd[BUFSIZ] ; 
    Getcwd(cwd , sizeof(cwd )) ; 
     p(C <<  cwd << " $$> " << RST)  ; 
     string line ; 
     if(!getline(cin  , line) ){
        line="" ; 
         if (cin.eof ()){
            p(RED "\n" RST ) ; 
         }
         else {
             p(RED "Getline  failed\n" RST) ; 
         }
     }
     return line ; 
}


// vector <string>  split_line (){
//     string line ; 
//     return line ; 
// }

int main(  ){ // argument vector
    PB1() ; 

    string line ; 
  
    while ( true ){
        // get line
      line= cell_read_line() ;

         if (line == "" && cin.eof() ){ // if line is empty and end of file is reached
             break ; 
         }
        p(Y << line  << endl << RST) ; 
            
    }
    return EXIT_SUCCESS ; 
}