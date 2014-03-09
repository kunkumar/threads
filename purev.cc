#include <iostream>

using namespace std;

class A
{
public:
   // this is stil a pure virtual function
   // when there is a definition
   virtual ~A() = 0;
};

A::~A()
{
  cout<<"In Destructor\n";
}

class B: public A
{};

int main()
{
   // fail to link due to missing definition of A::~A()
   B b;
}
