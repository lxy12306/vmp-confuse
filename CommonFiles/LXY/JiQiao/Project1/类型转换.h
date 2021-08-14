#pragma once
#include <iostream>
#include <string>


class TestClass
{
	/*友元声明以及using定义*/
private:
	/*私有数据成员*/

public:
	/*公有数据成员*/
	std::string strTest;
public:
	/*构造函数与析构函数*/
	TestClass(std::string str) :strTest(str) {
	}
	~TestClass() {
	}
public:
	/*导出函数*/
	void __fastcall e_Print(int type)
	{
		std::cout << strTest << std::endl;
	}

	/*
	TestClass tc("1312315423541");
	void * addr = LXY::union_cast<void*, void(__fastcall TestClass::*)(int type)>(&TestClass::e_Print);
	
	__asm
	{
		lea ecx, tc
		call addr
	};
	auto f = bind(&TestClass::e_Print, &tc,std::placeholders::_1);
	f(2);
	*/
	void e_Print2(int type)
	{
		std::cout << strTest << std::endl;
	}
private:
	/*其他非导出功能函数*/
};

namespace LXY
{
	template< typename dst_type, typename src_type>
	dst_type union_cast(src_type src)
	{
		union
		{
			dst_type d;
			src_type s;
		}u;
		u.s = src;
		return u.d;
	}



}