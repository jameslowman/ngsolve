#include "ngstd.hpp"

namespace ngstd
{

    SharedLibrary :: SharedLibrary(const filesystem::path & lib_name_, optional<filesystem::path> directory_to_delete_ )
        : lib_name(lib_name_),directory_to_delete(directory_to_delete_)
    {
      Load(lib_name);
    }

    SharedLibrary :: ~SharedLibrary()
    {
      Unload();
      if(directory_to_delete)
          filesystem::remove_all(*directory_to_delete);
    }

    void SharedLibrary :: Load( const filesystem::path & lib_name_ )
    {
      Unload();
      lib_name = lib_name_;
#ifdef WIN32
      lib = LoadLibrary(lib_name.string().c_str());
      if (!lib) throw std::runtime_error(string("Could not load library ") + lib_name.string());
#else // WIN32
      lib = dlopen(lib_name.c_str(), RTLD_NOW);
      if(lib == nullptr) throw std::runtime_error(dlerror());
#endif // WIN32
    }

    void SharedLibrary :: Unload() {
      if(lib)
      {
#ifdef WIN32
        FreeLibrary(lib);
#else // WIN32
        int rc = dlclose(lib);
        if(rc != 0) cerr << "Failed to close library " << lib_name << endl;
#endif // WIN32
      }
    }

    void* SharedLibrary :: GetRawFunction( string func_name )
    {
#ifdef WIN32
      void* func = GetProcAddress(lib, func_name.c_str());
      if(func == nullptr)
        throw std::runtime_error(string("Could not find function ") + func_name + " in library " + lib_name.string());
#else // WIN32
      void* func = dlsym(lib, func_name.c_str());
      if(func == nullptr)
          throw std::runtime_error(dlerror());
#endif // WIN32

      return func;
    }
} // namespace ngstd
