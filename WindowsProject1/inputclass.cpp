#include "inputclass.h"

InputClass::InputClass() {}
InputClass::InputClass(const InputClass& other) {}
InputClass::~InputClass() {}

void InputClass::Init() {
	for (int i = 0; i < KEYCONST; ++i) {
		m_keys[i] = false;
	}
}

void InputClass::KeyDown(unsigned int input) {
	m_keys[input] = true;
}

void InputClass::KeyUp(unsigned int input) {
	m_keys[input] = false;
}

bool InputClass::IsKeyDown(unsigned int key) {
	return m_keys[key];
}