#pragma once

namespace Eugene
{
	/// <summary>
	/// インデックス用のビュー
	/// </summary>
	class IndexView
	{
	public:
		virtual ~IndexView();
		virtual void* GetView(void) const = 0;
	protected:
		IndexView();
	};
}
