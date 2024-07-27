#pragma once
#include <cstdint>

namespace Eugene
{
	class Graphics;
	class BufferResource;
	class ImageResource;

	class Sampler;

	/// <summary>
	/// シェーダーリソース用のビュー(テクスチャ、定数バッファ等)
	/// </summary>
	class ShaderResourceViews
	{
	public:
		virtual ~ShaderResourceViews();

		/// <summary>
		/// テクスチャビューを生成
		/// </summary>
		/// <param name="resource"> リソース </param>
		/// <param name="idx"> インデックス </param>
		virtual void CreateTexture(ImageResource& resource, std::uint64_t idx) = 0;

		/// <summary>
		/// 定数バッファビューを生成
		/// </summary>
		/// <param name="resource"> リソース </param>
		/// <param name="idx"> インデックス </param>
		virtual void CreateConstantBuffer(BufferResource& resource, std::uint64_t idx) = 0;

		/// <summary>
		/// キューブマップビューを生成
		/// </summary>
		/// <param name="resource"> リソース </param>
		/// <param name="idx"> インデックス </param>
		virtual void CreateCubeMap(ImageResource& resource, std::uint64_t idx) = 0;

		/// <summary>
		/// UnorderedAccess可能なバッファをのビューを生成
		/// </summary>
		/// <param name="resource"></param>
		/// <param name="numElements"></param>
		/// <param name="strideSize"></param>
		virtual void CreateUnorderedAccessBuffer(BufferResource& resource, std::uint64_t idx ,std::uint64_t numElements, std::uint64_t strideSize) = 0;

		virtual void* GetViews(void) = 0;

		/// <summary>
		/// サイズを取得
		/// </summary>
		/// <param name=""></param>
		/// <returns> サイズ </returns>
		const std::uint64_t GetSize(void);
		virtual std::uint64_t GetImg(void) = 0;
	protected:
		ShaderResourceViews(std::uint64_t size);

		/// <summary>
		/// サイズ
		/// </summary>
		std::uint64_t size_;
	};
}
