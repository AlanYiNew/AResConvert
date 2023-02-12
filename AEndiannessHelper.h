#ifndef _PORTABLE_ENDIAN_H__
#define _PORTABLE_ENDIAN_H__

#include <cstddef>
#include <iostream>
#if (defined(_WIN16) || defined(_WIN32) || defined(_WIN64)) && !defined(__WINDOWS__)

#	define __WINDOWS__

#endif

#if defined(__linux__) || defined(__CYGWIN__)

#	include <endian.h>

#elif defined(__APPLE__)

#	include <libkern/OSByteOrder.h>

#	define htobe16(x) OSSwapHostToBigInt16(x)
#	define htole16(x) OSSwapHostToLittleInt16(x)
#	define be16toh(x) OSSwapBigToHostInt16(x)
#	define le16toh(x) OSSwapLittleToHostInt16(x)

#	define htobe32(x) OSSwapHostToBigInt32(x)
#	define htole32(x) OSSwapHostToLittleInt32(x)
#	define be32toh(x) OSSwapBigToHostInt32(x)
#	define le32toh(x) OSSwapLittleToHostInt32(x)

#	define htobe64(x) OSSwapHostToBigInt64(x)
#	define htole64(x) OSSwapHostToLittleInt64(x)
#	define be64toh(x) OSSwapBigToHostInt64(x)
#	define le64toh(x) OSSwapLittleToHostInt64(x)

#	define __BYTE_ORDER    BYTE_ORDER
#	define __BIG_ENDIAN    BIG_ENDIAN
#	define __LITTLE_ENDIAN LITTLE_ENDIAN
#	define __PDP_ENDIAN    PDP_ENDIAN

#elif defined(__OpenBSD__)

#	include <sys/endian.h>

#elif defined(__NetBSD__) || defined(__FreeBSD__) || defined(__DragonFly__)

#	include <sys/endian.h>

#	define be16toh(x) betoh16(x)
#	define le16toh(x) letoh16(x)

#	define be32toh(x) betoh32(x)
#	define le32toh(x) letoh32(x)

#	define be64toh(x) betoh64(x)
#	define le64toh(x) letoh64(x)

#elif defined(__WINDOWS__)

#	include <winsock2.h>
#	include <sys/param.h>

inline uint64_t htonll(uint64_t val)
{
	if (1 == htonl(1))
		return val;
	return (((uint64_t)htonl(val)) << 32) + htonl(val >> 32);
}
 
inline uint64_t ntohll(uint64_t val)
{
	if (1 == htonl(1))
		return val;
	return (((uint64_t)ntohl(val)) << 32) + ntohl(val >> 32);
}

#	if BYTE_ORDER == LITTLE_ENDIAN

#		define htobe16(x) htons(x)
#		define htole16(x) (x)
#		define be16toh(x) ntohs(x)
#		define le16toh(x) (x)

#		define htobe32(x) htonl(x)
#		define htole32(x) (x)
#		define be32toh(x) ntohl(x)
#		define le32toh(x) (x)

#		define htobe64(x) htonll(x)
#		define htole64(x) (x)
#		define be64toh(x) ntohll(x)
#		define le64toh(x) (x)

#	elif BYTE_ORDER == BIG_ENDIAN

		/* that would be xbox 360 */
#		define htobe16(x) (x)
#		define htole16(x) __builtin_bswap16(x)
#		define be16toh(x) (x)
#		define le16toh(x) __builtin_bswap16(x)

#		define htobe32(x) (x)
#		define htole32(x) __builtin_bswap32(x)
#		define be32toh(x) (x)
#		define le32toh(x) __builtin_bswap32(x)

#		define htobe64(x) (x)
#		define htole64(x) __builtin_bswap64(x)
#		define be64toh(x) (x)
#		define le64toh(x) __builtin_bswap64(x)

#	else

#		error byte order not supported

#	endif
#	define __BYTE_ORDER    BYTE_ORDER
#	define __BIG_ENDIAN    BIG_ENDIAN
#	define __LITTLE_ENDIAN LITTLE_ENDIAN
#	define __PDP_ENDIAN    PDP_ENDIAN
#else

