#pragma once
#ifndef CRBN_JOBMANAGER_TPP
#define CRBN_JOBMANAGER_TPP

#include "crbn_jobManager.hpp"

// namespace crbn
namespace crbn
{
    template <typename T>
    Variable_Mutex<T>::Variable_Mutex(const T data)
        : _variable(data) { }

    template <typename T>
    Variable_Mutex<T>::Variable_Mutex()
        // : _variable(T())
    {
    }

    ///template <typename T>
    ///Variable_Mutex<T>::Variable_Mutex(T && dataInput)
    ///    : _variable(std::move(dataInput))
    ///{
    ///}

    template <typename T>
    Variable_Mutex<T>::~Variable_Mutex() { }
    
    template <typename T>
    const T Variable_Mutex<T>::operator()() const 
    {
        std::unique_lock<std::mutex> lock = _lock();
        return _variable;
    }

    template <typename T>
    Variable_Mutex<T> &Variable_Mutex<T>::operator=(const T & data)
    {
        std::unique_lock<std::mutex> lock = _lock();
        _variable = data;
        return *this;
    }
    template <typename T> 
    template <typename Lambda>
    auto Variable_Mutex<T>::read(Lambda lambda) const
    {
        std::unique_lock<std::mutex> lock = _lock();
        return lambda(_variable);
    }
    template <typename T>
    template <typename Lambda>
    auto Variable_Mutex<T>::write(Lambda lambda) 
    {
        std::unique_lock<std::mutex> lock = _lock();
        return lambda(_variable);
    }


    // PRIVATE
    template <typename T>
    auto Variable_Mutex<T>::_lock() { return std::unique_lock<std::mutex>(_mutex); }

    template <typename T>
    auto Variable_Mutex<T>::_lock() const { return std::unique_lock<std::mutex>(_mutex); }

}
#endif