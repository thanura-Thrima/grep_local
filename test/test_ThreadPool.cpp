#include <iostream>

#include "../src/utility/ThreadPool.h"

bool testFunction_ThreaPoolDestruction()
{
    std::cout << "Thread auto join : ";
    {
        Utility::ThreadPool tp(5);
        tp.start();
        for (int i = 0; i < 10; i++)
        {
            tp.addAJob([=]()
                       { 
                for( int k = 0; k< 10; k++)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                } });
        }
    }
    return true;
}

bool testFunction_ThreaPoolSingleThread()
{
    std::cout << "Thread single thread : ";
    {
        Utility::ThreadPool tp(1);
        tp.start();
        for (int i = 0; i < 10; i++)
        {
            tp.addAJob([=]()
                       { 
                for( int k = 0; k< 10; k++)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                } });
        }
    }
    return true;
}