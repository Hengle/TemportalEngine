#include "controller/Controller.hpp"

#include "Engine.hpp"
#include "input/Queue.hpp"
#include "Camera.hpp"

Controller::Controller()
{
	f32 moveSpeed = 2.0f;
	// TODO: These are using y up directions (it should really be Z up)
	this->mForward = { glm::vec3(0, 1, 0), moveSpeed };
	this->mBackward = { glm::vec3(0, -1, 0), moveSpeed };
	this->mStrafeLeft = { glm::vec3(-1, 0, 0), moveSpeed };
	this->mStrafeRight = { glm::vec3(1, 0, 0), moveSpeed };
	this->mUp = { glm::vec3(0, 0, 1), moveSpeed };
	this->mDown = { glm::vec3(0, 0, -1), moveSpeed };
	this->mInputMappings = {
		{ input::EKey::W, &this->mForward },
		{ input::EKey::S, &this->mBackward },
		{ input::EKey::A, &this->mStrafeLeft },
		{ input::EKey::D, &this->mStrafeRight },
		{ input::EKey::E, &this->mUp },
		{ input::EKey::Q, &this->mDown },
	};
	this->mLookHorizontal = { glm::vec3(0, 1, 0), glm::radians(90.0f) };
	this->mLookVertical = { glm::vec3(1, 0, 0), glm::radians(90.0f) };
}

void Controller::assignCamera(std::shared_ptr<Camera> camera)
{
	this->mCamera = camera;
}

void Controller::subscribeToInput()
{
	auto inputQueue = engine::Engine::Get()->getInputQueue();
#define REGISTER_INPUT(EVENT, FUNC_PTR) inputQueue->OnInputEvent.bind(EVENT, this->weak_from_this(), std::bind(FUNC_PTR, this, std::placeholders::_1))
	REGISTER_INPUT(input::EInputType::KEY, &Controller::onKeyInput);
	REGISTER_INPUT(input::EInputType::MOUSE_MOVE, &Controller::onMouseMove);
#undef REGISTER_INPUT
}

void Controller::onKeyInput(input::Event const & evt)
{
	auto mapping = this->mInputMappings.find(evt.inputKey.key);
	if (mapping != this->mInputMappings.end())
	{
		if (evt.inputKey.action == input::EAction::PRESS)
			mapping->second->bIsActive = true;
		if (evt.inputKey.action == input::EAction::RELEASE)
			mapping->second->bIsActive = false;
	}
}

void Controller::onMouseMove(input::Event const & evt)
{
	this->mLookHorizontal.delta = evt.inputMouseMove.xDelta;
	this->mLookVertical.delta = evt.inputMouseMove.yDelta;
	//LogEngineDebug("MouseMove: (%.3f, %.3f)", this->mLookHorizontal.delta, this->mLookVertical.delta);
}

void Controller::processInput(input::Event const & evt)
{
	if (evt.type == input::EInputType::KEY)
	{
		if (evt.inputKey.action == input::EAction::PRESS)
			LogEngineDebug("%i Press", (i32)evt.inputKey.key);
		if (evt.inputKey.action == input::EAction::REPEAT)
			LogEngineDebug("%i Repeat", (i32)evt.inputKey.key);
		if (evt.inputKey.action == input::EAction::RELEASE)
			LogEngineDebug("%i Release", (i32)evt.inputKey.key);
	}
	else if (evt.type == input::EInputType::MOUSE_MOVE)
	{
		LogEngineDebug("MOVE by (%.3f, %.3f) to (%i, %i)", evt.inputMouseMove.xDelta, evt.inputMouseMove.yDelta, evt.inputMouseMove.xCoord, evt.inputMouseMove.yCoord);
	}
	else if (evt.type == input::EInputType::MOUSE_BUTTON)
	{
		if (evt.inputMouseButton.action == input::EAction::PRESS)
			LogEngineDebug("Mouse %i Press (%i) at (%i, %i)", (i32)evt.inputMouseButton.button, evt.inputMouseButton.clickCount, evt.inputMouseButton.coord[0], evt.inputMouseButton.coord[1]);
		if (evt.inputKey.action == input::EAction::RELEASE)
			LogEngineDebug("Mouse %i Release (%i) at (%i, %i)", (i32)evt.inputMouseButton.button, evt.inputMouseButton.clickCount, evt.inputMouseButton.coord[0], evt.inputMouseButton.coord[1]);
	}
	else if (evt.type == input::EInputType::MOUSE_SCROLL)
	{
		LogEngineDebug("Scroll by (%i, %i)", evt.inputScroll.delta[0], evt.inputScroll.delta[1]);
	}
}

void Controller::tick(f32 deltaTime)
{
	for (auto&[key, mapping] : this->mInputMappings)
	{
		if (!mapping->bIsActive) continue;
		this->mCamera->move(mapping->direction * deltaTime * mapping->speed);
	}

	// Testing with roll
	//this->mCamera->rotate(glm::vec3(0, 0, 1), glm::radians(90.0f) * deltaTime);

	// TODO: This isn't really frame independent (doesnt use deltaTime)
	if (std::abs(this->mLookHorizontal.delta) > std::numeric_limits<f32>::epsilon())
	{
		this->mCamera->rotate(this->mLookHorizontal.axis, (this->mLookHorizontal.radians) * this->mLookHorizontal.delta);
		this->mLookHorizontal.delta = 0.0f;
	}
	
	if (std::abs(this->mLookVertical.delta) > std::numeric_limits<f32>::epsilon())
	{
		this->mCamera->rotate(this->mLookVertical.axis, (this->mLookVertical.radians) * this->mLookVertical.delta);
		this->mLookVertical.delta = 0.0f;
	}
}
