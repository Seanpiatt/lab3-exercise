#ifndef SHARED_PTR_HEADER
#define SHARED_PTR_HEADER
#include <utility>
#include <cassert>

class ControlBlockBase {
public:
    ControlBlockBase(): mRefCount(1) {} // TODO: implement the default constructor.

    // dtor is virtual, so that we can call derived class's dtor from a ptr to this base class.
    virtual ~ControlBlockBase( ) {}// TODO: implement the destructor.

    // pure virtual function; must be overriden by derived classes
    virtual void* managedAddress() = 0;

    // Delete copies, which also implicitly deletes moves.
    ControlBlockBase(const ControlBlockBase&) = delete;
    ControlBlockBase& operator=(const ControlBlockBase&) = delete;

    long increment()
    {
        // TODO: increment refcount by 1 and return result.
        mRefCount++;
        return mRefCount;
    }

    long decrement()
    {
        // TODO: decrement refcount by 1 and return result.
        mRefCount--;
        return mRefCount;
    }

    long refCount() const
    {
        // TODO: just return the refcount.
        return mRefCount;
    }

private:
    long mRefCount;
};

template <typename T>
class ControlBlockBasic : public ControlBlockBase {
public:
    //Constructor
    ControlBlockBasic(T* something) : mSomething(something) {}

    //Override
    void* managedAddress() override {
        return mSomething;
    }
    
    ~ControlBlockBasic() {delete mSomething;}

    ControlBlockBasic(const ControlBlockBasic&) = delete;
    ControlBlockBasic& operator=(const ControlBlockBasic&) = delete;


private:
    T* mSomething;
};


template <typename T>
class SharedPtr {
public:
    SharedPtr() : mItem(nullptr), mControlBlock(nullptr) {}
    explicit SharedPtr(T* item) : mItem(item), mControlBlock(new ControlBlockBasic<T>(item)) {}
    ~SharedPtr() {
        if(mControlBlock!= nullptr){
            if (mControlBlock->decrement() == 0){
            
                delete mControlBlock;
            }
        }

    }

    SharedPtr& operator=(const SharedPtr& other) {
        if (this != &other ) {
            if(mControlBlock != nullptr && this->mControlBlock->decrement()== 0){
                delete this->mControlBlock;
            }
            this->mItem = other.mItem;
            this->mControlBlock = other.mControlBlock;

            this->mControlBlock->increment();
        }
        return *this;
    }
    SharedPtr& operator=(SharedPtr&& other) noexcept {
        if (this != &other) {
            if (mControlBlock != nullptr && mControlBlock->decrement() == 0) {
                delete mControlBlock;
            }
            this->mItem = other.mItem;
            this->mControlBlock = other.mControlBlock;
            other.mItem = nullptr;
            other.mControlBlock = nullptr;
        }
        return *this;
    }

    SharedPtr(const SharedPtr& other): mItem(other.mItem), mControlBlock(other.mControlBlock){
        mControlBlock->increment();
    }

    SharedPtr(SharedPtr&& other) noexcept : mItem(other.mItem), mControlBlock(other.mControlBlock) {
        other.mItem = nullptr;
        other.mControlBlock = nullptr;
    }


    T& operator*() const {
        assert(mItem != nullptr);
        return *mItem;
    }

    T* operator->() const {
        assert(mItem != nullptr);
        return mItem;
    }

    T* get() const {
        return mItem;
    }
    
    long useCount() const{
        if (mControlBlock != nullptr) {
            return mControlBlock->refCount();
        }
        else{
            return 0;
        }
        
    }

    bool operator==(const SharedPtr& other) const {
        return mItem == other.mItem;
    }

    void reset(T* newPtr = nullptr) {
        if (mControlBlock != nullptr && mControlBlock->decrement() == 0) {
            delete mControlBlock;
        }
        mItem = newPtr;  
        mControlBlock = new ControlBlockBasic<T>(newPtr);
        
    }

    void swap(SharedPtr& other) noexcept {
        T* temp = mItem;
        ControlBlockBase* temp2 = mControlBlock;
        mItem = other.mItem;
        mControlBlock = other.mControlBlock;
        other.mItem = temp;
        other.mControlBlock = temp2;
    }

    explicit operator bool() const {
        return mItem != nullptr;
    }

private:
    T* mItem;
    ControlBlockBase* mControlBlock;
};

template <typename T, typename... Args>
SharedPtr<T> makeSharedBasic(Args&&... args) {
    return SharedPtr<T>(new T(std::forward<Args>(args)...));
}

#endif
