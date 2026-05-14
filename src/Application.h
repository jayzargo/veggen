#pragma once

class Application
{
public:
	Application();
	~Application();

	bool Init(int width, int height, const char* title);
	void Run();
	void Shutdown();

private:


};
