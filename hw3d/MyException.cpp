#include "MyException.h"

/*
* just our constructor for the basic exception class
* 
* @param line -the line number the error occurred on
* @param file -the file the error occurred in
*/
MyException::MyException(int line, const char* file) noexcept
	:
	line(line),
	file(file)
{}

/*
* gets the error type and the string we use to output
* the complete error message and puts the into an output
* string stream and then stores them in our buffer
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
* just returns "MyException" so we know that we're dealing with
* our custom exception class
*/
const char* MyException::GetType() const noexcept
{
	return "My Exception";
}

/*
* returns the file that the exception occurred in (initialized in ctor)
*/
const std::string& MyException::GetFile() const noexcept
{
	return file;
}

/*
* returns a stringified output string stream containing the
* error message info
*/
std::string MyException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[FILE]: " << file << std::endl
		<< "[LINE]: " << line;

	return oss.str();
}