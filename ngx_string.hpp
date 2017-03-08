#pragma once

#include "nginx.hpp"
#include "ngx_wrapper.hpp"

#include <boost/utility/string_ref.hpp>

class NgxString final : public NgxWrapper<ngx_str_t>
{
public:
	typedef NgxWrapper<ngx_str_t> super_type;
	typedef NgxString this_type;

	typedef boost::string_ref string_ref_type;
public:
	NgxString(ngx_str_t& str) :super_type(str)
	{}

	~NgxString() = default;
public:
	const char* data() const
	{
		return reinterpret_cast<const char*>(get()->data);
	}

	std::size_t size() const
	{
		return get()->len;
	}

	bool empty() const
	{
		return !get()->data || !get()->len;
	}

	string_ref_type str() const
	{
		return string_ref_type(data(), size());
	}

	//自己补充begin、end、front、back

public:
	operator ngx_int_t() const
	{
		return ngx_atoi(get()->data, get()->len);
	}

	//重载比较操作符，大小写敏感比较两个ngx_str_t对象
	friend bool operator==(const this_type& l, const this_type& r)
	{
		return l.size() == r.size() && ngx_strncmp(l.data, r.data(), l.size()) == 0;
	}

	template <typename ... Args>
	void printf(const Args& ... args) const
	{
		//没有调用成员函数data()，这是因为data()使用了reinterpret_cast强制转型为const char*,而Nginx的字符串需要的是u_char*
		auto p = ngx_snprintf(get()->data, get()->len, args ...);

		get()->len = static_cast<std::size_t>(p - get()->data);
	}
public:
	//重载C++流输出操作符
	template <typename T>
	friend T& operator<<(T& o, const this_type& s)
	{
		o.write(s.data(), s.size());
		return o;
	}
};
