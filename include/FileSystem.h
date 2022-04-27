#ifndef FILESYSTEM_H
#define FILESYSTEM_H


#include <string>
#include <cstdlib>
#include <filesystem>
//#include "root_directory.h" // This is a configuration file generated by CMake.

class FileSystem
{
private:
  typedef std::string (*Builder) (const std::string& path);

public:
  static std::string getPath(const std::string& path)
  {
    string tmppath = path;
    for (int pos = 0; pos != string::npos; pos += 1) if ((pos = tmppath.find("\\", pos)) != string::npos) tmppath.replace(pos, 2, "/"); else break;
    return getResourcePath(tmppath);
  }

private:
    static std::string getResourcePath(const std::string& path)
    {
#ifdef _WIN64 //Assume cwd is in ProjectDir/build
        return "../" + path;
#elif _APPLE_ //Assume cwd is in ProjectDir/cmake-debug-build
        return "../" + path;
#endif 
    }
};

// FILESYSTEM_H
#endif