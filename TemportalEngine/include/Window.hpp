#ifndef WINDOW_H
#define WINDOW_H

#include "TemportalEnginePCH.hpp"

#include "types/integer.h"
#include "input/Event.hpp"

// TODO: Namespace
// TODO: Organize Headers

NS_RENDER
class Renderer;
NS_END

NS_UTILITY
struct SExecutableInfo;
NS_END

class TEMPORTALENGINE_API Window
{
public:
	static void renderUntilClose(void* ptr);

private:
	ui32 mWidth, mHeight;
	char const * mpTitle;

	void* mpHandle;
	void* mpJoystick;
	bool mIsPendingClose;

	render::Renderer *mpRenderer;

public:
	Window() = default;
	Window(
		ui32 width, ui32 height,
		utility::SExecutableInfo const *const appInfo
	);

	bool isValid();

	void initializeRenderContext();

	void markShouldClose();
	bool isPendingClose();

	void render();
	void destroy();

};

#endif //WINDOW_H
