#ifndef SHARED_PTR_HEADER
#define SHARED_PTR_HEADER

class ControlBlockBase {
public:
    ControlBlockBase(); // TODO: implement the default constructor.

    // dtor is virtual, so that we can call derived class's dtor from a ptr to this base class.
    virtual ~ControlBlockBase(); // TODO: implement the destructor.

    // pure virtual function; must be overriden by derived classes
    virtual void* managedAddress() = 0;

    // Delete copies, which also implicitly deletes moves.
    ControlBlockBase(const ControlBlockBase&) = delete;
    ControlBlockBase& operator=(const ControlBlockBase&) = delete;

    long increment()
    {
        // TODO: increment refcount by 1 and return result.
    }

    long decrement()
    {
        // TODO: decrement refcount by 1 and return result.
    }

    long refCount() const
    {
        // TODO: just return the refcount.
    }

private:
    // TODO: add field(s) which both control block types need to have
};

template <typename T>
class ShredPtr {
public:
    ShredPtr() : mItem(nullptr) {}
    explicit ShredPtr(T* item) : mItem(item) {}
    ~ShredPtr() { delete mItem; }

    ShredPtr(const ShredPtr&) = delete;
    ShredPtr& operator=(const ShredPtr&) = delete;

    ShredPtr(ShredPtr&& other) noexcept : mItem(other.mItem) {
        other.mItem = nullptr;
    }

    ShredPtr& operator=(ShredPtr&& other) noexcept {
        if (this != &other) {
            delete mItem;
            mItem = other.mItem;
            other.mItem = nullptr;
        }
        return *this;
    }

    T* release() {
        T* temp = mItem;
        mItem = nullptr;
        return temp;
    }

    template <typename U>
    ShredPtr(ShredPtr<U>&& other) : mItem(other.release()) {}

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

    bool operator==(const ShredPtr& other) const {
        return mItem == other.mItem;
    }

    void reset(T* newPtr = nullptr) {
        T* old = mItem;
        mItem = newPtr;
        delete old;
    }

    void swap(ShredPtr& other) noexcept {
        T* temp = mItem;
        mItem = other.mItem;
        other.mItem = temp;
    }

    explicit operator bool() const {
        return mItem != nullptr;
    }

private:
    T* mItem;
};

template <typename T, typename... Args>
ShredPtr<T> makeUnique(Args&&... args) {
    return ShredPtr<T>(new T(std::forward<Args>(args)...));
}

#endif
