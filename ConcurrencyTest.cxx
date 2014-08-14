#include <thread>
#include <iostream>

using namespace std;

int main()
{
  cout << "Hardware Concurrency " << std::thread::hardware_concurrency() 
       << endl;
}
