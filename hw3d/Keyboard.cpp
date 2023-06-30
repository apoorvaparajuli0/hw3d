#include "Keyboard.h"

/*
* indexes into keystates at the given keycode
* to determine if the key is being pressed.
*/
bool Keyboard::KeyIsPressed(unsigned char keycode) const noexcept
{
	return keystates[keycode];
}

/*
* reads the first "Event" off of the key buffer queue
* and returns it as long as the keybuffer has events on
* it
*/
Keyboard::Event Keyboard::ReadKey() noexcept
{
	if (keybuffer.size() > 0u)
	{
		Keyboard::Event e = keybuffer.front();
		keybuffer.pop();
		
		return e;
	}
	else 
	{
		return Keyboard::Event();
	}
}

/*
* returns whether our keyboard event buffer is
* empty or not
*/
bool Keyboard::KeyIsEmpty() const noexcept
{
	return keybuffer.empty();
}

/*
* reads a character off of the character queue,
* we treat characters separately from keyboard
* events, they aren't necessarily events, they could
* just be keyboard input into a text box
*/
char Keyboard::ReadChar() noexcept
{
	if (charbuffer.size() > 0u)
	{
		unsigned char charcode = charbuffer.front();
		charbuffer.pop();
		
		return charcode;
	}
	else
	{
		return 0;
	}
}

/*
* returns whether our charbuffer is empty
*/
bool Keyboard::CharIsEmpty() const noexcept
{
	return charbuffer.empty();
}

/*
* clears our keyboard event queue
*/
void Keyboard::FlushKey() noexcept
{
	keybuffer = std::queue<Event>();
}

/*
* clears our character queue
*/
void Keyboard::FlushChar() noexcept
{
	charbuffer = std::queue<char>();
}

/*
* flushes bothe keyboard event and
* character queues
*/
void Keyboard::Flush() noexcept
{
	FlushKey();
	FlushChar();
}

/*
* enables autorepeat for keyboard events
*/
void Keyboard::EnableAutorepeat() noexcept
{
	autorepeatEnabled = true;
}

/*
* disables autorepeat for keyboard events
*/
void Keyboard::DisableAutorepeat() noexcept
{
	autorepeatEnabled = false;
}

/*
* returns whether autorepeat is enabled or not
*/
bool Keyboard::AutorepeatIsEnabled() const noexcept
{
	return autorepeatEnabled;
}

/*
* indexes into our keystates bitset at the given keycode to set it's
* status to being pressed, we then push a keyboard event initialized with
* the appropriate enum type and keycode into our keyboard event queue and then
* we call TrimBuffer()
* 
* @param keycode -virtual keycode for unique keys as provided on MSDN docs
*/
void Keyboard::OnKeyPressed(unsigned char keycode) noexcept
{
	keystates[keycode] = true;
	keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Press, keycode));
	TrimBuffer(keybuffer);
}

/*
* same as the above but it sets our keystate to false and pushes a 
* "Released" enum type to the Event constructor
* 
* @param keycode -virtual keycode for a given keyboard event
*/
void Keyboard::OnKeyReleased(unsigned char keycode) noexcept
{
	keystates[keycode] = false;
	keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, keycode));
	TrimBuffer(keybuffer);
}

/*
* pushes a character onto our char buffer and trims
* 
* @param character -character that we want to push
*/
void Keyboard::OnChar(char character) noexcept
{
	charbuffer.push(character);
	TrimBuffer(charbuffer);
} 

/*
* clears our keystates to all be false, or 0
*/
void Keyboard::ClearState() noexcept
{
	keystates.reset();
}

/*
* trims down a buffer by one if they exceed the max buffer
* size as indicated by bufferSize
* 
* @param buffer -templated to take both char buffers and
*				 event buffers by reference
*/
template<typename T>
void Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept
{
	while (buffer.size() > bufferSize)
	{
		buffer.pop();
	}
}