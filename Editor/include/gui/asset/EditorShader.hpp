#pragma once

#include "gui/asset/AssetEditor.hpp"

NS_GUI
class TextEditor;

// Editor for `asset::Shader`
class EditorShader : public AssetEditor
{

public:
	static std::shared_ptr<AssetEditor> create(std::shared_ptr<memory::MemoryChunk> mem);

	void setAsset(asset::AssetPtrStrong asset) override;

protected:
	void renderView() override;
	void saveAsset() override;

private:
	std::string mSavedShaderContent;
	std::shared_ptr<TextEditor> mTextEditor;

};

NS_END
