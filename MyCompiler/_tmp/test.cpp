#include <iostream>
using namespace std;

class Base
{
public:
  virtual void show() { cout << "call Base: " << m_array << endl; }

protected:
  Base(int size) :
  m_size(size)
  {
    cout << "call base construc" << endl;
    m_array = new int[m_size];
    cout << m_array << endl;
  }
  virtual ~Base() {
    cout << "call base dest" << endl;
    delete[] m_array;
  }

private:
  int *m_array;
  int m_size;
};

class A : public Base
{
public:
  A(int size) :
  Base(size),
  ma_size(size),
  m_array(new int[ma_size])
  {
    cout << "call A construc" << endl;
    cout << m_array << endl;
  }
  virtual ~A() {
    cout << "call A dest" << endl;
    delete[] m_array;
  }
  void show() { cout << "call A: " << m_array << endl; }

private:
  int* m_array;
  int ma_size;
};

void vir(Base &b)
{
  cout << "vir: " << endl;
  b.show();
}

int main() {
  A a(100);
  vir(a);
}
