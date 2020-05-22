#ifndef WINDOW_H
#define WINDOW_H

#include "TemportalEnginePCH.hpp"

#include "types/integer.h"
#include "input/Event.hpp"
#include "input/types.h"

// TODO: Namespace
// TODO: Organize Headers

NS_INPUT
class Queue;
NS_END

NS_RENDER
class Renderer;
NS_END

NS_UTILITY
struct SExecutableInfo;
NS_END

class TEMPORTALENGINE_API Window
{
private:
	ui32 mWidth, mHeight;
	char const * mpTitle;

	void* mpHandle;
	void* mpJoystick;
	bool mIsPendingClose;
	input::ListenerHandle mInputHandleQuit;

	render::Renderer *mpRenderer;

public:
	Window() = default;
	Window(
		ui32 width, ui32 height,
		utility::SExecutableInfo const *const appInfo
	);

	bool isValid();
	void addInputListeners(input::Queue *pQueue);

	void onInputQuit(input::Event const &evt);
	void markShouldClose();
	bool isPendingClose();

	bool renderUntilClose();
	void waitForCleanup();
	void destroy();

};

#endif //WINDOW_H
