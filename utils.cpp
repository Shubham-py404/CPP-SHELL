#include "cell.hpp"

void Getcwd(char *buf, size_t size) {
  if (NULL == getcwd(buf, size))
    perror(RED "getcwd failed" RST);
}

void PB1() {
  p(G R"(     _______. __    __   _______  __       __         
    /       ||  |  |  | |   ____||  |     |  |        
   |   (----`|  |__|  | |  |__   |  |     |  |        
    \   \    |   __   | |   __|  |  |     |  |        
.----)   |   |  |  |  | |  |____ |  `----.|  `----.   
|_______/    |__|  |__| |_______||_______||_______|   
                                                   )"
    << endl
    << RST);
}

void PB2() {
  p(G R"(███████╗██╗  ██╗███████╗██╗     ██╗     
██╔════╝██║  ██║██╔════╝██║     ██║     
███████╗███████║█████╗  ██║     ██║     
╚════██║██╔══██║██╔══╝  ██║     ██║     
███████║██║  ██║███████╗███████╗███████╗
╚══════╝╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝
                                        )"
    << endl
    << RST);
}

// 1. Enforce static C-linkage and strict void* casting
static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                            void *userp) {
  if (userp != nullptr) {
    ((std::string *)userp)->append((char *)contents, size * nmemb);
  }
  return size * nmemb;
}

// Function to securely transmit the prompt to the Gemini API
string call_gemini_api(const string &prompt, const string &api_key) {
  CURL *curl;
  CURLcode res;
  string readBuffer = ""; // Explicitly initialize an empty buffer

  curl = curl_easy_init();
  if (curl) {
    string url = "https://generativelanguage.googleapis.com/v1beta/models/"
                 "gemini-3.5-flash-lite:generateContent";
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 7L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&readBuffer);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    string auth_header = "x-goog-api-key: " + api_key;
    headers = curl_slist_append(headers, auth_header.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    json payload;
    payload["contents"][0]["parts"][0]["text"] = prompt;

    string json_payload = payload.dump();
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload.c_str());

    res = curl_easy_perform(curl);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
      return "Error: Faild to connect to AI API. (check your internet "
             "connection or API key)";
    }

    try {
      if (readBuffer.empty()) {
        return "Error: HTTP response buffer is empty. Curl did not capture "
               "data.";
      }

      json response = json::parse(readBuffer);
      if (response.contains("candidates") &&
          response["candidates"].size() > 0) {
        return response["candidates"][0]["content"]["parts"][0]["text"]
            .get<string>();
      } else if (response.contains("error")) {
        return "API Error: " + response["error"]["message"].get<string>();
      }

      return "Error: Unexpected API response format.";

    } catch (const std::exception &e) {
      return "JSON Parse Error: " + string(e.what());
    }
  }
  return "Error: Could not initialize libcurl.";
}

// Function to parse a .env file and load variables into the process
void load_env_file() {
  ifstream file(".env");
  string line;

  // If the file doesn't exist, just return silently.
  // The shell will still run, but API calls will fail later if the key is
  // missing.
  if (!file.is_open())
    return;

  while (getline(file, line)) {
    // Skip empty lines and comments
    if (line.empty() || line[0] == '#')
      continue;
    if (!line.empty() && line.back() == '\r')
      line.pop_back();
    size_t delimiter_pos = line.find('=');
    if (delimiter_pos != string::npos) {
      string key = line.substr(0, delimiter_pos);
      string value = line.substr(delimiter_pos + 1);

      // Strip double quotes from the value if the user added them
      if (value.length() >= 2 && value.front() == '"' && value.back() == '"') {
        value = value.substr(1, value.length() - 2);
      }
      // Strip single quotes
      else if (value.length() >= 2 && value.front() == '\'' &&
               value.back() == '\'') {
        value = value.substr(1, value.length() - 2);
      }

      // Inject the key-value pair into the C++ environment (1 = overwrite
      // existing)
      setenv(key.c_str(), value.c_str(), 1);
    }
  }
}
struct termios orig_termios;

// Restore the terminal to normal mode
void disableRawMode() { tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); }

// Switch the terminal to raw mode (disables Enter-key buffering and automatic
// echoing)
void enableRawMode() {
  tcgetattr(STDIN_FILENO, &orig_termios);
  struct termios raw = orig_termios;
  // Disable canonical mode (ICANON) and local echo (ECHO)
  raw.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

// Keep track of successful commands during this session
vector<string> session_history;

void add_to_history(const string &cmd) {
  if (!cmd.empty() &&
      (session_history.empty() || session_history.back() != cmd)) {
    session_history.push_back(cmd);
  }
}

// Silently execute a system command and return its stdout
string get_system_context(const char *cmd) {
  array<char, 128> buffer;
  string result;

  // Explicitly define the function pointer type to silence the attribute
  // warning
  unique_ptr<FILE, int (*)(FILE *)> pipe(popen(cmd, "r"), pclose);

  if (!pipe)
    return "";
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  if (!result.empty() && result.back() == '\n')
    result.pop_back();
  return result;
}

// True Context-Aware AI Auto-Complete
string get_ai_autocomplete(const string &input) {
  if (input.empty())
    return "";
  const char *api_key_env = getenv("GEMINI_API_KEY");
  if (api_key_env == nullptr)
    return "";
  string api_key(api_key_env);

  // Capture real-time environment context
  string files = get_system_context("ls -1 | head -n 10");
  string branch = get_system_context("git branch --show-current 2>/dev/null");
  string history_context = "";
  for (size_t i = (session_history.size() > 5 ? session_history.size() - 5 : 0);
       i < session_history.size(); ++i) {
    history_context += session_history[i] + "\n";
  }

  // Build a highly specific prompt
  string prompt = "You are a Linux autocomplete AI. Output ONLY the remaining "
                  "characters to complete the user's command. Do not repeat "
                  "the characters they already typed. No markdown, quotes, or "
                  "explanation. \nContext:\nFiles:\n" +
                  files + "\nGit Branch: " + branch + "\nRecent Commands:\n" +
                  history_context + "\nUser Input: " + input;

  string response = call_gemini_api(prompt, api_key);

  size_t pos;
  while ((pos = response.find("```bash")) != string::npos)
    response.replace(pos, 7, "");
  while ((pos = response.find("```sh")) != string::npos)
    response.replace(pos, 5, "");
  while ((pos = response.find("```")) != string::npos)
    response.replace(pos, 3, "");
  while (!response.empty() &&
         (response.back() == '\n' || response.back() == '\r' ||
          response.back() == ' '))
    response.pop_back();
  while (!response.empty() &&
         (response.front() == '\n' || response.front() == '\r' ||
          response.front() == ' '))
    response.erase(0, 1);

  if (response.find("Error:") != string::npos)
    return "";
  return response;
}