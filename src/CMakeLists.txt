set(appName "grep_local")
add_executable(${appName} main.cpp utility/ThreadPool.cpp)
target_compile_features(${appName} PRIVATE cxx_std_20)
