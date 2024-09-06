#include "../../../Include/System/Linux/SharedObject.h"
#include <dlfcn.h>
#include "../../../Include/Utils/EugeneLibException.h"

Eugene::DynamicLibrary::DynamicLibrary(const std::filesystem::path &path)
{
    dl_ = dlopen(path.c_str(),RTLD_LAZY);

    if (dl_ == nullptr)
    {
        throw EugeneLibException(dlerror());
    }
}

Eugene::DynamicLibrary::~DynamicLibrary()
{
    dlclose(dl_);
}

void *Eugene::DynamicLibrary::FindFunction(const std::string_view& functionName) const
{
    return dlsym(dl_, functionName.data());
}