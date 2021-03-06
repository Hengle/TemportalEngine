#pragma once

#include "TemportalEnginePCH.hpp"

#include "asset/Project.hpp"
#include "commandlet/Commandlet.hpp"
#include "graphics/ImGuiRenderer.hpp"
#include "gui/MainDockspace.hpp"
#include "utility/StringUtils.hpp"

class Window;
FORWARD_DEF(NS_ASSET, class AssetManager)
FORWARD_DEF(NS_ASSET, class Settings)
FORWARD_DEF(NS_ENGINE, class Engine)
FORWARD_DEF(NS_GUI, class AssetEditor)
FORWARD_DEF(NS_GRAPHICS, class VulkanRenderer)
FORWARD_DEF(NS_MEMORY, class MemoryChunk)

class Editor
{
	struct RegistryEntryAssetEditor
	{
		asset::AssetType key;
		std::function<std::shared_ptr<gui::AssetEditor>(std::shared_ptr<memory::MemoryChunk> mem)> create;
	};

public:
	static Editor* EDITOR;

	Editor(int argc, char *argv[]);
	~Editor();

	void registerCommandlet(std::shared_ptr<editor::Commandlet> cmdlet);
	void registerAssetEditor(RegistryEntryAssetEditor entry);

	void initialize();
	bool setup();
	void run();

#pragma region Project Being Editted
	bool hasProject() const;
	void setProject(asset::AssetPtrStrong asset);
	asset::ProjectPtrStrong getProject() const;
	std::filesystem::path getCurrentAssetDirectory() const;
#pragma endregion

#pragma region Editor Settings per Project
	void loadEditorSettings(std::filesystem::path projectDir);
	std::shared_ptr<asset::Settings> getEditorSettings() const;
	std::filesystem::path getOutputDirectory() const;
	std::filesystem::path getAssetBinaryPath(asset::AssetPtrStrong asset) const;
#pragma endregion

#pragma region View Management Shortcuts
	std::shared_ptr<memory::MemoryChunk> getMemoryGui() const;
	void openAssetEditor(asset::AssetPtrStrong asset);
	void closeGui(std::string id);
	void openProjectSettings();
	void openSettings();
#pragma endregion

protected:

	void createEngine();
	virtual void registerAssetTypes(std::shared_ptr<asset::AssetManager> assetManager);

	virtual void registerAssetEditors();
	bool hasRegisteredAssetEditor(asset::AssetType type) const;
	
	virtual void registerAllCommandlets();

private:
	bool mbShouldRender;

	utility::ArgumentMap mArgs;
	std::unordered_map<std::string, uSize> mMemorySizes;
	std::shared_ptr<engine::Engine> mpEngine;

	std::unordered_map<std::string, std::shared_ptr<editor::Commandlet>> mCommandlets;
	// TODO: Make a registry class that handles the storage of register items
	std::unordered_map<asset::AssetType, RegistryEntryAssetEditor> mAssetEditors;

	std::shared_ptr<Window> mpWindow;
	std::shared_ptr<memory::MemoryChunk> mpMemoryGui;
	std::shared_ptr<graphics::ImGuiRenderer> mpRenderer;
	std::shared_ptr<gui::MainDockspace> mpDockspace;

	// The project that the editor is currently operating on
	asset::ProjectPtrStrong mpProject;
	std::shared_ptr<asset::Settings> mpEditorSettings;

	void initializeRenderer(std::shared_ptr<graphics::VulkanRenderer> pRenderer);

};
