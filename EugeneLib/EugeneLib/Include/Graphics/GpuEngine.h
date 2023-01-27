#pragma once

namespace Eugene
{
	class Graphics;
	class CommandList;

	/// <summary>
	/// CommandListクラスを実行などの処理を行うクラス
	/// </summary>
	class GpuEngine
	{
	public:

		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~GpuEngine();

		/// <summary>
		/// PushしたCommandListを実行する
		/// </summary>
		/// <param name=""></param>
		virtual void Execute(void) = 0;

		/// <summary>
		/// 
		/// </summary>
		/// <param name=""></param>
		virtual void Wait(void) = 0;

		/// <summary>
		/// 実行するコマンドリストを追加する
		/// </summary>
		/// <param name="commandList"> コマンドリスト </param>
		virtual void Push(CommandList& commandList) = 0;

		virtual void* GetQueue(void) const = 0;
	protected:

		/// <summary>
		/// コマンドリスト
		/// </summary>
		GpuEngine();
	};
}