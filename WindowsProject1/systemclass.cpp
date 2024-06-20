#include "systemclass.h"

SystemClass::SystemClass() {
	m_Input = 0;
	m_Application = 0;
}

SystemClass::SystemClass(const SystemClass& other) {}
SystemClass::~SystemClass() {}

bool SystemClass::Init() {
	int screenWidth = 0, screenHeight = 0;
	bool result;


	//makes and inits input object; used to handle keyboard input
	InitWindows(screenWidth, screenHeight);
	m_Input = new InputClass;
	m_Input->Init();

	//makes and inits app object; used to render graphics
	m_Application = new ApplicationClass;
	result = m_Application->Init(screenWidth, screenHeight, m_hwnd);

	return result;
}

void SystemClass::Shutdown() {
	//Release app object
	if (m_Application) {
		m_Application->Shutdown();
		delete m_Application;
		m_Application = 0;
	}

	//Release input object
	if (m_Input) {
		delete m_Input;
		m_Input = 0;
	}

	ShutdownWindows();
}

//Run: loop until quit
//Frame() called each iteration to process app
//pseudocode as follows:
//while running:
//	check for windows system messages
//	process system messages
//	process application loop
//	check for quit
void SystemClass::Run() {
	MSG msg;
	bool running = true, result;

	//Init message structure
	ZeroMemory(&msg, sizeof(MSG));

	//Main loop
	while (running) {
		//Handle windows messages
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//check for quit message
		if (msg.message == WM_QUIT) {
			running = false;
		}
		else {
			//otherwise process frame
			result = Frame();
			running = result; //stops loop if Frame() fails
		}
	}
}

bool SystemClass::Frame() {
	bool result;

	if (m_Input->IsKeyDown(VK_ESCAPE)) {
		return false;
	}
	
	result = m_Application->Frame();
	return result;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) {
	switch (umsg) {
		//check for key press to send to input object
		case WM_KEYDOWN: { 
			m_Input->KeyDown((unsigned int)wparam);
			return 0;
		}

		//check for key release send to input object to unset state
		case WM_KEYUP: {
			m_Input->KeyUp((unsigned int)wparam);
			return 0;
		}

		default: {
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

void SystemClass::InitWindows(int& screenWidth, int& screenHeight) {
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;


	// Get an external pointer to this object.	
	ApplicationHandle = this;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	m_applicationName = L"Engine";

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		screenWidth = 800;
		screenHeight = 600;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Hide the mouse cursor.
	ShowCursor(false);
}

void SystemClass::ShutdownWindows() {
	ShowCursor(true);
	if (FULL_SCREEN) ChangeDisplaySettings(NULL, 0);

	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	ApplicationHandle = NULL;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam) {
	switch (umessage){
		// Check if the window is being destroyed.
		case WM_DESTROY:{
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed.
		case WM_CLOSE:{
			PostQuitMessage(0);
			return 0;
		}

		// All other messages pass to the message handler in the system class.
		default:{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}