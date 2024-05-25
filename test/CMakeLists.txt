set(appName "test_grep")
add_executable(${appName}   main.cpp 
                            ../src/utility/ThreadPool.cpp
                            )

target_compile_features(${appName} PRIVATE cxx_std_20)