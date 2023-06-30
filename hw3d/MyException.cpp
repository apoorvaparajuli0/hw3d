#include "MyException.h"

/*
* @param line
* @param file
*/
MyException::MyException(int line, const char* file) noexcept
	:
	line(line),
	file(file)
{}

/*
*
*/
const char* MyException::what() const noexcept
{
	std::ostringstream oss;

	oss << GetType() << std::endl
		<< GetOriginString();

	whatBuffer = oss.str();

	//we want to return a ptr to the buffer
	//that lives past the end of this function
	//since if we just return oss.c_str() then
	//the pointer would be pointing to memory
	//that's already been destroyed
	return whatBuffer.c_str();
}

/*
*
*/
const char* MyException::GetType() const noexcept
{
	return "My Exception";
}

/*
*
*/
const std::string& MyException::GetFile() const noexcept
{
	return file;
}

/*
*
*/
std::string MyException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[FILE]: " << file << std::endl
		<< "[LINE]: " << line;

	return oss.str();
}