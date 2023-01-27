#pragma once

namespace Eugene
{
	class IndexView
	{
	public:
		virtual ~IndexView();
		virtual void* GetView(void) const = 0;
	protected:
		IndexView();
	};
}
