#include <iostream>
#include <malloc.h>

using namespace std;

class A
{
int x;
public:
        A(int xx):x(xx)
 {
  printf("A::A()\n");
 }
};

/*const A& getA1()
{
 A a(5);
 return a;
}

A& getA2()
{
 A a(5);
 return a;
}*/

A getA3()
{
 A a(5);
 return a;
}

int func1()
{
    int i;
    i = 1;
    return i;
}
int main()
{ 
     //const A& newA1 = getA1(); 
     //A& newA2 = getA2();       
     const A& newA3 = getA3();   
      const int&  k = func1();    
     return 0;
}
