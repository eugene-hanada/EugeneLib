#pragma once
//#include "../../../Include/Sound/Sound3DControl.h"
//#include <span>
//#include <xaudio2.h>
//
//namespace Eugene
//{
//	class Xaudio23DControl :
//		public Sound3DControl
//	{
//	public:
//		Xaudio23DControl(IXAudio2* xaudio2, std::uint32_t sample, std::uint32_t stage, std::uint16_t inChannel, std::uint16_t outChannel,std::span<std::uint8_t, 20> handle);
//		~Xaudio23DControl();
//		void Set3DSound(
//			const glm::vec3& listenerFront, const glm::vec3& listenerTop, const glm::vec3& listenerPos, const glm::vec3& listenerVeclocity
//			, const glm::vec3& emitterFront, const glm::vec3& emitterTop, const glm::vec3& emitterPos, const glm::vec3& emitterVelocity) final;
//	private:
//		std::span<std::uint8_t, 20> handle_;
//
//		struct SubmixVoiceDeleter
//		{
//			void operator()(IXAudio2SubmixVoice* submix)
//			{
//				submix->DestroyVoice();
//			}
//		};
//
//
//		std::unique_ptr<IXAudio2SubmixVoice, SubmixVoiceDeleter> submix_;
//
//		// Sound3DControl を介して継承されました
//		void* Get(void) final;
//		void SetVolume(float volume) final;
//		void SetPan(std::span<float> volumes) final;
//		void SetOutput(SoundControl& control) final;
//	};
//}
