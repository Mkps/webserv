#ifndef HTTP_UTILS_HPP
# define HTTP_UTILS_HPP

#include <string>

#define DEBUG 1

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



#endif
