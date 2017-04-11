#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Format.h"

using namespace llvm;

/// Utility function to encode a ULEB128 value to a buffer. Returns
/// the length in bytes of the encoded value.
unsigned encodeULEB128(uint64_t Value, uint8_t *p,
                              unsigned Padding = 0) {
  uint8_t *orig_p = p;
  do {
    uint8_t Byte = Value & 0x7f;
    Value >>= 7;
    if (Value != 0 || Padding != 0)
      Byte |= 0x80; // Mark this byte to show that more bytes will follow.
    *p++ = Byte;
  } while (Value != 0);

  // Pad with 0x80 and emit a null byte at the end.
  if (Padding != 0) {
    for (; Padding != 1; --Padding)
      *p++ = '\x80';
    *p++ = '\x00';
  }
  return (unsigned)(p - orig_p);
}

/// Utility function to encode a ULEB128 value to an output stream.
inline void encodeULEB128(uint64_t Value, raw_ostream &OS,
                          unsigned Padding = 0) {
  do {
    uint8_t Byte = Value & 0x7f;
    Value >>= 7;
    if (Value != 0 || Padding != 0)
      Byte |= 0x80; // Mark this byte to show that more bytes will follow.
    OS << format_hex(Byte, 2) << " ";
  } while (Value != 0);

  // Pad with 0x80 and emit a null byte at the end.
  if (Padding != 0) {
    for (; Padding != 1; --Padding)
      OS << '\x80';
    OS << '\x00';
  }
}

inline uint64_t decodeULEB128(const uint8_t *p, unsigned *n = nullptr,
                              const uint8_t *end = nullptr,
                              const char **error = nullptr) {
  const uint8_t *orig_p = p;
  uint64_t Value = 0;
  unsigned Shift = 0;
  if(error)
    *error = nullptr;
  do {
    if(end && p == end){
      if(error)
        *error = "malformed uleb128, extends past end";
      if (n)
        *n = (unsigned)(p - orig_p);
      return 0;
    }
    uint64_t Slice = *p & 0x7f;
    if(Shift >= 64 || Slice << Shift >> Shift != Slice){
      if(error)
        *error = "uleb128 too big for uint64";
      if (n)
        *n = (unsigned)(p - orig_p);
      return 0;
    }
    Value += uint64_t(*p & 0x7f) << Shift;
    Shift += 7;
  } while (*p++ >= 128);
  if (n)
    *n = (unsigned)(p - orig_p);
  return Value;
}

int main() {
  uint64_t Value = 624485;
  llvm::errs() << format_hex(Value, 2) << "\n";

  uint64_t Result = 0;
  encodeULEB128(Value, (uint8_t *)&Result);
  llvm::errs() << format_hex(Result, 2) << "\n\n";

  encodeULEB128(Value, llvm::errs());

  llvm::errs() <<  "\n" << format_hex(decodeULEB128((uint8_t *)&Result), 2) << "\n";

  return 0;
}
