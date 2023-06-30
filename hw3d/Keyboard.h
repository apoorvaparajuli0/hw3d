#pragma once
#include <queue>
#include <bitset>

class Keyboard
{
	//making a class a friend allows it to
	//access private member functions
	friend class Window;

public:
	class Event
	{
	public:
		//our keyboard event types
		enum class Type
		{
			Press,
			Release,
			Invalid
		};

	private:
		//type corresponds to the enum class above
		Type type;
		//code is the VK keycode of the keyboard event
		unsigned char code;

	public:
		Event()
			:
			type(Type::Invalid),
			code(0u)
		{}

		Event(Type type, unsigned char code) noexcept 
			:
			type(type),
			code(code)
		{}

		bool isPress() const noexcept
		{
			return type == Type::Press;
		}

		bool isRelease() const noexcept
		{
			return type == Type::Release;
		}

		bool isInvalid() const noexcept
		{
			return type == Type::Invalid;
		}

		unsigned char GetCode() const noexcept
		{
			return code;
		}
	};

public:
	Keyboard() = default;
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;

	bool KeyIsPressed(unsigned char keycode) const noexcept;
	Event ReadKey() noexcept;
	bool KeyIsEmpty() const noexcept;
	void FlushKey() noexcept;
	char ReadChar() noexcept;
	bool CharIsEmpty() const noexcept; 
	void FlushChar() noexcept;
	void Flush() noexcept;
	void EnableAutorepeat() noexcept;
	void DisableAutorepeat() noexcept;
	bool AutorepeatIsEnabled() const noexcept;

private:
	void OnKeyPressed(unsigned char keycode) noexcept;
	void OnKeyReleased(unsigned char keycode) noexcept;
	void OnChar(char character) noexcept;
	void ClearState() noexcept;

	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer) noexcept;

private:
	//the number of key events supported by the windows VK_Key API thingy
	static constexpr unsigned int nKeys = 256u;
	//the max size of either our character or keyboard event buffer
	static constexpr unsigned int bufferSize = 16u;
	//our autorepeat enabled/disabled tracker
	bool autorepeatEnabled = false;
	//bitset that indicates whether our VK events that we can use
	//to index into the bitset are happening or not
	std::bitset<nKeys> keystates;
	//queue of keyboard events
	std::queue<Event> keybuffer;
	//queue of character input
	std::queue<char> charbuffer;
};