#pragma once
#include <assert.h>
#include <memory>
#include "dll_definitions.h"

struct BASIC_DLL_EXPORT IArguments {
	virtual ~IArguments() { }
};

template< typename Arg> \
class CArgumentsOne : public IArguments{
public:
	Arg arg; 

	CArgumentsOne() noexcept{}
	CArgumentsOne(Arg arg_) : arg(arg_) { }
	virtual ~CArgumentsOne(){ }
};

template< typename Arg1, typename Arg2> \
class CArgumentsTwo : public IArguments{
public:
	Arg1 arg1; 
	Arg2 arg2;

	CArgumentsTwo() noexcept {}
	CArgumentsTwo(Arg1 arg1_, Arg2 arg2_) : arg1(arg1_), arg2(arg2_) { }
	virtual ~CArgumentsTwo(){ }
};

struct IContainer {
	virtual void Call(IArguments*) = 0;
	virtual ~IContainer(){ }
};

template< class Class, typename Func > \
class CContainer : public IContainer {
public:
	virtual ~CContainer(){ }
};

template <class Class> \
class CContainer<Class, void (Class::*)(void) > : public IContainer{
	typedef void (Class::*PFunc)(void);

	Class* p_class;
	PFunc p_func;
public: 
	CContainer(Class* c, PFunc f) noexcept : p_class(c), p_func(f) {
	
		assert(c);
		assert(f);
	}
	void Call(IArguments* no_args){
	
		(p_class->*p_func)();
	}
	virtual ~CContainer() { }
};

template <class Class, typename Arg> \
class CContainer<Class, void (Class::*)(Arg) > : public IContainer{
	typedef void (Class::*PFunc)(Arg);
	typedef CArgumentsOne<Arg> TArguments;

	Class* p_class;
	PFunc p_func;
public:
	CContainer(Class* c, PFunc f) noexcept : p_class(c), p_func(f) {
	
		assert(c);
		assert(f);
	}
	void Call(IArguments* args){
		
		TArguments* a = dynamic_cast< TArguments* >(args);
		assert(a);
		if (a) (p_class->*p_func)(a->arg);
	}
	virtual ~CContainer() { }
};

template <class Class, typename Arg1, typename Arg2> \
class CContainer<Class, void (Class::*)(Arg1, Arg2) > : public IContainer{
	typedef void (Class::*PFunc)(Arg1, Arg2);
	typedef CArgumentsTwo<Arg1, Arg2> TArguments;

	Class* p_class;
	PFunc p_func;
public:
	CContainer(Class* c, PFunc f) noexcept : p_class(c), p_func(f) {
	
		assert(c);
		assert(f);
	}
	void Call(IArguments* args){
		
		TArguments* a = dynamic_cast< TArguments* >(args);
		assert(a);
		if (a) (p_class->*p_func)(a->arg1, a->arg2);
	}
	virtual ~CContainer() { }
};

template <class Callable> \
class CContainerSimple : public IContainer {
	Callable callable;
public:
	CContainerSimple(Callable f) noexcept : callable(f) { }
	void Call(IArguments* no_args) {

		callable();
	}
	virtual ~CContainerSimple() { }
};

class BASIC_DLL_EXPORT CDelegate{
	std::shared_ptr<IContainer> container;
public:
	CDelegate() noexcept : container() {}

	CDelegate(const CDelegate &obj) noexcept = default;
	CDelegate(CDelegate &&obj) noexcept = default;
	CDelegate &operator=(const CDelegate &obj) noexcept = default;
	CDelegate &operator=(CDelegate &&obj) noexcept = default;
	
	template< class Class, typename Func > \
		void Connect(Class* p_class, Func p_func) noexcept{

		assert(p_class);
		assert(p_func != nullptr);
		if (container) container.reset();
		container = std::make_shared<CContainer< Class, Func > >(p_class, p_func);
	}

	template< typename Callable > \
		void Connect(Callable callable) noexcept {

		if (container) container.reset();
		container = std::make_shared<CContainerSimple< Callable > >(callable);
	}

	inline bool empty() const noexcept {

		return container == nullptr;
	}

	inline void operator()(){
		IArguments ignored;

		container->Call(&ignored);
	}

	template< typename Arg > \
		inline void operator()(Arg arg){
		CArgumentsOne<Arg> wrapped_arg(arg);

		container->Call(&wrapped_arg);
	}

	template< typename Arg1, typename Arg2 > \
		inline void operator()(Arg1 arg1, Arg2 arg2){
		CArgumentsTwo<Arg1, Arg2> wrapped_arg(arg1, arg2);

		container->Call(&wrapped_arg);
	}

	inline void Call(IArguments *args) {

		container->Call(args);
	}

	inline void reset() {

		container.reset();
	}

	virtual ~CDelegate() {

		reset();
	}
};
