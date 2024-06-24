#ifndef HTTP_UTILS_HPP
# define HTTP_UTILS_HPP

#include <string>

enum e_fileStatus{FILE_REG = 0, FILE_DIR, FILE_NOT, FILE_ELS};

template <typename T>
std::string NumberToString ( T Number );

int	fileStatus(const std::string & path);
int	getFileSize(const std::string & path);



#endif
