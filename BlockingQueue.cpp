//
// Created by samuel on 11/2/17.
//


#include "BlockingQueue.h"

using namespace Async;

std::mutex ioLock;

void test(BlockingQueue<std::string>* pQ)
{
    std::string msg;
    do
    {
        msg = pQ->deQ();
        {
            std::lock_guard<std::mutex> l(ioLock);
            std::cout << "\n    thread deQed "<< msg.c_str();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }while(msg != "quit");
}

//#define TEST_BLOCKINGQUEUE
#ifdef TEST_BLOCKINGQUEUE
int main()
{
    std::cout << "\n    Demonstraing Blocking Queue";
    std::cout << "\n   =============================";

    BlockingQueue<std::string> q;
    std::thread t(test, &q);

    for(int i=0; i<15; i++)
    {
        std::ostringstream temp;
        temp << i;
        std::string msg = std::string("msg#") + temp.str();
        {
            std::lock_guard<std::mutex> l(ioLock);
            std::cout << "\n    main enQing" << msg.c_str();
        }
        q.enQ(msg);
        std::this_thread::sleep_for(std::chrono::milliseconds(3));
    }
    q.enQ("quit");
    t.join();


    std::cout << "\n  move copy";
    // move copy
    std::string msg = "test";
    q.enQ(msg);
    std::cout << "\n before move:";
    std::cout << "\n  q.size() = " << q.size();
    std::cout << "\n  q.front() = " << q.front();
    BlockingQueue<std::string> q2 = std::move(q);
    std::cout << "\n After move:";
    std::cout << "\n  q2.size() = " << q2.size();
    std::cout << "\n  q.size() = " << q.size();
    std::cout << "\n  q2 element = " << q2.deQ();

    std::cout << "\n  move assignment";
    // move copy
    BlockingQueue<std::string> q3;
    q.enQ(msg);
    std::cout << "\n before move:";
    std::cout << "\n  q.size() = " << q.size();
    std::cout << "\n  q.front() = " << q.front();
    q3 = std::move(q);
    std::cout << "\n After move:";
    std::cout << "\n  q2.size() = " << q3.size();
    std::cout << "\n  q.size() = " << q.size();
    std::cout << "\n  q2 element = " << q3.deQ();
}


#endif

