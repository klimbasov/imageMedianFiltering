#include "Padder.h"

void Padder::addPadding(image& dst, const image& src, size_t padding_size)
{
	dst.magic = src.magic;
	dst.depth = src.depth;
	dst.width = src.width + 2 * padding_size;
	dst.height = src.height + 2 * padding_size;
	dst.data = new char[dst.width * dst.height]{0};
	size_t dst_data_pos = (size_t)dst.data;
	size_t src_data_pos = (size_t)src.data;
	for (int src_row_index = 0; src_row_index < src.height; src_row_index++) {
		void* dst_paste_ptr = (void*)(dst_data_pos + dst.width * (padding_size + src_row_index) + padding_size);
		const void* src_copy_ptr = (const void*)(src_data_pos + src.width * src_row_index);
		memcpy(dst_paste_ptr, src_copy_ptr, src.width);
	}
}