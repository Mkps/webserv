#include "http_utils.hpp"

#include <map>
#include <sstream>
#include <sys/stat.h>
#include <iostream>

int	fileStatus(const std::string & path)
{
	struct stat buf;
	if (stat( path.c_str(), &buf) == 0)
	{
		if (buf.st_mode & S_IFDIR)
			return FILE_DIR;
		else if (buf.st_mode & S_IFREG)
			return FILE_REG;
		else
			return FILE_ELS;
	}
	else
		return FILE_NOT;
}

int	getFileSize(const std::string & path)
{
	struct stat buf;
	if (stat( path.c_str(), &buf) == 0)
		return buf.st_size;
	return FILE_NOT;
}

template <typename T>
std::string NumberToString ( T Number )
{
	std::ostringstream ss;
	ss << Number;
	return ss.str();
}

typedef std::map<std::string, std::string> hashmap;
void showHeaders(hashmap tmp)
{
	for (hashmap::const_iterator it = tmp.begin(); it != tmp.end(); ++it)
	{
		std::cout << "key " << it->first << " value " << it->second << std::endl;
	}
}
