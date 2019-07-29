#ifndef SRC_UTILS_HB_UTILS_H_
#define SRC_UTILS_HB_UTILS_H_



#include "os_dependency.h"          /* platform dependent definitions and includes */


#define ROUNDF(a)  ((int32_t)((a) + ((a) < 0.f ? -0.5f : 0.5f)))
#define SIGN(x)    ((x) >= 0 ? 1 : -1)
#define SIGN0(x)   ((x) > 0 ? 1 : ((x)==0 ? 0 : -1))

#define ARRAY_SIZE(array)   (sizeof(array)/sizeof(array[0]))


// see https://en.wikipedia.org/wiki/Q_(number_format)
// add 0.5 for rounding the unsigned

#define Q7_9_TO_FLOAT(x)   ((x * 64) / 32768.f)
#define FLOAT_TO_Q7_9(x)   ((int16_t)(x * 32768 / 64))

#define FLOAT_TO_UQ4_12(x)  ((uint16_t)(x * 4096.f + 0.5f))
#define UQ4_12_TO_FLOAT(x)  (x / 4096.f)
#define MAX_FLOAT_AT_UQ4_12 15.999755f

#define FLOAT_TO_UQ6_10(x)  (((uint16_t)(x * 1024.f + 0.5f)))
#define UQ6_10_TO_FLOAT(x)  (x / 1024.f)
#define MAX_FLOAT_AT_UQ6_10 63.999023f

#define FLOAT_TO_UQ8(x)     ((uint8_t)(x * 256.f + 0.5f))
#define UQ8_TO_FLOAT(x)     (x / 256.f)
#define MAX_FLOAT_AT_UQ8    0.996093f


// AK TODO: the following 3 FLT_xxx macros by C.Korikov have performance about 20-30 worse
//          than the regular comparisons. It seems to be a good idea to avoid their usage
//          wherever possible
// See Donald Knuth. Seminumerical Algorithms, volume 2 of The Art Of Computer Programming. AddisonWesley, 1997
#define FLT_MORE(a,b) (((a) - (b)) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * EPSILON6))
#define FLT_APPROX_EQ(a,b) (fabs((a) - (b)) <= ( (fabs(a) > fabs(b) ? fabs(b) : fabs(a)) * EPSILON6))
#define FLT_MORE_EQ(a,b) (FLT_MORE(a,b) || FLT_APPROX_EQ(a,b))


HB_INLINE int32_t limit_value_int(int32_t val, int32_t min_, int32_t max_)
{
  if(val <= min_)
    return min_;
  if (val >=max_)
    return max_;

  return val;
}

HB_INLINE float32_t limit_value_flt(float32_t val, float32_t min_, float32_t max_)
{
  if(val <= min_)
    return min_;
  if (val >=max_)
    return max_;

  return val;
}

HB_INLINE HB_FLOAT FABS(HB_FLOAT val)
{
  if(val >= 0.0f)
    return val;
  else
    return -val;
}


//returns true if 'value' is near to 'about_pt'
HB_INLINE int32_t about_eps_flt(float32_t value, float32_t about_pt, float32_t eps)
{
  float32_t diff = value - about_pt;

  return (diff < 0.0f ? -diff : diff) <= eps;
}

//returns true if 'value' is near to 'about_pt'
HB_INLINE int32_t about_eps_dbl(double value, double about_pt, double eps)
{
  double diff = value - about_pt;

  return (diff < 0.0f ? -diff : diff) <= eps;
}

/*************************************************************************
*  Format conversion functions
*************************************************************************/

uint8_t hexchar2nibble (uint8_t c);

uint8_t hex2byte_ (uint8_t* src_buf);

void byte2hex (uint8_t byte, uint8_t* dst_buf);

// short = 2 bytes
void short2hex (uint8_t* src_buf, uint8_t* dst_buf);
void hex2short (uint8_t* src_buf, uint8_t* dst_buf);

// long = 4 bytes
void long2hex (uint8_t* src_buf, uint8_t* dst_buf);
void hex2long (uint8_t* src_buf, uint8_t* dst_buf);

void bin2hex (uint8_t* src_buf, uint8_t* dst_buf, uint32_t bin_data_len);
void hex2bin (uint8_t* src_buf, uint8_t* dst_buf, uint32_t hex_data_len);

uint32_t is_hex_digit (uint8_t c);

void* int2ascii (int32_t i, void* dst_buf);

HB_INLINE uint32_t decToBcd(uint8_t val) {return ( (val/10*16) + (val%10) );}
HB_INLINE uint32_t bcdToDec(uint8_t val) {return ( (val/16*10) + (val%16) );}

char* addr2str(char* addr);

/* ==================================================================== */


/*************************************************************************
*  Various CRC-related functions
*************************************************************************/

uint16_t crc16_update(uint16_t crc, uint8_t data);
uint16_t crc16_update_bulk(uint16_t crc, uint8_t data[], int len);
uint16_t crc16(const uint8_t* buf, uint32_t len);
uint16_t crc16_table(uint8_t *adr_buffer, uint32_t byte_cnt);
void crc32(uint32_t* _crc, const uint8_t* _data, uint32_t _len);

/* ==================================================================== */


/*************************************************************************
*  Print buffer functions
*************************************************************************/

void print_buf(uint8_t* buf, uint32_t len, uint32_t add_new_line);
void print_vartype32(void* pval);
void print_buf_hex(uint8_t* buf, uint32_t len, uint32_t add_new_line);
void print_chars(uint8_t* buf, uint32_t len);
void print_rx_buff(uint8_t* buf, uint32_t len);

/* ==================================================================== */


/*************************************************************************
*  Power control functions
*************************************************************************/

void system_standby();
void system_shutdown();

/* ==================================================================== */

#endif /* SRC_UTILS_HB_UTILS_H_ */
