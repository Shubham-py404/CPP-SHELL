#include "cell.hpp"

int cell_exit(vector<char *> &args)
{

    {
        cout << "\033[?25l";

        cout << Y << "Initiating shutdown sequence..." << RST << endl;


        int bar_width = 15;
        for (int i = 0; i <= bar_width; ++i)
        {
            
            cout << "\r[";

            
            for (int j = 0; j < bar_width; ++j)
            {
                if (j < i)
                {
                    cout << G << "=" << RST; 
                }
                else if (j == i)
                {
                    cout << C << ">" << RST; 
                }
                else
                {
                    cout << " "; 
                }
            }


            int percent = (i * 100) / bar_width;
            cout << "] " << percent << "% " << flush; 
            
            this_thread::sleep_for(chrono::milliseconds(50));
        }

        cout << endl
             << RED << "Powering off. Goodbye!" << RST << endl;
        cout << "\033[?25h";
    }

    (void)args;
    exit(EXIT_SUCCESS);
}


int cell_time(vector<char*> &args) {
    (void) args;
    
    time_t now = time(0);
    
    char* dt = ctime(&now);
    
    cout << G << " Time: " << Y << dt << RST;
    
    return 1; 
}

int cell_env(vector<char*> &args) {
    extern char **environ ; 
    if(!environ){
        return (1) ; 

    }
    for(int i = 0 ; environ[i]!=nullptr ; i++ ){
        cout << environ[i] << endl ; 
    }
    return (0); 
}