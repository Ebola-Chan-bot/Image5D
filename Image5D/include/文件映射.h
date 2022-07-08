#pragma once
#include <memory>
template<typename T>
concept 字符 = std::_Is_any_of_v<T, char, wchar_t>;
class Image5DAPI 文件映射
{
	const HANDLE 文件句柄;
	HANDLE 映射句柄;
	LPVOID i映射指针 = nullptr;
	LONGLONG i文件大小;
	文件映射(const char* 文件路径, bool 只读);
	文件映射(const wchar_t* 文件路径, bool 只读);
	文件映射(const char* 文件路径, LONGLONG 文件大小);
	文件映射(const wchar_t* 文件路径, LONGLONG 文件大小);
public:
	const bool 只读;
	//构造方法不会自动建立映射，需要手动设置映射指针，否则返回nullptr
	LPVOID 映射指针()const noexcept { return i映射指针; }
	//如果输入nullptr，将由系统自动分配映射指针
	Image5D异常 映射指针(LPVOID)noexcept;
	LONGLONG 文件大小()const noexcept { return i文件大小; }
	//如果当前映射指针不为nullptr，此方法可能会改变映射指针
	Image5D异常 文件大小(LONGLONG 大小)noexcept;
	//禁止拷贝，以免意外的析构
	文件映射(const 文件映射&) = delete;
	//请勿重复析构，会出错
	~文件映射()noexcept;
	//不能跨DLL抛异常，用静态方法包装构造函数
	template<字符 T>
	static Image5D异常 打开(const T* 文件路径, bool 只读, std::unique_ptr<文件映射>& 返回指针)noexcept;
	template<字符 T>
	static Image5D异常 创建(const T* 文件路径, LONGLONG 文件大小, std::unique_ptr<文件映射>& 返回指针)noexcept;
};
template Image5DAPI Image5D异常 文件映射::创建<char>(const char* 文件路径, LONGLONG 文件大小, std::unique_ptr<文件映射>& 返回指针)noexcept;
template Image5DAPI Image5D异常 文件映射::创建<wchar_t>(const wchar_t* 文件路径, LONGLONG 文件大小, std::unique_ptr<文件映射>& 返回指针)noexcept;
template Image5DAPI Image5D异常 文件映射::打开<char>(const char* 文件路径, bool 只读, std::unique_ptr<文件映射>& 返回指针)noexcept;
template Image5DAPI Image5D异常 文件映射::打开<wchar_t>(const wchar_t* 文件路径, bool 只读, std::unique_ptr<文件映射>& 返回指针)noexcept;