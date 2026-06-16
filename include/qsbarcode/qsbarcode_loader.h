#ifndef QSBC_BARCODE_LOADER_H
#define QSBC_BARCODE_LOADER_H

#include <stddef.h>
#include <stdint.h>

#ifdef _WIN32
#define QSBC_LOADER_API __declspec(dllimport)
#else
#define QSBC_LOADER_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum {
    QSBC_FORMAT_UNKNOWN = 0,
    QSBC_FORMAT_GIF = 1,
    QSBC_FORMAT_PNG = 2,
    QSBC_FORMAT_BMP = 3,
    QSBC_FORMAT_JPEG = 4,
    QSBC_FORMAT_PDF = 5,
    QSBC_FORMAT_TIFF = 6
};

enum {
    QSBC_CAP_MEMORY_SCAN = 1u << 0,
    QSBC_CAP_FILE_SCAN = 1u << 1,
    QSBC_CAP_CALLBACK_SCAN = 1u << 2,
    QSBC_CAP_MULTIPAGE_SCAN = 1u << 3,
    QSBC_CAP_FORMAT_GIF = 1u << 8,
    QSBC_CAP_FORMAT_PNG = 1u << 9,
    QSBC_CAP_FORMAT_BMP = 1u << 10,
    QSBC_CAP_FORMAT_JPEG = 1u << 11,
    QSBC_CAP_FORMAT_PDF = 1u << 12,
    QSBC_CAP_FORMAT_TIFF = 1u << 13
};

enum {
    QSBC_ERROR_INVALID_ARGUMENT = -1,
    QSBC_ERROR_INVALID_BUFFER = -2,
    QSBC_ERROR_LOAD_FAILED = -3,
    QSBC_ERROR_OUT_OF_MEMORY = -4,
    QSBC_ERROR_CALLBACK_FAILED = -5,
    QSBC_ERROR_LICENSE_REQUIRED = -6,
    QSBC_ERROR_TIMEOUT = -7,
    QSBC_STATUS_MISS = 0,
    QSBC_STATUS_HIT = 1
};

enum {
    QSBC_SCAN_CODE128 = 0x00000001,
    QSBC_SCAN_EAN128 = 0x00000002,
    QSBC_SCAN_CODE39 = 0x00000004,
    QSBC_SCAN_CODE39_EXT = 0x00000008,
    QSBC_SCAN_CODE32 = 0x00000010,
    QSBC_SCAN_CODE11 = 0x00000020,
    QSBC_SCAN_I25 = 0x00000040,
    QSBC_SCAN_INDUSTRIAL25 = 0x00000080,
    QSBC_SCAN_IATA25 = 0x00000100,
    QSBC_SCAN_INVERTED25 = 0x00000200,
    QSBC_SCAN_MATRIX25 = 0x00000400,
    QSBC_SCAN_DATALOGIC25 = 0x00000800,
    QSBC_SCAN_BCD_MATRIX25 = 0x00001000,
    QSBC_SCAN_CODABAR = 0x00002000,
    QSBC_SCAN_CODE93 = 0x00004000,
    QSBC_SCAN_CODE93_EXT = 0x00008000,
    QSBC_SCAN_EAN8 = 0x00010000,
    QSBC_SCAN_EAN13 = 0x00020000,
    QSBC_SCAN_UPCA = 0x00040000,
    QSBC_SCAN_UPCE = 0x00080000,
    QSBC_SCAN_CODABLOCK = 0x00100000,
    QSBC_SCAN_DATABAR = 0x00200000,
    QSBC_SCAN_PHARMA = 0x00400000,
    QSBC_SCAN_PATCH = 0x00800000,
    QSBC_SCAN_DATABAR_OMNI = 0x01000000,
    QSBC_SCAN_DATABAR_EXPANDED = 0x02000000,
    QSBC_SCAN_DATABAR_LIMITED = 0x04000000,
    QSBC_SCAN_DATAMATRIX = 0x08000000,
    QSBC_SCAN_QR = 0x10000000,
    QSBC_SCAN_AZTEC = 0x20000000,
    QSBC_SCAN_PDF417 = 0x40000000,
    QSBC_SCAN_POSTAL = (int32_t)0x80000000u
};

enum {
    QSBC_LICENSE_DEMO = 0x00000001,
    QSBC_LICENSE_LINEAR = 0x00000002,
    QSBC_LICENSE_PDF417 = 0x00000004,
    QSBC_LICENSE_DATAMATRIX = 0x00000008,
    QSBC_LICENSE_QR = 0x00000010,
    QSBC_LICENSE_AZTEC = 0x00000020
};

enum {
    QSBC_SCAN_FLAG_DM_REPORT_SYMBOL_IDENTIFIER = 1u << 0,
    QSBC_SCAN_FLAG_DM_SUPPRESS_ECI = 1u << 1,
    QSBC_SCAN_FLAG_DM_INTENSIVE_SEARCH = 1u << 2,
    QSBC_SCAN_FLAG_DM_SEARCH_ON_DOUBLED_REGION = 1u << 3,
    QSBC_SCAN_FLAG_DM_ZEBRA_DOUBLING = 1u << 4,
    QSBC_SCAN_FLAG_DM_TRY_ERODED_IMAGE = 1u << 5,
    QSBC_SCAN_FLAG_QR_ECI = 1u << 6,
    QSBC_SCAN_FLAG_QR_DOUBLE_IMAGE = 1u << 7
};

typedef struct qsbc_loader_scan_result {
    int32_t status;
    int32_t format;
    int32_t decoded_len;
    uint32_t width;
    uint32_t height;
    uint32_t reserved0;
    uint32_t reserved1;
    uint32_t reserved2;
    uint32_t reserved3;
} qsbc_loader_scan_result;

