#include <iostream>

int Init()
{
    std::cout << "Init of discord has been started.." << std::endl;
    return 1;
}

namespace Numbers
{
    // this is a local namespace function
    int Init(int Num1,int Num2)
    {
        std::cout << Num1 + Num2 << std::endl;
        ::Init(); // this calls the global namespace function;
        return 1;
    }
}

// don't give namespace and class the same name;
// WARNING !!
namespace Render
{
    class Draw
    {
        private:
            int a{0};
        public:
            Draw(int a) : a(a){}
            // local namespace function
            int Init()
            {
                return 0;
            }
            friend std::ostream& operator>>(std::ostream& os,const Render::Draw& rhs)
            {
                os << rhs.a;
                return os;
            }
             Draw& operator<<(int&& rhs)
            {
                std::cout << rhs << std::endl;
                return *this;
            }
             Draw& operator=(int&& rhs)
            {
                std::cout << rhs  + this->a << std::endl;
                return *this;
            }
    };
}


int main()
{
    Render::Draw A(10);
    A = 10;
    A << 2010 << 10 << 100;
    return 0;
}
