/* Generated by https://github.com/corsix/fast-crc32/ using: */
/* ./generate -i neon -p crc32 -a v3s4x2e_v2 */
/* MIT licensed */

#include <stddef.h>
#include <stdint.h>
#include <arm_acle.h>
#include <arm_neon.h>

#if defined(_MSC_VER)
#define CRC_AINLINE static __forceinline
#define CRC_ALIGN(n) __declspec(align(n))
#else
#define CRC_AINLINE static __inline __attribute__((always_inline))
#define CRC_ALIGN(n) __attribute__((aligned(n)))
#endif
#define CRC_EXPORT extern

CRC_AINLINE uint64x2_t clmul_lo_e(uint64x2_t a, uint64x2_t b, uint64x2_t c) {
  uint64x2_t r;
  __asm("pmull %0.1q, %2.1d, %3.1d\neor %0.16b, %0.16b, %1.16b\n" : "=w"(r), "+w"(c) : "w"(a), "w"(b));
  return r;
}

CRC_AINLINE uint64x2_t clmul_hi_e(uint64x2_t a, uint64x2_t b, uint64x2_t c) {
  uint64x2_t r;
  __asm("pmull2 %0.1q, %2.2d, %3.2d\neor %0.16b, %0.16b, %1.16b\n" : "=w"(r), "+w"(c) : "w"(a), "w"(b));
  return r;
}

CRC_AINLINE uint64x2_t clmul_scalar(uint32_t a, uint32_t b) {
  uint64x2_t r;
  __asm("pmull %0.1q, %1.1d, %2.1d\n" : "=w"(r) : "w"(vmovq_n_u64(a)), "w"(vmovq_n_u64(b)));
  return r;
}

static uint32_t xnmodp(uint64_t n) /* x^n mod P, in log(n) time */ {
  uint64_t stack = ~(uint64_t)1;
  uint32_t acc, low;
  for (; n > 191; n = (n >> 1) - 16) {
    stack = (stack << 1) + (n & 1);
  }
  stack = ~stack;
  acc = ((uint32_t)0x80000000) >> (n & 31);
  for (n >>= 5; n; --n) {
    acc = __crc32w(acc, 0);
  }
  while ((low = stack & 1), stack >>= 1) {
    poly8x8_t x = vreinterpret_p8_u64(vmov_n_u64(acc));
    uint64_t y = vgetq_lane_u64(vreinterpretq_u64_p16(vmull_p8(x, x)), 0);
    acc = __crc32d(0, y << low);
  }
  return acc;
}

CRC_AINLINE uint64x2_t crc_shift(uint32_t crc, size_t nbytes) {
  return clmul_scalar(crc, xnmodp(nbytes * 8 - 33));
}

