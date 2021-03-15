//
//  CAny.hpp
//  ZJCrossCpp
//
//  Created by eafy on 2021/3/2.
//  Copyright © 2021 ZJ. All rights reserved.
//

#ifndef CAny_hpp
#define CAny_hpp

#include <stdio.h>
#include <type_traits>
 
class CAny {
public:
    CAny& operator=(const CAny& ref) = delete;
 
    CAny(): base_(nullptr) {}
 
    CAny(CAny&& ref):base_(nullptr) {
        base_ = ref.base_;
        ref.base_ = nullptr;
    };
    
    template<typename T>
    CAny(T t) {
        base_ = new Data<T>(std::forward<T>(t));
    }
 
    CAny& operator=(CAny&& ref) {
        if (base_)
            delete base_;
        base_ = ref.base_;
        ref.base_ = nullptr;
        return *this;
    };
 
    ~CAny() {
        if (base_)
            delete base_;
        base_ = nullptr;
    }
    const char* typeName() {
        if (base_)
            return base_->typeName();
        return nullptr;
    }
 
    void swap(CAny& oth) {
        Base* data = oth.base_;
        oth.base_ = base_;
        base_ = data;
    }
 
    template<typename T>
    void setValue(T t) {
        if (base_)
            delete base_;
        base_ = new Data<T>(std::forward<T>(t));
    }
 
    template<typename T>
    T  value() const {
        Data<T>* pdata = dynamic_cast<Data<T>*>(base_);
        if (pdata)
            return *pdata->t_;
        else if (std::is_pointer<T>())
            return nullptr;
        else
            return T();
    };
 
private:
    struct Base {
        virtual const char* typeName() = 0;
        virtual ~Base() {};
    };
 
 
    template<typename T>
    struct Data: public Base {
        Data(T t): t_(nullptr) {
            t_ = new T(std::forward<T>(t));
        };
        
        const char* typeName() {
            return typeid(T).name();
        }
 
        void setData(const T& t) { t_ = std::forward<T>(t); }
    
        T* t_;
    };
 
private:
    Base* base_;
};

#endif /* CAny_hpp */
