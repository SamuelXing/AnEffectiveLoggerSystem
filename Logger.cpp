//
// Created by samuel on 11/2/17.
//

#include <functional>
#include <fstream>
#include "Logger.h"

using namespace Logging;

// --------<send text message to std::ostream>------
void Logger::write(const std::string &msg)
{
    if(_ThreadingRunning)
    {
        _queue.enQ(msg);
    }
}

void Logger::title(const std::string &msg, char underline)
{
    std::string temp = "\n  " + msg + "\n   " + std::string(msg.size()+2, underline);
    write(temp);
}

// -------<attach logger to existing std::ostream>-------
void Logger::attach(std::ostream *pOut)
{
    streams_.push_back(pOut);
}

// -------<start logging>----------------------------
/*
 * Log to all the attached streams
 * */
void Logger::start()
{
    if(_ThreadingRunning)
        return;
    _ThreadingRunning = true;
    std::function<void()> tp = [=]()
    {
        while(true)
        {
            std::string msg = _queue.deQ();
            if(msg == "quit")
            {
                _ThreadingRunning = false;
                break;
            }
            for(auto pStrm : streams_)
            {
                *pStrm << msg;
            }
        }
    };
    _pThr = new std::thread(tp);
}

// --------<has logger been started>-------------------
bool Logger::running()
{
    return _ThreadingRunning;
}

// ---------<suspend logger>------------------------
void Logger::pause()
{
    pthread_mutex_lock(&mtx_);
    _Paused = true;
    pthread_mutex_unlock(&mtx_);

}

// ---------<resume logger>------------------------
void Logger::resume()
{
   pthread_mutex_lock(&mtx_);
    _Paused = false;
    pthread_cond_broadcast(&cond_);
    pthread_mutex_unlock(&mtx_);
}


// ---<is Logger currently paused?>--------------------
bool Logger::paused()
{
    return _Paused;
}

// -----<wait for logging to empty input queue> ----------
void Logger::flush()
{
    if(_ThreadingRunning && !_Paused)
    {
        while(_queue.size() > 0);
        for(auto pStream : streams_)
            pStream->flush();
    }
}

// -------<stop logging>------------------------------
void Logger::stop(const std::string &msg)
{
    if(_ThreadingRunning)
    {
        if(msg != "")
            write(msg);
        write("quit");
        if(_pThr->joinable())
            _pThr->join();

        _ThreadingRunning = false;
    }
}

// ----------<wait for logger to finish>--------------------
void Logger::wait()
{
    if(_ThreadingRunning && _pThr->joinable())
    {
        _pThr->join();
    }
}

// ------------<stop logging thread>-----------
Logger::~Logger()
{
    stop();
}

#define TEST_LOGGER
#ifdef  TEST_LOGGER

int main()
{
    Logger log;
    log.attach(&std::cout);
    std::ofstream out("logFile.txt");
    if(out.good())
        log.attach(&out);
    else
        std::cout << "\n    couldn't open logfile for writing";
    log.write("\n won't get logged - not started yet");
    log.start();
    log.title("Testing Logger Class", '=');
    log.write("\n one");
    log.write("\n two");
    log.write("\n three");
    log.stop();
    log.write("\n won't get logged - not started yet");
    log.start();
    log.write("\n starting again");
    log.stop("\n terminating now");
    log.wait();

    StaticLogger<1>::attach(&std::cout);
    StaticLogger<1>::attach(&out);
    StaticLogger<1>::start();
    StaticLogger<1>::write("\n");
    StaticLogger<1>::title("Testing SaticLogger class");
    StaticLogger<1>::write("\n static logger at work");
    Logger& logger = StaticLogger<1>::instance();
    logger.write("\n static logger still at work");
    StaticLogger<1>::write("\n static logger at work");

    for(size_t i=0; i<5; i++)
    {
        logger.write("\n a log message.");
    }
    logger.write("\n suspending logger");
    logger.pause();
    if(logger.paused())
    {
        logger.write("\n logger paused");
    }
    for(size_t i=0; i<5; i++)
    {
        logger.write("\n a log msg written while log suspend");
    }
    logger.resume();

    logger.write("\n a log msg written after log resumed");
    logger.stop("\n stopping static logger");
    logger.wait();
    out.close();

}

#endif



