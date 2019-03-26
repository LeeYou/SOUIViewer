#pragma once
namespace Utils
{
	void DbgPrintf(LPCSTR lpFmt, ...);
	void DbgPrintf(LPCWSTR lpFmt, ...);
	std::string Format(const char *fmt, ...);
	std::wstring Format(const wchar_t *fmt, ...);
	// �ж��ļ��д���
	BOOL DirectoryExists(LPCTSTR lpDirectory);
};