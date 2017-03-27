/*************************************************************************
	> File Name: ngx_value_test.cpp
	> Author: 
	> Mail: 
	> Created Time: 2017年03月27日 星期一 13时17分18秒
 ************************************************************************/

#include "../ngx_value.hpp"

#include <iostream>
#include <cassert>

using namespace std;

int main(int argc, char* argv[])
{
    int x = 10;
    int y = 20;
    void *p;

    NgxValue::unset(x, y, p);
    assert(NgxValue::invalid(p));
}
