#include "avz.h"

using namespace AvZ;
struct str {
    int a = 0;
    int b = [](int& temp) {
        return temp + 1;
    }(a);
};
void Script()
{
    str s1;
    s1.a = 0;
    ShowErrorNotInQueue("#", s1.b);
    s1.a = 1;
    ShowErrorNotInQueue("#", s1.b);
}