#include "cell.hpp"


t_builtin g_builtin[] = 
{
    {.builtin_name = "env" ,  .foo = cell_env}  ,
  {.builtin_name = "exit" , .foo = cell_exit}  ,
  {.builtin_name = "time", .foo = cell_time},
  {.builtin_name = NULL , .foo = nullptr} , 
} ; 


int status =0 ; 


// lets use loop to run multiple commands 
// REPL - read , eval , print / exec  , loop ; 

// is built- in 
//   call it - echo , env , exit 
// else -> fork + execvp +wait

void cell_launch(vector <char*> & arg ){
    int pid = fork() ;
    if(pid==0){
           if ( execvp(arg[0] , arg.data()) == -1 ) {
            p(RED "Command not found : " << arg[0] << "\n" RST) ; 
            exit(EXIT_FAILURE) ;
           }
    }
    else if (pid < 0 ){
        p(RED "Failed to fork process\n" RST) ; 
    }
    else {
     
        wait(&status) ;
    }
}

void cell_exec(vector <char*> & args ){
    if (args.empty() || args[0] == nullptr) return;
    int i = 0  ; 
    const char * curr ; 
    while (curr = g_builtin[i].builtin_name) {
        if ( !strcmp(curr , args[0]) ) {
         status =  g_builtin[i].foo(args)  ; 
          return ; 
        }
        ++i ; 
    }
    cell_launch(args) ; 
}


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
       
         

         vector<char*> arg = split_line(line);
        

        cell_exec(arg) ; 

    
        
    }
    return EXIT_SUCCESS ; 
}