typedef struct qsbc_loader_barcode_result {
    int32_t format;
    int32_t symbology_mask;
    const uint8_t *text;
    int32_t text_len;
    uint32_t width;
    uint32_t height;
    uint32_t image_index;
    int32_t page_index;
    int32_t barcode_x;
    int32_t barcode_y;
    int32_t barcode_width;
    int32_t barcode_height;
    uint32_t has_bounds;
    uint32_t reserved0;
    uint32_t reserved1;
    uint32_t reserved2;
} qsbc_loader_barcode_result;

typedef struct qsbc_loader_scan_options {
    uint32_t struct_size;
    int32_t mask;
    uint32_t min_length;
    uint32_t reserved_result_limit;
    uint32_t flags;
    int32_t page_start;
    int32_t page_count;
    uint32_t dpi;
    uint32_t reserved0;
    uint32_t reserved1;
    uint32_t reserved2;
    uint32_t reserved3;
    uint32_t threshold;
    uint32_t orientation;
    uint32_t max_skew_degrees;
    uint32_t light_margin;
    uint32_t scan_distance_barcode;
    uint32_t tolerance;
    uint32_t min_height;
    uint32_t percent;
    uint32_t scan_distance;
    uint32_t max_gap;
    uint32_t max_height;
    uint32_t checksum_flags;
    uint32_t scan_timeout_ms;
} qsbc_loader_scan_options;

typedef struct qsbc_loader_image_buffer {
    uint8_t *data;
    size_t len;
    uint32_t width;
    uint32_t height;
    int32_t format;
    int32_t page_index;
    uint32_t reserved0;
    uint32_t reserved1;
} qsbc_loader_image_buffer;

typedef int32_t (*qsbc_loader_result_callback)(const qsbc_loader_barcode_result *result, void *user_data);

QSBC_LOADER_API uint32_t qsbc_loader_abi_version_major(void);
QSBC_LOADER_API uint32_t qsbc_loader_abi_version_minor(void);
QSBC_LOADER_API uint32_t qsbc_loader_abi_version_patch(void);
QSBC_LOADER_API const char *qsbc_loader_abi_version_string(void);
QSBC_LOADER_API const char *qsbc_loader_engine_version_string(void);
QSBC_LOADER_API uint32_t qsbc_loader_capabilities(void);
QSBC_LOADER_API int32_t qsbc_loader_is_format_supported(int32_t format);
QSBC_LOADER_API const char *qsbc_loader_format_name(int32_t format);
QSBC_LOADER_API const char *qsbc_loader_status_name(int32_t status);
QSBC_LOADER_API int32_t qsbc_loader_status_is_error(int32_t status);
QSBC_LOADER_API int32_t qsbc_loader_license_status(void);
QSBC_LOADER_API int32_t qsbc_loader_license_status_file(const char *license_file);
QSBC_LOADER_API int32_t qsbc_loader_scan_options_init(qsbc_loader_scan_options *options);
QSBC_LOADER_API int32_t qsbc_loader_detect_file_format(const char *path);
QSBC_LOADER_API int32_t qsbc_loader_detect_image_format(const uint8_t *bytes, size_t byte_len);
QSBC_LOADER_API int32_t qsbc_loader_page_count_file(const char *path);
QSBC_LOADER_API int32_t qsbc_loader_page_count_image_memory(const uint8_t *bytes, size_t byte_len);
QSBC_LOADER_API int32_t qsbc_loader_scan_file_with_options(const char *path, uint8_t *out, int32_t out_len, const qsbc_loader_scan_options *options, qsbc_loader_scan_result *result);
QSBC_LOADER_API int32_t qsbc_loader_scan_file_cb_with_options(const char *path, const qsbc_loader_scan_options *options, qsbc_loader_result_callback callback, void *user_data);
QSBC_LOADER_API int32_t qsbc_loader_scan_image_memory_with_options(const uint8_t *bytes, size_t byte_len, uint8_t *out, int32_t out_len, const qsbc_loader_scan_options *options, qsbc_loader_scan_result *result);
QSBC_LOADER_API int32_t qsbc_loader_scan_image_memory_cb_with_options(const uint8_t *bytes, size_t byte_len, const qsbc_loader_scan_options *options, qsbc_loader_result_callback callback, void *user_data);
QSBC_LOADER_API int32_t qsbc_loader_scan_gray8_cb_with_options(const uint8_t *pixels, uint32_t width, uint32_t height, uint32_t stride, const qsbc_loader_scan_options *options, qsbc_loader_result_callback callback, void *user_data);
QSBC_LOADER_API int32_t qsbc_loader_render_file_page_bmp_with_options(const char *path, const qsbc_loader_scan_options *options, qsbc_loader_image_buffer *out);
QSBC_LOADER_API int32_t qsbc_loader_render_image_memory_page_bmp_with_options(const uint8_t *bytes, size_t byte_len, const qsbc_loader_scan_options *options, qsbc_loader_image_buffer *out);
QSBC_LOADER_API int32_t qsbc_loader_render_file_page_gray8_with_options(const char *path, const qsbc_loader_scan_options *options, qsbc_loader_image_buffer *out);
QSBC_LOADER_API int32_t qsbc_loader_render_image_memory_page_gray8_with_options(const uint8_t *bytes, size_t byte_len, const qsbc_loader_scan_options *options, qsbc_loader_image_buffer *out);
QSBC_LOADER_API void qsbc_loader_free_image_buffer(qsbc_loader_image_buffer *buffer);

#ifdef __cplusplus
}
#endif

#endif
