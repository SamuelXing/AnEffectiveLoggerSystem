//
// Created by samuel on 11/2/17.
//
/*
 * Purpose:
 * This package supports logging for multiple concurrent clients to a
 * single std::ostream. It does this be enqueuing messages in a blocking
 * queue and dequeuing with a single thread that writes to the
 * std::ostream
 * */

#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <pthread.h>
#include "./BlockingQueue.h"

namespace Logging
{
    class Logger
    {
    public:
        Logger(){}
        void attach(std::ostream* pOut);
        void start();
        bool running();
        void pause();
        void resume();
        bool paused();
        void flush();
        void stop(const std::string& msg = "");
        void wait();
        void write(const std::string& msg);
        void title(const std::string& msg, char underline = '-');
        ~Logger();
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

    private:
        std::thread* _pThr;
        std::vector<std::ostream*> streams_;
        Async::BlockingQueue<std::string> _queue;
        bool _ThreadingRunning = false;
        bool _Paused = false;
        pthread_mutex_t mtx_;
        pthread_cond_t cond_;
    };

    template<int i>
    class StaticLogger
    {
    public:
        static void attach(std::ostream* pOut) {_logger.attach(pOut); }
        static void start() {_logger.start(); }
        static bool running() { return _logger.running();}
        static void pause() { _logger.pause(); }
        static void resume() {_logger.resume();}
        static bool paused() { return _logger.paused(); }
        static void flush() {_logger.flush(); }
        static void stop(const std::string& msg = "") {_logger.stop(msg); }
        static void wait() {_logger.wait(); }
        static void write(const std::string& msg){_logger.write(msg); }
        static void title(const std::string& msg, char underline = '-') {_logger.title(msg, underline);}
        static Logger& instance(){ return _logger; }
        StaticLogger(const StaticLogger&) = delete;
        StaticLogger& operator=(const StaticLogger&) = delete;
    private:
        static Logger _logger;
    };

    template<int i>
    Logger StaticLogger<i>::_logger;

    using Rslt = Logging::StaticLogger<0>;
    using Demo = Logging::StaticLogger<1>;
    using Dbug = Logging::StaticLogger<2>;

}



#endif //ANEFFECTIVELOGGERSYSTEM_LOGGER_H
