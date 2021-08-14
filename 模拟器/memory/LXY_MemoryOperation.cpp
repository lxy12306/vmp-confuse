#include "LXY_MemoryOperation.h"
#include "LXY_malloc.h"
#if _WIN64
#else

extern "C"  unsigned char* m_lxy_memcmp32_aligned(void* rhs, void* lhs, size_t size);
extern "C"  unsigned char* m_lxy_memcmp32_noaligned(void* rhs, void* lhs, size_t size);
extern "C"  uint16_t m_lxy_internet_checksum32(void* source, size_t size);

std::shared_ptr<unsigned char> lxy_memcmp(void* rhs, void* lhs, size_t size)
{
	if((size_t(rhs)&0x3f) == 0 && (size_t(lhs)&0x3f) == 0)
		return std::shared_ptr<unsigned char>(m_lxy_memcmp32_aligned(rhs, lhs, size),lxy_free);
	else {
		return std::shared_ptr<unsigned char>(m_lxy_memcmp32_noaligned(rhs, lhs, size), lxy_free);
	}
}

int lxy_internet_checksum(void* source, size_t size,uint16_t *answer)
{
	if (((size_t(source) & 2 )== 1) || ((size &2) == 1)) {
		return -1;
	}//����2�ֽڶ���
	uint16_t u_sum = m_lxy_internet_checksum32(source, size);
	u_sum -= 1;
	u_sum = ~(u_sum);
	
	*answer = u_sum;
	
	return 0;
}
//0 ��ʾУ�鲻�ɹ�
//1 ��ʾУ��ɹ� -1 ��ʾ����ʧ��
int lxy_check_internet_checksum(void* source, size_t size, uint16_t check)
{
	if (((size_t(source) & 2) == 1) || ((size & 2) == 1)) {
		return -1;
	}//����2�ֽڶ���
	uint16_t u_sum = m_lxy_internet_checksum32(source, size);
	u_sum += check;

	return (u_sum==0);
}
#endif
