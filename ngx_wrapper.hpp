#pragma once

#include <type_traits>

//#include <boost/type_traits.hpp>

template <typename T>
class NgxWrapper
{
public:
	//typedef typename boost::remove_pointer<T>::type wrapped_type;
	typedef typename std::remove_pointer<T>::type wrapped_type;

	typedef wrapped_type* pointer_type;
	typedef wrapped_type& reference_type;
private:
	pointer_type m_ptr = nullptr;
protected:
	NgxWrapper(pointer_type p) :m_ptr(p)
	{}

	NgxWrapper(reference_type x) :m_ptr(&X)
	{}

	~NgxWraper() = default;
public:
	pointer_type get() const
	{
		return m_ptr;
	}

	operator bool() const
	{
		return get();
	}

	operator pointer_type() const
	{
		return get();
	}

	//¿¿¿¿¿¿¿
	ponter_type operator->() const
	{
		return get();
	}
};
