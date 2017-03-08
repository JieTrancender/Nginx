#pragma once

#include "nginx.hpp"
#include "ngx_wrapper.hpp"

template <typename T>
class NgxAlloctor : public NgxWrapper<ngx_pool_t>
{
public:
	typedef NgxWrapper<ngx_pool_t> super_type;
	typedef NgxAlloctor this_type;
public:
	typedef std::size_t size_type;
	typedef T* pointer;
	typedef T value_type;
public:
	NgxAlloctor(ngx_pool_t *p) :super_type(p)
	{}

	~NgxAlloctor() = default;
public:
	//在C++11标准自定义内存分配器时只需要实现allocate和deallocate两个基本函数
	pointer allocate(size_type n)
	{
		//转型使用Nginx内存池
		return reinpterpret_cast<pointer>(ngx_pnalloc(get(), n * sizeof(T)));
	}

	//由Nginx负责内存回收
	void deallocate(pointer ptr, size_type n)
	{
		boost::ignore_unused(n);
		ngx_pfree(get(), ptr);
	}
};

//test
//使用模板别名简化容器的类型声明
//template <typename T>
//using NgxStdVector = std::vector<T, NgxAlloctor<T>>;

//NgxStdVector<char> v(pool);