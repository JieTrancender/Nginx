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
	//��C++11��׼�Զ����ڴ������ʱֻ��Ҫʵ��allocate��deallocate������������
	pointer allocate(size_type n)
	{
		//ת��ʹ��Nginx�ڴ��
		return reinpterpret_cast<pointer>(ngx_pnalloc(get(), n * sizeof(T)));
	}

	//��Nginx�����ڴ����
	void deallocate(pointer ptr, size_type n)
	{
		boost::ignore_unused(n);
		ngx_pfree(get(), ptr);
	}
};

//test
//ʹ��ģ���������������������
//template <typename T>
//using NgxStdVector = std::vector<T, NgxAlloctor<T>>;

//NgxStdVector<char> v(pool);