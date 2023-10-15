#ifndef IR_H
#define IR_H
#include <cstdint>
#include <cstddef>

int32_t IrWriteSomethingInt(const uint8_t* buf, size_t len);

template<typename T>
int32_t IrWriteSomething(const T* data, size_t len) {
    static_assert(sizeof(T) == 1);
    return IrWriteSomethingInt(reinterpret_cast<const uint8_t*>(data), len);
}

int32_t IrReadSomethingInt(uint8_t* out, size_t len);

template<typename T>
int32_t IrReadSomething(T* out, size_t len) {
    static_assert(sizeof(T) == 1);
    return IrReadSomethingInt(reinterpret_cast<uint8_t*>(out), len);
}

void IrInit();
void IrDeInit();

#endif //IR_H
