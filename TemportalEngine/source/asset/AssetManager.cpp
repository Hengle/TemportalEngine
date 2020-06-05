#include "asset/AssetManager.hpp"

#include "Engine.hpp"
#include "logging/Logger.hpp"

#include "asset/Asset.hpp"
#include "asset/Project.hpp"
#include "asset/Shader.hpp"

#include <filesystem>
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/json.hpp>

using namespace asset;

static logging::Logger LOG = DeclareLog("AssetManager");

std::shared_ptr<AssetManager> AssetManager::get()
{
	return engine::Engine::Get()->getAssetManager();
}

void AssetManager::setAssetMemory(std::shared_ptr<memory::MemoryChunk> assetMemory)
{
	this->mpAssetMemory = assetMemory;
}

std::shared_ptr<memory::MemoryChunk> AssetManager::getAssetMemory() const
{
	return this->mpAssetMemory;
}

void AssetManager::queryAssetTypes()
{
	this->registerType(AssetType_Project, { "Project", ".te-project", &Project::createNewAsset, &Project::createEmptyAsset, std::nullopt });
	this->registerType(AssetType_Shader, { "Shader", ".te-asset", &Shader::createNewAsset, &Shader::createEmptyAsset, &Shader::onAssetDeleted });
}

void AssetManager::scanAssetDirectory(std::filesystem::path directory, asset::EAssetSerialization type)
{
	this->mScannedAssetPathsByExtension.clear();
	this->mScannedAssetMetadataByPath.clear();

	ui32 totalFilesScanned = 0, foundAssetCount = 0;
	for (const auto& entry : std::filesystem::recursive_directory_iterator(directory))
	{
		// Ignore any non-normal files (directories, symlinks, etc.)
		if (!entry.is_regular_file()) continue;

		totalFilesScanned++;

		// Ignore all files whose extension is not known
		if (!this->isValidAssetExtension(entry.path().extension().string())) continue;

		foundAssetCount++;

		AssetType assetType;
		{
			auto asset = makeAsset<Asset>();
			asset->readFromDisk(entry.path(), type);
			assetType = asset->getAssetType();
		}

		this->addScannedAsset({ assetType, entry.path() });
	}

	LOG.log(logging::ECategory::LOGINFO, "Scanned %i files and found %i assets", totalFilesScanned, foundAssetCount);
}

void AssetManager::addScannedAsset(AssetMetadata metadata)
{
	this->mScannedAssetPathsByExtension.insert(std::make_pair(metadata.path.extension().string(), metadata.path));
	this->mScannedAssetMetadataByPath.insert(std::make_pair(metadata.path.string(), metadata));
}

std::set<AssetType> AssetManager::getAssetTypes() const
{
	return this->mAssetTypes;
}

bool AssetManager::isValidAssetExtension(std::string extension) const
{
	return this->mAssetTypeExtensions.find(extension) != this->mAssetTypeExtensions.end();
}

AssetTypeMetadata AssetManager::getAssetTypeMetadata(AssetType type) const
{
	auto iter = this->mAssetTypeMap.find(type);
	assert(iter != this->mAssetTypeMap.end());
	return iter->second;
}

std::string AssetManager::getAssetTypeDisplayName(AssetType type) const
{
	return this->getAssetTypeMetadata(type).DisplayName;
}

void AssetManager::registerType(AssetType type, AssetTypeMetadata metadata)
{
	// Assumes that the type has not been registered before
	assert(this->mAssetTypes.find(type) == this->mAssetTypes.end());
	// Add type and metadata to mappings
	this->mAssetTypes.insert(type);
	this->mAssetTypeMap.insert(std::make_pair(type, metadata));
	// Ensure that if the extension isn't already cataloged, it gets added to the set
	if (!this->isValidAssetExtension(metadata.fileExtension))
	{
		this->mAssetTypeExtensions.insert(metadata.fileExtension);
	}
}

std::optional<AssetMetadata> AssetManager::getAssetMetadata(std::filesystem::path filePath) const
{
	auto iter = this->mScannedAssetMetadataByPath.find(filePath.string());
	return iter == this->mScannedAssetMetadataByPath.end() ? std::nullopt : std::make_optional(iter->second);
}

std::shared_ptr<Asset> AssetManager::createAsset(AssetType type, std::filesystem::path filePath)
{
	auto typeMapEntry = this->mAssetTypeMap.find(type);
	assert(typeMapEntry != this->mAssetTypeMap.end());
	auto asset = typeMapEntry->second.createNewAsset(filePath);
	asset->writeToDisk(filePath, EAssetSerialization::Json);
	this->addScannedAsset({ type, filePath });
	return asset;
}

void AssetManager::deleteFile(std::filesystem::path filePath)
{
	if (this->isValidAssetExtension(filePath.extension().string()))
	{
		auto assetMeta = this->getAssetMetadata(filePath);
		if (!assetMeta.has_value())
		{
			LOG.log(logging::ECategory::LOGWARN, "Deleting asset %s which has not been scanned.", filePath.string().c_str());
		}
		else
		{
			auto assetTypeMeta = this->getAssetTypeMetadata(assetMeta.value().type);
			if (assetTypeMeta.onAssetDeleted.has_value())
			{
				assetTypeMeta.onAssetDeleted.value()(filePath);
			}
		}
	}
	std::filesystem::remove(filePath);
}

std::shared_ptr<Asset> AssetManager::readAssetFromDisk(std::filesystem::path filePath, asset::EAssetSerialization type, bool bShouldHaveBeenScanned)
{
	assert(std::filesystem::exists(filePath));
	if (!this->isValidAssetExtension(filePath.extension().string()))
	{
		return nullptr;
	}

	// Read the asset using the base asset class to determine the asset type that is stored
	auto assetMetadata = this->getAssetMetadata(filePath);
	AssetType assetType;
	if (!assetMetadata.has_value())
	{
		if (bShouldHaveBeenScanned)
			LOG.log(logging::ECategory::LOGWARN, "Reading asset %s which has not been scanned.", filePath.string().c_str());
		auto asset = makeAsset<Asset>();
		asset->readFromDisk(filePath, type);
		assetType = asset->getAssetType();
	}
	else
	{
		assetType = assetMetadata->type;
	}

	// Determine the functor for loading this asset type
	auto typeMapEntry = this->mAssetTypeMap.find(assetType);
	assert(typeMapEntry != this->mAssetTypeMap.end());

	auto asset = typeMapEntry->second.createEmptyAsset();
	asset->readFromDisk(filePath, type);
	return asset;
}
