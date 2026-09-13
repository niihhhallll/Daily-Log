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
                std::cout << a + 10 << std::endl;
                return 0;
            }
            Draw& operator<<(int&& rhs)
            {
                std::cout << rhs << std::endl;
                return *this;
            }
    };
}
int main()
{
    Render::Draw A(10);
    A.Init();
    A << 10;
    return 0;
}
