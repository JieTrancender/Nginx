#pragma once

#include "ngx_wrapper.hpp"
#include "ngx_exception.hpp"

#include <ngx_core.h>
#include <ngx_palloc.h>

//#include <winsock.h>
typedef unsigned char   u_char;

class NgxPool final : public NgxWrapper<ngx_pool_t>
{
public:
	typedef NgxWrapper<ngx_pool_t> super_type;
	typedef NgxPool this_type;
public:
	NgxPool(ngx_pool_t* p) :super_type(p)
	{}

	~NgxPool() = default;

	//ngx_pool_t再狠多Nginx的数据结构里以成员变量pool的形式出现，所以增加一个模板构造函数自动使用这个成员
	//末班参数T可以是ngx_http_request_t、ngx_conf_t等含有pool成员的结构
	template <typename T>
	NgxPool(T* x) :NgxPool(x->pool)
	{}
public:
	//ngx_pool_t相关的内存分配与释放函数
	//void* ngx_palloc(ngx_pool_t *pool, size_t size);
	//void* ngx_pnalloc(ngx_pool_t *pool, size_t size);
	//void* ngx_pcalloc(ngx_poll_t *poll, size_t size);
	//ngx_int_t ngx_pfree(ngx_pool_t *pool, void *p);

	//三个内存申请函数的差别
	//ngx_palloc使用了内存对齐，但可能会有少量的内存浪费
	//ngx_pnalloc没有使用内存对齐
	//ngx_pcalloc内部调用了ngx_palloc，并且把内存块清零
	//大多数情况下我们应该使用ngx_pcalloc，更加安全
	template <typename T, bool no_exception = false>
	T* palloc() const
	{
		auto p = ngx_pcalloc(get(), sizeof(T));

		if (!p)
		{
			//是否允许抛出异常
			if (no_exception)
			{
				return nullptr;
			}

			NgxException::raise();
		}

	    //断言增强安全性
		assert(p);
		return new (p) T();
	}

    //抛出异常版本
	template <typename T>
	T* alloc() const
	{
		return palloc<T, false>();
	}

	//不抛出异常版本
	template <typename T>
	T* alloc_noexcept() const
	{
		return palloc<T, true>();
	}

	//分配指定字节数的内存
	template <typename T>
	T* nalloc(std::size_t n) const
	{
		auto p = ngx_pnalloc(get(), n);

		//检查空指针
		NgxException::require(p);

		return reinterpret_cast<T*>(p);
	}

	//多参数构造对象
	template <typename T, typename ... Args>
	T* construct(const Args& ... args) const
	{
		auto p = ngx_pcalloc(get(), sizeof(T));

		NgxException::require(p);

		//转发参数构造
		return new (p) T(args ...);
	}

	//对ngx_pool_cleanup_add()的封装
	//Nginx里释放资源的基本流程
	//1. 获取一个ngx_pool_cleanup_t对象
	//2. 设置它的handler和data
	//3. 如果不提供data对象(nullptr)，那么用户就需要手工设置
	template <typename F, typename T>
	ngx_pool_cleanup_t* cleanup(F func, T* data, std::size_t size = 0) const
	{
		auto p = ngx_poll_cleanup_add(get(), size);

		NgxException::require(p);

		//设置清理函数
		p->handler = func;

		if (data)
		{
			p->data = data;
		}

		//返回清理信息对象
		return p;
	}

	//如果我们自己的类用到了其他资源，就必须把类的析构函数适配到Nginx的清理机制里
	template <typename T>
	static void destory(void *p)
	{
		//转换void*，再调用析构函数
		(reinterpret_cast<T*>(p))->~T();
	}
	
	//重载cleanup
	//函数指针指明释放内存时所调用的函数为对应对象的析构函数
	template <typename T>
	void cleanup(T* data) const
	{
		cleanup(&this_type::destory<T>, data);
	}

	//字符串复制
	ngx_str_t dup(ngx_str_t& str) const
	{
		ngx_str_t  temp;

		temp.len = str.len;
		temp.data = ngx_pstrdup(get(), &str);

		NgxException::require(temp.data);
		return temp;
	}

	//复制普通字符串
	ngx_str_t dup(boost::string_ref str) const
	{
		ngx_str_t temp{ str.size(), (u_char*)str.data() };
		return dup(temp);
	}
};
