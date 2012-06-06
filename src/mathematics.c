#include "mathematics.h"

inline int int_abs(int a){
    if(a < 0){
        return -a;
    }else{
        return a;
    }
}

inline double double_abs(double a){
    if(a < 0){
        return -a;
    }else{
        return a;
    }
}


inline double double_min(double a, double b){
    if(a < b){
        return a;
    }else{
        return b;
    }
}

inline double double_max(double a, double b){
    if(a > b){
        return a;
    }else{
        return b;
    }
}

inline short int short_min(short int a, short int b){
    if(a < b){
        return a;
    }else{
        return b;
    }
}

inline short int short_max(short int a, short int b){
    if(a > b){
        return a;
    }else{
        return b;
    }
}
