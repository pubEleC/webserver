#pragma once
#include <random>
#include <unordered_map>



//uniform_real_distribution
//normal_distribution
struct PairHash {
    template <typename T1, typename T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        auto hash1 = std::hash<T1>{}(p.first);
        auto hash2 = std::hash<T2>{}(p.second);
        // 简单组合哈希（可采用更复杂的混合方式）
        return hash1 ^ (hash2 << 1);
    }
};

template<typename T>
class UniformIntRandomOpt{
public:
    UniformIntRandomOpt(std::random_device& rd){
        gen = std::mt19937(rd());
    }
    UniformIntRandomOpt(std::random_device& rd,std::pair<T,T> p){
        gen = std::mt19937(rd());
        createRandom(p); 
    }

    T getValue(std::pair<T,T> p){
        if(ump.count(p)){
            return ump[p](gen);
        }
        else{
            createRandom(p);
            return getValue(p);
        }
    }

    void createRandom(std::pair<T,T> p){
        std::uniform_int_distribution<T> temp = std::uniform_int_distribution<T>(p.first,p.second);
        ump[p] = temp;    
    }

public:
    std::unordered_map<std::pair<T,T>,std::uniform_int_distribution<T>,PairHash> ump;
    std::mt19937 gen;
};


template<typename T>
class UniformRealRandomOpt{
public:
    UniformRealRandomOpt(std::random_device& rd){
        gen = std::mt19937(rd());
    }
    UniformRealRandomOpt(std::random_device& rd,std::pair<T,T> p){
        gen = std::mt19937(rd());
        createRandom(p); 
    }

    T getValue(std::pair<T,T> p){
        if(ump.count(p)){
            return ump[p](gen);
        }
        else{
            createRandom(p);
            return getValue(p);
        }
    }

    void createRandom(std::pair<T,T> p){
        std::uniform_real_distribution<T> temp = std::uniform_real_distribution<T>(p.first,p.second);
        ump[p] = temp;    
    }

public:
    std::unordered_map<std::pair<T,T>,std::uniform_real_distribution<T>,PairHash> ump;
    std::mt19937 gen;
};


template<typename T>
class NormalRandomOpt{
public:
    NormalRandomOpt(std::random_device& rd){
        gen = std::mt19937(rd());
    }
    NormalRandomOpt(std::random_device& rd,std::pair<T,T> p){
        gen = std::mt19937(rd());
        createRandom(p); 
    }

    T getValue(std::pair<T,T> p){
        if(ump.count(p)){
            return ump[p](gen);
        }
        else{
            createRandom(p);
            return getValue(p);
        }
    }

    void createRandom(std::pair<T,T> p){
        std::normal_distribution<T> temp = std::normal_distribution<T>(p.first,p.second);
        ump[p] = temp;    
    }

public:
    std::unordered_map<std::pair<T,T>,std::normal_distribution<T>,PairHash> ump;
    std::mt19937 gen;
};


