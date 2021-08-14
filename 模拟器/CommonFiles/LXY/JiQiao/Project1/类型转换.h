#pragma once
#include <iostream>
#include <string>


class TestClass
{
	/*��Ԫ�����Լ�using����*/
private:
	/*˽�����ݳ�Ա*/

public:
	/*�������ݳ�Ա*/
	std::string strTest;
public:
	/*���캯������������*/
	TestClass(std::string str) :strTest(str) {
	}
	~TestClass() {
	}
public:
	/*��������*/
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
	/*�����ǵ������ܺ���*/
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