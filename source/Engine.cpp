#include "Engine.hpp"
#include "Log.h"
#include <GLFW/glfw3.h> // TODO: Encapsulation Leak (GLFW key_callback)

using namespace engine;

char const * Engine::Log = "LogEngine";
void* Engine::spInstance = nullptr;

void windowKeyInputCallback(Window* window, int key, int scancode, int action, int mods);
void inputQueueListener(input::Event const & evt);

Engine* Engine::Create()
{
	if (spInstance == nullptr)
	{
		spInstance = new Engine();
		return Engine::Get();
	}
	return nullptr;
}

Engine * Engine::Get()
{
	return (Engine*)spInstance;
}

bool Engine::GetChecked(Engine *& instance)
{
	instance = Engine::Get();
	return instance != nullptr;
}

void Engine::Destroy()
{
	if (spInstance != nullptr)
	{
		delete spInstance;
		spInstance = nullptr;
	}
}

Engine::Engine()
{
	*mpInputQueue = input::Queue(&inputQueueListener);
}

Engine::~Engine()
{
	this->destroyWindow();
	this->terminateDependencies();
}

bool Engine::initializeDependencies()
{
	*mpDepGlfw = GLFW();
	if (!mpDepGlfw->initialize()) return false;

	return true;
}

void Engine::terminateDependencies()
{
	if (mpDepGlfw->hasBeenInitialized())
		mpDepGlfw->terminate();
}

bool Engine::createWindow()
{
	*mpWindowGame = Window(640, 480, "Temportal Engine");
	if (!mpWindowGame->isValid()) return false;

	mpWindowGame->setKeyCallback(&windowKeyInputCallback);
	mpWindowGame->initializeRenderContext(1);

	return true;
}

void Engine::destroyWindow()
{
	if (mpWindowGame->isValid())
	{
		mpWindowGame->destroy();
	}
}

void Engine::run()
{
	*mpThreadRender = Thread<Window*>("ThreadRender", &Window::renderUntilClose);
	mpThreadRender->start(mpWindowGame);

	while (mpWindowGame->isValid() && !mpWindowGame->isClosePending())
	{
		this->pollInput();
		mpInputQueue->dispatchAll();
	}
	mpThreadRender->join();

}

void Engine::pollInput()
{
	mpWindowGame->pollInput();
}

void windowKeyInputCallback(Window* window, int key, int scancode, int action, int mods)
{
	Engine* pEngine;
	if (Engine::GetChecked(pEngine))
	{
		pEngine->enqueueInput(input::Event{ key, scancode, action, mods });
	}
}

void inputQueueListener(input::Event const & evt)
{
	Engine* pEngine;
	if (Engine::GetChecked(pEngine))
	{
		pEngine->processInput(evt);
	}
}

void Engine::enqueueInput(input::Event const & evt)
{
	this->mpInputQueue->enqueue(evt);
}

void Engine::processInput(input::Event const & evt)
{
	LogEngineInfo("Received Input Event");
	if (evt.key == GLFW_KEY_ESCAPE && evt.action == GLFW_PRESS)
	{
		mpWindowGame->markShouldClose();
	}
}
