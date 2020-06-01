#include "asset/Project.hpp"

#include "asset/AssetManager.hpp"

#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>

using namespace asset;

Project::Project(std::string name, Version version) : Asset()
{
	this->mName = name;
	this->mVersion = version;
}

std::string Project::getName() const
{
	return this->mName;
}

Version Project::getVersion() const
{
	return this->mVersion;
}

std::string Project::getDisplayName() const
{
	return this->getName() + " (" + this->getVersion().toString() + ")";
}

std::filesystem::path Project::getAbsoluteDirectoryPath() const
{
	return this->mProjectDirectory;
}

std::filesystem::path Project::getAssetDirectory() const
{
	return this->getAbsoluteDirectoryPath() / "assets";
}

std::shared_ptr<Asset> Project::createAsset(std::filesystem::path filePath)
{
	auto ptr = asset::AssetManager::makeAsset<Project>();
	ptr->mName = filePath.stem().string();
	ptr->mProjectDirectory = filePath.parent_path();

	std::ofstream os(filePath);
	ptr->writeToDisk(&os);

	return ptr;
}

std::shared_ptr<Asset> Project::readFromDisk(std::ifstream *stream, std::filesystem::path filePath)
{
	auto ptr = asset::AssetManager::makeAsset<Project>();
	cereal::JSONInputArchive archive(*stream);
	ptr->load(archive);
	ptr->mProjectDirectory = filePath.parent_path();
	return ptr;
}

void Project::writeToDisk(std::ofstream *stream)
{
	cereal::JSONOutputArchive archive(*stream);
	this->save(archive);
}
