#include "cell.hpp"

int cell_cd(vector<char *> &args) {
  string target = "";

  // Properly expand tilde (~) into the absolute HOME path
  if (args.size() < 2 || args[1] == nullptr) {
    const char *home = getenv("HOME");
    if (home)
      target = home;
  } else {
    target = args[1];
    if (target.front() == '~') {
      const char *home = getenv("HOME");
      if (home)
        target.replace(0, 1, home);
    }
  }

  if (target.empty()) {
    cerr << "\033[1;31mcd: HOME not set\033[0m\n";
    return 1;
  }

  // Update OS directory, and if successful, update the PWD environment variable
  if (chdir(target.c_str()) != 0) {
    perror("cpp-shell: cd");
  } else {
    char cwd[BUFSIZ];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
      setenv("PWD", cwd, 1);
    }
  }
  return 1;
}

int cell_exit(vector<char *> &args) {

  int exit_code = EXIT_SUCCESS;
  if (args.size() > 1 && args[1] != nullptr) {
    exit_code = atoi(args[1]);
  }
  cout << "\033[?25l";

  cout << Y << "Initiating shutdown sequence..." << RST << endl;

  // 2. The Progress Bar Loop
  int bar_width = 15;
  for (int i = 0; i <= bar_width; ++i) {
    // \r moves the cursor back to the start of the line
    cout << "\r[";

    // Draw the filled part of the bar
    for (int j = 0; j < bar_width; ++j) {
      if (j < i) {
        cout << G << "=" << RST; // Green filled area
      } else if (j == i) {
        cout << C << ">" << RST; // Cyan leading arrow
      } else {
        cout << " "; // Empty space
      }
    }

    // Calculate percentage and print it
    int percent = (i * 100) / bar_width;
    cout << "] " << percent << "% "
         << flush; // flush forces the screen to update!

    // Pause for 50 milliseconds before drawing the next frame
    this_thread::sleep_for(chrono::milliseconds(50));
  }

  // 3. Print the final goodbye and restore the cursor
  cout << endl << RED << "Powering off. Goodbye!" << RST << endl;
  cout << "\033[?25h";

  exit(exit_code);

  int cell_env(vector<char *> & args) {
    (void)args; // Silence unused parameter warning
    extern char **environ;
    if (!environ)
      return 1;

    for (int i = 0; environ[i] != nullptr; i++) {
      cout << environ[i] << endl;
    }
    return 0;
  }

  int cell_time(vector<char *> & args) {
    (void)args;

    time_t now = time(0);

    char *dt = ctime(&now);

    cout << G << " Time: " << Y << dt << RST;

    return 1;
  }

  int cell_ai(vector<char *> & args) {
    // Check if the user typed a prompt after 'ai'
    if (args.size() < 2 || args[1] == nullptr) {
      cerr << "\033[1;31mError:\033[0m Expected a prompt. (Usage: ai <your "
              "question>)"
           << endl;
      return 1; // Return 1 to keep the REPL running
    }

    // Concatenate the string array back into a single natural language sentence
    string prompt = "";
    for (size_t i = 1; i < args.size() && args[i] != nullptr; i++) {
      prompt += string(args[i]) + " ";
    }

    // Securely retrieve the API key from the Linux environment variables
    const char *api_key_env = getenv("GEMINI_API_KEY");
    if (api_key_env == nullptr) {
      cerr << "\033[1;31mError:\033[0m GEMINI_API_KEY environment variable is "
              "not set."
           << endl;
      cerr << "Run this before starting the shell: export "
              "GEMINI_API_KEY=\"your_key_here\""
           << endl;
      return 1;
    }
    string api_key(api_key_env);

    // Provide immediate visual feedback while making the network request
    cout << "\033[1;90mThinking...\033[0m" << endl; // Dark gray text

    string response = call_gemini_api(prompt, api_key);

    // Print the AI response in distinct Magenta so it doesn't blend with system
    // output
    cout << "\033[1;35m[AI Assistant]:\033[0m \n" << response << "\n" << endl;

    return 1;
  }
  int cell_cmd(vector<char *> & args) {
    if (args.size() < 2 || args[1] == nullptr) {
      cerr << "\033[1;31mError:\033[0m Expected a description. (Usage: cmd "
              "<what "
              "you want to do>)"
           << endl;
      return 1;
    }

    string prompt = "";
    for (size_t i = 1; i < args.size() && args[i] != nullptr; i++) {
      prompt += string(args[i]) + " ";
    }

    const char *api_key_env = getenv("GEMINI_API_KEY");
    if (api_key_env == nullptr) {
      cerr << "\033[1;31mError:\033[0m GEMINI_API_KEY environment variable is "
              "not set."
           << endl;
      return 1;
    }
    string api_key(api_key_env);

    cout << "\033[1;90mGenerating command...\033[0m" << endl;

    // Strict prompt engineering to force Gemini to output ONLY the raw terminal
    // command
    string sys_instruct =
        "You are a strict Linux terminal. Translate the user's request into a "
        "single, valid Linux command. Output ONLY the raw command. Do not "
        "include markdown, backticks, or explanations. Request: ";

    string response = call_gemini_api(sys_instruct + prompt, api_key);

    // Cleanup: Strip accidental markdown backticks just in case the AI
    // hallucinates them
    size_t pos;
    while ((pos = response.find("```bash")) != string::npos)
      response.replace(pos, 7, "");
    while ((pos = response.find("```sh")) != string::npos)
      response.replace(pos, 5, "");
    while ((pos = response.find("```")) != string::npos)
      response.replace(pos, 3, "");

    // Trim leading and trailing newlines
    while (!response.empty() &&
           (response.back() == '\n' || response.back() == '\r' ||
            response.back() == ' '))
      response.pop_back();
    while (!response.empty() &&
           (response.front() == '\n' || response.front() == '\r' ||
            response.front() == ' '))
      response.erase(0, 1);

    // Print the generated command in Yellow and Green
    cout << "\n\033[1;33mSuggested Command:\033[0m \033[1;32m" << response
         << "\033[0m\n"
         << endl;

    cout << "Execute this command? (y/n): ";
    string choice;
    getline(cin, choice);

    if (choice == "y" || choice == "Y") {
      cout << endl;
      // Fix: Use system() so AI-generated pipelines (|) and redirects (>) work
      // perfectly. This completely bypasses the split_line tokenizer,
      // eliminating the memory leak.
      int ret = system(response.c_str());
      (void)ret; // Silence unused variable warning
    } else {
      cout << "\033[1;90mExecution cancelled.\033[0m" << endl;
    }

    return 1;
  }