#pragma once

#include "nginx.hpp"
#include "ngx_unset_value.hpp"

class NgxValue final
{
public:
	NgxValue() = default;
	~NgxValue() = default;
public:
	template <typename T>
	static bool invalid(const T& v)
	{
		return v == static_cast<T>(NgxUnsetValue::get());
	}

	template <typename T, typename U>
	static void init(T& x, const U& v)
	{
		if (invalid(x))
		{
			x = v;
		}
	}

	template <typename T, typename U, typename V>
	static void merge(T& c, const U& p, const V& d)
	{
		if (invalid(c))
		{
			c = invalid(p) ? d : p;
		}
	}

	//����merge��ʵ��ngx_str_t��������ֵ����
	static void merge(ngx_str_t& c, const ngx_str_t& p, const ngx_str_t& d)
	{
		if (!c.data)
		{
			c = p.data ? p : d;
		}
	}

	//�ɱ������Ϊδ��ʼ��״̬
	template <typename T, typename ... Args>
	static void unset(T& v, Args& ... args)
	{
		v = NgxUnsetValue::get();
		unset(args...);
	}

	//�ݹ��սắ��
	static void unset()
	{}
};
