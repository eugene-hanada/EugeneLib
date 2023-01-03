#pragma once

namespace EugeneLib
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