CRC_EXPORT uint32_t crc32_impl(uint32_t crc0, const char* buf, size_t len) {
  crc0 = ~crc0;
  for (; len && ((uintptr_t)buf & 7); --len) {
    crc0 = __crc32b(crc0, *buf++);
  }
  if (((uintptr_t)buf & 8) && len >= 8) {
    crc0 = __crc32d(crc0, *(const uint64_t*)buf);
    buf += 8;
    len -= 8;
  }
  if (len >= 112) {
    const char* end = buf + len;
    size_t blk = (len - 0) / 112;
    size_t klen = blk * 16;
    const char* buf2 = buf + klen * 4;
    const char* limit = buf + klen - 32;
    uint32_t crc1 = 0;
    uint32_t crc2 = 0;
    uint32_t crc3 = 0;
    uint64x2_t vc0;
    uint64x2_t vc1;
    uint64x2_t vc2;
    uint64x2_t vc3;
    uint64_t vc;
    /* First vector chunk. */
    uint64x2_t x0 = vld1q_u64((const uint64_t*)buf2), y0;
    uint64x2_t x1 = vld1q_u64((const uint64_t*)(buf2 + 16)), y1;
    uint64x2_t x2 = vld1q_u64((const uint64_t*)(buf2 + 32)), y2;
    uint64x2_t k;
    { static const uint64_t CRC_ALIGN(16) k_[] = {0x3db1ecdc, 0xaf449247}; k = vld1q_u64(k_); }
    buf2 += 48;
    /* Main loop. */
    while (buf <= limit) {
      y0 = clmul_lo_e(x0, k, vld1q_u64((const uint64_t*)buf2)), x0 = clmul_hi_e(x0, k, y0);
      y1 = clmul_lo_e(x1, k, vld1q_u64((const uint64_t*)(buf2 + 16))), x1 = clmul_hi_e(x1, k, y1);
      y2 = clmul_lo_e(x2, k, vld1q_u64((const uint64_t*)(buf2 + 32))), x2 = clmul_hi_e(x2, k, y2);
      crc0 = __crc32d(crc0, *(const uint64_t*)buf);
      crc1 = __crc32d(crc1, *(const uint64_t*)(buf + klen));
      crc2 = __crc32d(crc2, *(const uint64_t*)(buf + klen * 2));
      crc3 = __crc32d(crc3, *(const uint64_t*)(buf + klen * 3));
      crc0 = __crc32d(crc0, *(const uint64_t*)(buf + 8));
      crc1 = __crc32d(crc1, *(const uint64_t*)(buf + klen + 8));
      crc2 = __crc32d(crc2, *(const uint64_t*)(buf + klen * 2 + 8));
      crc3 = __crc32d(crc3, *(const uint64_t*)(buf + klen * 3 + 8));
      buf += 16;
      buf2 += 48;
    }
    /* Reduce x0 ... x2 to just x0. */
    { static const uint64_t CRC_ALIGN(16) k_[] = {0xae689191, 0xccaa009e}; k = vld1q_u64(k_); }
    y0 = clmul_lo_e(x0, k, x1), x0 = clmul_hi_e(x0, k, y0);
    x1 = x2;
    y0 = clmul_lo_e(x0, k, x1), x0 = clmul_hi_e(x0, k, y0);
    /* Final scalar chunk. */
    crc0 = __crc32d(crc0, *(const uint64_t*)buf);
    crc1 = __crc32d(crc1, *(const uint64_t*)(buf + klen));
    crc2 = __crc32d(crc2, *(const uint64_t*)(buf + klen * 2));
    crc3 = __crc32d(crc3, *(const uint64_t*)(buf + klen * 3));
    crc0 = __crc32d(crc0, *(const uint64_t*)(buf + 8));
    crc1 = __crc32d(crc1, *(const uint64_t*)(buf + klen + 8));
    crc2 = __crc32d(crc2, *(const uint64_t*)(buf + klen * 2 + 8));
    crc3 = __crc32d(crc3, *(const uint64_t*)(buf + klen * 3 + 8));
    vc0 = crc_shift(crc0, klen * 3 + blk * 48);
    vc1 = crc_shift(crc1, klen * 2 + blk * 48);
    vc2 = crc_shift(crc2, klen + blk * 48);
    vc3 = crc_shift(crc3, 0 + blk * 48);
    vc = vgetq_lane_u64(veorq_u64(veorq_u64(vc0, vc1), veorq_u64(vc2, vc3)), 0);
    /* Reduce 128 bits to 32 bits, and multiply by x^32. */
    crc0 = __crc32d(0, vgetq_lane_u64(x0, 0));
    crc0 = __crc32d(crc0, vc ^ vgetq_lane_u64(x0, 1));
    buf = buf2;
    len = end - buf;
  }
  if (len >= 32) {
    /* First vector chunk. */
    uint64x2_t x0 = vld1q_u64((const uint64_t*)buf), y0;
    uint64x2_t x1 = vld1q_u64((const uint64_t*)(buf + 16)), y1;
    uint64x2_t k;
    { static const uint64_t CRC_ALIGN(16) k_[] = {0xf1da05aa, 0x81256527}; k = vld1q_u64(k_); }
    x0 = veorq_u64((uint64x2_t){crc0, 0}, x0);
    buf += 32;
    len -= 32;
    /* Main loop. */
    while (len >= 32) {
      y0 = clmul_lo_e(x0, k, vld1q_u64((const uint64_t*)buf)), x0 = clmul_hi_e(x0, k, y0);
      y1 = clmul_lo_e(x1, k, vld1q_u64((const uint64_t*)(buf + 16))), x1 = clmul_hi_e(x1, k, y1);
      buf += 32;
      len -= 32;
    }
    /* Reduce x0 ... x1 to just x0. */
    { static const uint64_t CRC_ALIGN(16) k_[] = {0xae689191, 0xccaa009e}; k = vld1q_u64(k_); }
    y0 = clmul_lo_e(x0, k, x1), x0 = clmul_hi_e(x0, k, y0);
    /* Reduce 128 bits to 32 bits, and multiply by x^32. */
    crc0 = __crc32d(0, vgetq_lane_u64(x0, 0));
    crc0 = __crc32d(crc0, vgetq_lane_u64(x0, 1));
  }
  for (; len >= 8; buf += 8, len -= 8) {
    crc0 = __crc32d(crc0, *(const uint64_t*)buf);
  }
  for (; len; --len) {
    crc0 = __crc32b(crc0, *buf++);
  }
  return ~crc0;
}


// Export the function for Go to use
#ifdef __cplusplus
extern "C" {
#endif

uint32_t crc32_neon_update(uint32_t crc, const char* buf, size_t len) {
    return crc32_impl(crc, buf, len);
}

#ifdef __cplusplus
}
#endif