#	error platform not supported

#endif

inline int32_t WriteUInt32(void* f, uint32_t val) {
    (*(uint32_t*)f) = htole32(val);
    return sizeof(val);
}

inline int32_t WriteInt32(void* f, int32_t val) {
    (*(int32_t*)f) = htole32(val);
    return sizeof(val);
}

inline int32_t WriteInt16(void* f, int16_t val) {
    (*(int16_t*)f) = htole16(val);
    return sizeof(val);
}

inline int32_t WriteUInt16(void* f, uint16_t val) {
    (*(uint16_t*)f) = htole16(val);
    return sizeof(val);
}

inline int32_t WriteInt64(void* f, int64_t val) {
    (*(int64_t*)f) = htole64(val);
    return sizeof(val);
}

inline int32_t WriteUInt64(void* f, uint64_t val) {
    (*(uint64_t*)f) = htole64(val);
    return sizeof(val);
}

inline int32_t WriteInt8(void* f, int8_t val) {
    (*(int8_t*)f) = val;
    return sizeof(val);
}

inline int32_t WriteUInt8(void* f, uint8_t val) {
    (*(uint8_t*)f) = val;
    return sizeof(val);
}

inline int32_t WriteDouble(void* f, double val) {
    (*(double*)f) = htole64(val);
    return sizeof(val);
}

inline int32_t WriteFloat(void* f, float val) {
    (*(float*)f) = htole32(val);
    return sizeof(val);
}

inline int32_t WriteString(void* f, const std::string& val) {
    strcpy((char*)f, val.c_str());
    return val.size() + 1;
}

inline int32_t ReadUInt32(void* f, uint32_t& val) {
    val = le32toh(*(uint32_t*) f);
    return sizeof(val);
}

inline int32_t ReadInt32(void* f, int32_t& val) {
    val = le32toh(*(int32_t*) f);
    return sizeof(val);
}

inline int32_t ReadInt16(void* f, int16_t& val) {
    val = le16toh(*(int16_t*) f);
    return sizeof(val);
}

inline int32_t ReadUInt16(void* f, uint16_t& val) {
    val = le16toh(*(uint16_t*) f);
    return sizeof(val);
}

inline int32_t ReadInt64(void* f, int64_t& val) {
    val = le64toh(*(int64_t*) f);
    return sizeof(val);
}

inline int32_t ReadUInt64(void* f, uint64_t& val) {
    val = le64toh(*(uint64_t*) f);
    return sizeof(val);
}

inline int32_t ReadInt8(void* f, int8_t& val) {
    val = *(int8_t*) f;
    return sizeof(val);
}

inline int32_t ReadUInt8(void* f, uint8_t& val) {
    val = *(uint8_t*) f;
    return sizeof(val);
}

inline int32_t ReadDouble(void* f, double& val) {
    val = le64toh(*(double*) f);
    return sizeof(val);
}

inline int32_t ReadFloat(void* f, float& val) {
    val = le32toh(*(float*) f);
    return sizeof(val);
}

inline int32_t ReadString(void* f, std::string& val) {
    val = std::string((char*)f);
    return val.size() + 1;
}

inline int32_t WriteInt32(std::FILE* f, int32_t val) {
    val = htole32(val);
    std::fwrite(&val, sizeof(val), 1, f);
    return sizeof(val);
}

inline int32_t WriteUInt32(std::FILE* f, uint32_t val) {
    val = htole32(val);
    std::fwrite(&val, sizeof(val), 1 , f);
    return sizeof(val);
}

inline int32_t WriteInt16(std::FILE* f, int16_t val) {
    val = htole16(val);
    std::fwrite(&val, sizeof(val), 1, f);
    return sizeof(val);
}

inline int32_t WriteUInt16(std::FILE* f, uint16_t val) {
    val = htole16(val);
    std::fwrite(&val, sizeof(val), 1, f);
    return sizeof(val);
}

inline int32_t WriteInt64(std::FILE* f, int64_t val) {
    val = htole64(val);
    std::fwrite(&val, sizeof(val), 1,  f);
    return sizeof(val);
}

