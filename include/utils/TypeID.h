
#ifndef TYPEID_H
#define TYPEID_H
/**
     * @brief This is a hack to be able to use switch case with type id inside of
     * 
     * @tparam T DataType to assign unique identifier to
     */
template<typename T>
struct TypeID {
    static constexpr size_t id() {
        return reinterpret_cast<size_t>(&uniqueInstance);
    }

private:
    inline static const char uniqueInstance{};// A unique instance for each type
};
#endif// TYPEID_H