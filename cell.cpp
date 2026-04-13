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


vector <char*>  split_line (string &line ){
    vector<char*> tokens; 

    size_t st = line.find_first_not_of(DEL);

    while(st != string::npos) {
         
        tokens.push_back(&line[st]);
        
        size_t ed = line.find_first_of(DEL, st);

        if (ed == string::npos)   break ; 

        line[ed] = '\0'; 
            
        st = line.find_first_not_of(DEL, ed + 1);
    }

   
    tokens.push_back(nullptr);

    return tokens ;

}

int main(  ){
    PB1() ; 

    string  line ;
    while ( true ) { 
        
         line = cell_read_line() ;
         if (line == "" && cin.eof() ) break ; 
       
         
         p(Y << "line : " << line  << endl << RST) ; 

         vector<char*> arg = split_line(line);
        
        int pid = fork() ; 
        if(!pid){
            execvp(arg[0] , arg.data()) ; 
            p(RED "Command not found : " << arg[0] << "\n" RST) ; 
            exit(EXIT_FAILURE) ;
        }
         

    
        
    }
    return EXIT_SUCCESS ; 
}