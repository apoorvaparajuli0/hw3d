#pragma once
#include <exception>
#include <string>
#include <sstream>


class MyException : public std::exception
{
public:
	MyException(int line, const char* file) noexcept;
	const char* what() const noexcept override;
	virtual const char* GetType() const noexcept;
	int GetLine() const noexcept;
	const std::string& GetFile() const noexcept;
	std::string GetOriginString() const noexcept;

private:
	//line of wherever exception is thrown
	int line;
	//file where exception was thrown
	std::string file;
protected:
	//our whatBuffer which stores the string returned by what()
	//since the std::exception "what" returns a const char* and 
	//the stack memory would be deleted by the time the ptr returns
	//we want to store the const char* in our own member unencapsulated
	//by a function
	//(I think we make it mutable since if we directly copy a const char* onto a
	// std::string, the string might be implicitly const-ified)
	mutable std::string whatBuffer;
};