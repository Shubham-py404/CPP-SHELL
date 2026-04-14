#include "cell.hpp"

int cell_exit(vector<char *> &args)
{

    {
        cout << "\033[?25l";

        cout << Y << "Initiating shutdown sequence..." << RST << endl;

        // 2. The Progress Bar Loop
        int bar_width = 15;
        for (int i = 0; i <= bar_width; ++i)
        {
            // \r moves the cursor back to the start of the line
            cout << "\r[";

            // Draw the filled part of the bar
            for (int j = 0; j < bar_width; ++j)
            {
                if (j < i)
                {
                    cout << G << "=" << RST; // Green filled area
                }
                else if (j == i)
                {
                    cout << C << ">" << RST; // Cyan leading arrow
                }
                else
                {
                    cout << " "; // Empty space
                }
            }

            // Calculate percentage and print it
            int percent = (i * 100) / bar_width;
            cout << "] " << percent << "% " << flush; // flush forces the screen to update!

            // Pause for 50 milliseconds before drawing the next frame
            this_thread::sleep_for(chrono::milliseconds(50));
        }

        // 3. Print the final goodbye and restore the cursor
        cout << endl
             << RED << "Powering off. Goodbye!" << RST << endl;
        cout << "\033[?25h";
    }

    (void)args;
    exit(EXIT_SUCCESS);
}


int cell_time(vector<char*> &args) {
    (void) args; // We don't need any arguments for this command
    
    // 1. Get the current time from the operating system
    time_t now = time(0);
    
    // 2. Convert it into a readable human string
    char* dt = ctime(&now);
    
    // 3. Print it out! 
    // Note: ctime() automatically adds a newline (\n) at the end of the string
    cout << G << "⌚ Current System Time: " << Y << dt << RST;
    
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