#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zstd.h>

int main() {
    char* my_data = "aaaaaaabbbbbbbbbccccccccddddddddeeeeeeeefffffffffggggggggg";
    size_t size_of_data = strlen(my_data);
    size_t estimated_size_of_compressed_data = ZSTD_compressBound(size_of_data);
    void* compressed_data = malloc(estimated_size_of_compressed_data);
    size_t actual_size_of_compressed_data =
            ZSTD_compress(compressed_data, estimated_size_of_compressed_data,
                          my_data, size_of_data, 19);
    FILE *fp = fopen("mydata.zst", "wb");
    fwrite(compressed_data, actual_size_of_compressed_data, 1, fp);
    fclose(fp);
    void* decompressed_data = malloc(size_of_data);
    ZSTD_decompress(decompressed_data, size_of_data,
                    compressed_data, actual_size_of_compressed_data);
    FILE *fp2 = fopen("decompres1.txt", "wb");
    fwrite(decompressed_data, size_of_data, 1, fp2);
    fclose(fp2);
}