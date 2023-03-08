#pragma once
#include "../../../Include/Sound/SoundStreamSpeaker.h"
#include <filesystem>
#include <thread>
#include <atomic>
#include <semaphore>
#include <memory>
#include <xaudio2.h>
#include <vector>



namespace Eugene
{
	
	class Xa2SoundStreamSpeaker :
		public SoundStreamSpeaker
	{
	public:
		Xa2SoundStreamSpeaker(IXAudio2* device,const std::filesystem::path& path, std::uint16_t outChannel);
		~Xa2SoundStreamSpeaker();
	private:
		class CollBack : public IXAudio2VoiceCallback
		{
		public:
			CollBack(Xa2SoundStreamSpeaker& speaker);
			void OnBufferEnd(void* pBufferContext) noexcept final;
			void OnBufferStart(void* pBufferContext) noexcept final;
			void OnLoopEnd(void* pBufferContext) noexcept final;
			void OnStreamEnd() noexcept final;
			void OnVoiceError(void* pBufferContext,HRESULT Error) noexcept final;
			void OnVoiceProcessingPassEnd() noexcept final;
			void OnVoiceProcessingPassStart(std::uint32_t BytesRequired) noexcept final;
		private:
			Xa2SoundStreamSpeaker& speaker_;
		};

		void Play(void) final;
		void Stop(void);
		bool IsEnd(void) const final;
		void SetPitchRate(float rate) final;
		void SetOutput(SoundControl& control) final;
		void SetVolume(float volume) final;
		void SetPan(std::span<float> volumes) final;

		void Worker(void);

		/// <summary>
		/// �\�[�X�{�C�X
		/// </summary>
		IXAudio2SourceVoice* source_;

		/// <summary>
		/// �R�[���o�b�N�p�I�u�W�F�N�g
		/// </summary>
		std::unique_ptr<CollBack> collback_;
		
		/// <summary>
		/// �X�g���[�~���O�Ŏg�p����X���b�h
		/// </summary>
		std::thread streamThread_;

		/// <summary>
		/// ���s�t���O
		/// </summary>
		std::atomic_bool isRun_;

		/// <summary>
		/// �Đ��t���O
		/// </summary>
		std::atomic_bool isPlay_;

		/// <summary>
		/// �Z�}�t�H
		/// </summary>
		std::binary_semaphore semaphore_{0};

		/// <summary>
		/// 
		/// </summary>
		std::streampos starPos_;

		/// <summary>
		/// �f�[�^�T�C�Y
		/// </summary>
		std::uint32_t dataSize_;

		/// <summary>
		/// �Đ������T�C�Y
		/// </summary>
		std::uint32_t nowSize_;

		/// <summary>
		/// �o�b�t�@�[
		/// </summary>
		std::unique_ptr<XAUDIO2_BUFFER> buffer_;

		/// <summary>
		/// �o�b�t�@�[�p�f�[�^
		/// </summary>
		std::vector<std::uint8_t> bufferData_;

		/// <summary>
		/// �X�g���[�~���O�œǂݍ��񂾃f�[�^
		/// </summary>
		std::vector<std::uint8_t> streamData_;

		/// <summary>
		/// �X�g���[�~���O�œǂݍ��񂾃f�[�^�T�C�Y
		/// </summary>
		std::uint32_t streamSize_;

		/// <summary>
		/// 1�b������̃o�C�g��
		/// </summary>
		std::uint32_t bytesPerSec;

		friend class CollBack;
	};
}
