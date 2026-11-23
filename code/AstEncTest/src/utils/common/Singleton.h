#pragma once
#include <memory>

/*
	单列类模版-智能指针
	注意：多线程下没有锁，并发调用 GetInstance不是线程安全
		如果手动调用DelInstance()，s_sp.reset()清空，再次调用GetInstance，不会重新创建实例！
*/
template<class T>
class Singleton
{
public:
	static T* GetInstance()
	{
		static bool b = !!(s_sp = (std::shared_ptr<T>(new T)));
		return s_sp.get();
	}
	static void DelInstance()
	{// 需要中途销毁时，调用它
		s_sp.reset();
	}
protected:
	static std::shared_ptr<T> s_sp;
};

// 类模板里面的静态成员，如果在头文件里写定义，这个头被多个.cpp include
// 每个编译单元都会生成一份这个变量，链接的时候报multiple definition（多重定义）链接错误。
//_declspec(selectany)是MSVC微软扩展(GCC也兼容这个属性)
// 告诉链接器：这个全局变量允许出现多份一模一样的定义；链接的时候随便挑其中一份，其余全部丢弃，不报重定义错误
// 注意：selectany是编译器扩展，不是标准 C++，在别的编译器（某些嵌入式编译器）不一定能用；新项目优先inline static
template<class T>
__declspec(selectany)std::shared_ptr<T> Singleton<T>::s_sp;// 定义

// 注意：宏末尾带分号，虽然现在看着方便，如果嵌套使用容易引发诡异语法问题
#define GetSingleton(class_name) class_name::GetInstance()
#define DelSingleton(class_name) class_name::DelInstance();
#define GET(class_name) class_name::GetInstance()
#define DET(class_name) class_name::DelInstance();


/*
	单列类模版-静态存储区
	注：不支持手动销毁，程序退出自动析构
	C++11 标准规定：如果多个线程同时第一次执行这个静态局部变量初始化，编译器保证初始化只会执行一次，自带同步。
*/
template<class T>
class StaticSingleton
{
public:
	static T* Instance()
	{
		// static std::shared_ptr<T> s_ins(new T);
		// return s_ins.get();
		static T s_ins;// 静态存储区
		return &s_ins;
	}
};

#define SSLT(class_name) class_name::Instance()