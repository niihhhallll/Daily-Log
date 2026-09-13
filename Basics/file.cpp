#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <sys/stat.h>

/*
 *
 *  TO-DO
 * // file name shall be in a constructor or in a init function ?
 * OPEN A FILE
 * function open(); with name of the file
 * READ A FILE
 * function read(int fd,void *buf,size_t nbyte);
 * WRITE TO A FILE
 *
 * CLOSE A FILE
 * this must be in a destructor;
 */

namespace File {
class Console {
private:
  int fd = 0;

public:
  // constructor;
  Console(){};
  int Init(const std::string& FileName) {
    try {
      this->fd =
          open(FileName.c_str(), O_RDWR | O_CREAT | O_APPEND,0644);
      if (this->fd < 0) {
        throw std::runtime_error("Cannot Open the file.");
      }
      std::cout << "Init Completed." << std::endl;
      return 0;
    } catch (std::exception &e) {
      std::cout << e.what() << std::endl;
      return 0;
    }
  }
  int Write(const std::string& Data)
  {
      try {
      const char* data = Data.data();
      ssize_t bytes = write(this->fd,data,Data.length());
      if(bytes == -1)
          {
             throw std::runtime_error("Write Error");
          }
      return 0;
      }
      catch (std::exception& e)
      {
          std::cout << e.what() << std::endl;
          return 1;
      }
  }
/*
 * The user can use the operator or the Init method;
 */
  // operator overloading for Easier Access;
  void operator<<(const std::string& FileName)
  {
      int ReturnValue = Init(FileName);
      if(ReturnValue != 0)
          {
              std::cerr << "Something Went Wrong In Init The File." << std::endl;
          }
      return;
  }
  void operator>>(const std::string& Data)
  {
      int ReturnValue = Write(Data);
      if(ReturnValue != 0)
      {
          std::cerr << "Something Went Wrong In Writing." << std::endl;
      }
      return;
  }
  // destructor
  ~Console()
  {
      if(this->fd != 0)
      {
          close(this->fd);
      }
  }
};
} // namespace File

int main()
{
    File::Console file{};
    file << "Hello.txt";
    file.Write("hello this is nihal.");
    file >> "this is from a operator";
    return 0;
}
