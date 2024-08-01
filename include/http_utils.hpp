#ifndef HTTP_UTILS_HPP
# define HTTP_UTILS_HPP

#include <string>

#define DEBUG 0

class Client;
enum e_fileStatus{FILE_REG = 0, FILE_DIR, FILE_NOT, FILE_ELS};

template <typename T>
std::string NumberToString ( T Number );

std::string to_hex(size_t value);
int sendStr(int clientSocket, std::string const &str); 
int sendChunk(int clientSocket, std::string const &chunk);
int	fileStatus(const std::string & path);
int	getFileSize(const std::string & path);
std::string get_current_date();
std::string sizeToStr( std::string::size_type size);
void logStep(std::string const &s, int n);
void logItem(std::string const &s, std::string const &item) ;
void logItem(std::string const &s, int const &item) ;
int logError(std::string const &s, int const &status) ;
std::string errPage(Client &client, size_t error_code);
std::string getResponse(short status);
std::string trim_copy(std::string s) ;
std::string formatHeader(const std::string& input);

#endif
