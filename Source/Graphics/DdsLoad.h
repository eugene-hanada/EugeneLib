#pragma once
#include <cstdint>
#include <filesystem>
#include <unordered_map>
#include <functional>

namespace Eugene
{

	struct DdsHeader
	{
		/// <summary>
		/// �w�b�_�[�T�C�Y
		/// </summary>
		std::int32_t size;

		/// <summary>
		/// �w�b�_���̏��̃t���O
		/// </summary>
		std::int32_t flags;

		/// <summary>
		/// ����
		/// </summary>
		std::int32_t height;

		/// <summary>
		/// ��
		/// </summary>
		std::int32_t width;

		/// <summary>
		/// ��1���C���̃o�C�g���܂���1�ʕ��̃o�C�g��
		/// </summary>
		std::int32_t pitchOrLinearSize;

		/// <summary>
		/// ���s�T�C�Y
		/// </summary>
		std::int32_t depth;

		/// <summary>
		/// �~�b�v�}�b�v��
		/// </summary>
		std::int32_t mipMapCount;

		/// <summary>
		/// �\��̈�
		/// </summary>
		std::int32_t reserved1[11];

		/// <summary>
		/// �s�N�Z���t�H�[�}�b�g���̃o�C�g��
		/// </summary>
		std::int32_t pixelFormatSize;

		/// <summary>
		/// �s�N�Z���t�H�[�}�b�g�̃t���O
		/// </summary>
		std::int32_t pixelFormatFlags;

		/// <summary>
		/// �t�H�[�}�b�g�̒�`
		/// </summary>
		std::int32_t fourCC;

		/// <summary>
		/// 1�s�N�Z��������̃r�b�g��
		/// </summary>
		std::int32_t rgbBitCount;

		std::int32_t rBitMask;
		std::int32_t gBitMask;
		std::int32_t bBitMask;
		std::int32_t aBitMask;

		/// <summary>
		/// �~�b�v�}�b�v��L���[�u�}�b�v�Ƃ��̏��
		/// </summary>
		std::int32_t caps;

		/// <summary>
		/// ��̏ڍׂȓz
		/// </summary>
		std::int32_t caps2;

		/// <summary>
		/// �\��̈�
		/// </summary>
		std::int32_t reservedCaps[2];

		/// <summary>
		/// �\��̈�
		/// </summary>
		std::int32_t reserved2;
	};

	struct DdsExtensionHeader
	{
		/// <summary>
		/// �t�H�[�}�b�g
		/// </summary>
		std::uint32_t format = 0u;

		/// <summary>
		/// 1D�Ƃ�2D�Ƃ��̏��
		/// </summary>
		std::uint32_t dimension = 0u;

		/// <summary>
		/// 
		/// </summary>
		std::uint32_t miscFlag = 0u;

		/// <summary>
		/// �z��T�C�Y
		/// </summary>
		std::uint32_t arraySize = 0u;

		/// <summary>
		/// 
		/// </summary>
		std::uint32_t miscFlag2 = 0u;
	};

	struct ColcBase
	{
		virtual ~ColcBase() {};
		virtual std::int32_t operator()(std::int32_t x, std::int32_t y, std::int32_t pixelSize) = 0;
	};

	struct ColcDxt1Size 
	{
		std::int32_t operator()(std::int32_t x, std::int32_t y, std::int32_t pixelSize)
		{
			return std::max(1, ((x + 3) / 4)) * std::max(1, ((y + 3) / 4)) * 8;
		}
	};

	struct ColcSize
	{
		std::int32_t operator()(std::int32_t x, std::int32_t y, std::int32_t pixelSize)
		{
			return x * y * pixelSize;
		}
	};

	struct ColcDxt3Size
	{
		std::int32_t operator()(std::int32_t x, std::int32_t y, std::int32_t pixelSize)
		{
			return std::max(1, ((x + 3) / 4)) * std::max(1, ((y + 3) / 4)) * 16;
		}
	};

	const std::unordered_map<std::int32_t, std::function<std::int32_t(std::int32_t, std::int32_t, std::int32_t)>> colcMap
	{
		{0,ColcSize{}},
		{'1TXD',ColcDxt1Size{}},
		{'3TXD',ColcDxt3Size{}},
		{'5TXD',ColcDxt3Size{}}
	};

	void LoadDdsExtension(std::ifstream& file, TextureInfo& info)
	{
		// �ǉ�����ǂݍ���
		DdsExtensionHeader ext;
		file.read(reinterpret_cast<char*>(&ext), sizeof(ext));
		info.arraySize = std::max(1u,ext.arraySize);
		
		// �t�H�[�}�b�g���Z�b�g����
		switch (ext.format)
		{
		case 98:
			info.format = Format::BC7_UNORM;
			return;
		case 70:
			info.format = Format::BC1_UNORM;
			return;
		default:
			info.format = Format::R8G8B8A8_SNORM;
			return;
		}
		
	}
}
