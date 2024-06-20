#pragma once

class InputClass {
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	void Init();
	void KeyDown(unsigned int);
	void KeyUp(unsigned int);

	bool IsKeyDown(unsigned int);

private:
	const static int KEYCONST = 256;
	bool m_keys[KEYCONST];
};