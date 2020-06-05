#pragma once

#include "TemportalEnginePCH.hpp"

#include "asset/Asset.hpp"
#include "memory/MemoryChunk.hpp"

#include <filesystem>
#include <fstream>
#include <functional>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <map>

NS_ASSET

/**
 * Metadata information about a generic type of asset.
 */
struct AssetTypeMetadata
{
	std::string DisplayName;
	std::string fileExtension;
	std::function<asset::AssetPtrStrong(std::filesystem::path filePath)> createNewAsset;
	std::function<asset::AssetPtrStrong()> createEmptyAsset;
	std::optional<std::function<void(std::filesystem::path filePath)>> onAssetDeleted;
};

/**
 * Metadata information about a specific asset instance.
 * TODO: This is basically the asset-id (type and path).
 */
struct AssetMetadata
{
	AssetType type;
	/**
	 * The location of the asset relative to the directory passed to `AssetManager#scanAssetDirectory`.
	 */
	std::filesystem::path path;
};

class AssetManager
{

public:
	static std::shared_ptr<AssetManager> get();

	std::shared_ptr<memory::MemoryChunk> mpAssetMemory;
	void setAssetMemory(std::shared_ptr<memory::MemoryChunk> assetMemory);
	std::shared_ptr<memory::MemoryChunk> getAssetMemory() const;
	
	template <typename T, typename... Types>
	static std::shared_ptr<T> makeAsset(Types&& ...args)
	{
		return AssetManager::get()->mpAssetMemory->make_shared<T>(args...);
	}

	/**
	 * Registers all known asset types.
	 * Should only ever be called once.
	 */
	void queryAssetTypes();

	/**
	 * Recursively scans all files in the directory and catalogs any files
	 * whose extensions are known (based on asset types cataloged by `registerType`).
	 * Performs a read operation on each file with a known extension to determine its `AssetType`.
	 * Generally collects `AssetMetadata`.
	 */
	void scanAssetDirectory(std::filesystem::path directory, asset::EAssetSerialization type);

	std::set<AssetType> getAssetTypes() const;
	bool isValidAssetExtension(std::string extension) const;
	AssetTypeMetadata getAssetTypeMetadata(AssetType type) const;
	std::string getAssetTypeDisplayName(AssetType type) const;
	std::optional<AssetMetadata> getAssetMetadata(std::filesystem::path filePath) const;
	void registerType(AssetType type, AssetTypeMetadata metadata);

	std::shared_ptr<Asset> createAsset(AssetType type, std::filesystem::path filePath);
	void deleteFile(std::filesystem::path filePath);
	std::shared_ptr<Asset> readAssetFromDisk(std::filesystem::path filePath, asset::EAssetSerialization type, bool bShouldHaveBeenScanned=true);

	template <typename TAsset>
	std::shared_ptr<TAsset> createAssetAs(AssetType type, std::filesystem::path filePath)
	{
		return std::dynamic_pointer_cast<TAsset>(this->createAsset(type, filePath));
	}
	template <typename TAsset>
	std::shared_ptr<TAsset> readFromDisk(std::filesystem::path filePath, asset::EAssetSerialization type, bool bShouldHaveBeenScanned = true)
	{
		return std::dynamic_pointer_cast<TAsset>(this->readAssetFromDisk(filePath, type, bShouldHaveBeenScanned));
	}

private:
	/**
	 * A unique set of all known asset types.
	 * New types can be registered with `registerType`.
	 */
	std::set<AssetType> mAssetTypes;
	/**
	 * A list of all asset extensions.
	 * Populated when an asset type is registered via `registerType`.
	 */
	std::set<std::string> mAssetTypeExtensions;
	/**
	 * A mapping of a unique asset type to its metadata information.
	 * Populated when an asset type is registered via `registerType`.
	 */
	std::unordered_map<AssetType, AssetTypeMetadata> mAssetTypeMap;

	/**
	 * A map of asset extension (see `AssetTypeMetadata::fileExtension`) to all known asset paths (see `mScannedAssetMetadataByPath`).
	 * Initially populated by `scanAssetDirectory`.
	 * Can be modified when assets are created by `createAsset` or when they are destroyed (TODO: Assets cannot yet be destroyed without editing files directly).
	 */
	std::unordered_multimap<std::string, std::filesystem::path> mScannedAssetPathsByExtension;
	/**
	 * A map of an asset's path (relative to the directory passed to `scanAssetDirectory`) to all known assets.
	 * Initially populated by `scanAssetDirectory`.
	 * Can be modified when assets are created by `createAsset` or when they are destroyed (TODO: Assets cannot yet be destroyed without editing files directly).
	 */
	std::unordered_multimap<std::string, AssetMetadata> mScannedAssetMetadataByPath;

	void addScannedAsset(AssetMetadata metadata);

};

NS_END