inline int32_t WriteUInt64(std::FILE* f, uint64_t val) {
    val = htole64(val);
    std::fwrite(&val, sizeof(val), 1, f);
    return sizeof(val);
}

inline int32_t WriteInt8(std::FILE* f, int8_t val) {
    std::fwrite(&val, sizeof(val), 1,f);
    return sizeof(val);
}

inline int32_t WriteUInt8(std::FILE* f, uint8_t val) {
    std::fwrite(&val, sizeof(val), 1, f);
    return sizeof(val);
}

inline int32_t WriteDouble(std::FILE* f, double val) {
    val = htole64(val);
    std::fwrite(&val, sizeof(val), 1, f);
    return sizeof(val);
}

inline int32_t WriteFloat(std::FILE* f, float val) {
    val = htole32(val);
    std::fwrite(&val, sizeof(val), 1, f);
    return sizeof(val);
}

inline int32_t WriteString(std::FILE* f, const std::string& val) {
    std::fwrite(val.c_str(), 1, val.size() + 1, f);
    return val.size() + 1;
}

inline int32_t WriteBytes(std::FILE* f, const std::string& val) {
    std::fwrite(val.c_str(), 1, val.size(), f);
    return val.size();
}

inline int32_t WriteBytes(std::FILE* f, char* buffer, int32_t size) {
    std::fwrite(buffer, 1, size, f);
    return size;
}

inline bool ReadInt32(std::FILE* f, int32_t& val) {
    val = 0;
    auto x = std::fread(&val, sizeof(val), 1, f);
    if (x == 1) {
       val = le32toh(val);
       return true;
    }
    return false;
}

inline bool ReadUInt32(std::FILE* f, uint32_t& val) {
    val = 0;
    auto x = std::fread(&val, sizeof(val), 1 , f);
    if (x == 1) {
       val = le32toh(val);
       return true;
    }
    return false;
}

inline bool ReadInt16(std::FILE* f, int16_t& val) {
    val = 0;
    auto x = std::fread(&val, sizeof(val), 1, f);
    if (x == 1) {
       val = le16toh(val);
       return true;
    }
    return false;
}

inline bool ReadUInt16(std::FILE* f, uint16_t& val) {
    val = 0;
    auto x = std::fread(&val, sizeof(val), 1, f);
    if (x == 1) {
       val = le16toh(val);
       return true;
    }
    return false;
;
}

inline bool ReadInt64(std::FILE* f, int64_t& val) {
    val = 0;
    auto x = std::fread(&val, sizeof(val), 1,  f);
    if (x == 1) {
       val = le64toh(val);
       return true;
    }
    return false;
}

inline bool ReadUInt64(std::FILE* f, uint64_t& val) {
    val = 0;
    auto x = std::fread(&val, sizeof(val), 1, f);
    if (x == 1) {
       val = le64toh(val);
       return true;
    }
    return false;
}

inline bool ReadInt8(std::FILE* f, int8_t& val) {
    val = 0;
    auto x = std::fread(&val, sizeof(val), 1, f);
    if (x == 1) {
        return true;   
    }
    return false;
}

inline bool ReadUInt8(std::FILE* f, uint8_t& val) {
    val = 0;
    auto x = std::fread(&val, sizeof(val), 1, f);
    if (x == 1) {
        return true;   
    }
    return false;
}

inline bool ReadDouble(std::FILE* f, double& val) {
    val = 0;
    auto x = std::fread(&val, sizeof(val), 1, f);
    if (x == 1) {
        val = le64toh(val);
        return true;   
    }
    return false;
}

inline bool ReadFloat(std::FILE* f, float& val) {
    val = 0;
    auto x = std::fread(&val, sizeof(val), 1, f);
    if (x == 1) {
        val = le32toh(val);
        return true;
    }
    return false;
}

inline bool ReadBytes(std::FILE* f, void* buffer, int32_t size) {
    auto x = std::fread(buffer, 1, size, f);
    if (x == size) {
        return true;
    }
    return false;
}
#endif
