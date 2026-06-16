#include <qsbarcode/qsbarcode_loader.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define set_license_env(value) _putenv_s("QSBC_LICENSE_FILE", value)
#else
#define set_license_env(value) setenv("QSBC_LICENSE_FILE", value, 1)
#endif

typedef struct result_counter {
    int count;
} result_counter;

static int on_result(const qsbc_loader_barcode_result* result, void* user_data) {
    result_counter* counter = (result_counter*)user_data;
    if (result == NULL) {
        return QSBC_STATUS_MISS;
    }

    counter->count++;
    printf("  %s page=%d text=", qsbc_loader_format_name(result->format), result->page_index);
    if (result->text != NULL && result->text_len > 0) {
        fwrite(result->text, 1, (size_t)result->text_len, stdout);
    }
    printf("\n");
    return 0;
}

static int fail(const char* message) {
    fprintf(stderr, "QSBarcode public C demo failed: %s\n", message);
    return 1;
}

static int verify_runtime(const char* license_file) {
    const char* abi = qsbc_loader_abi_version_string();
    const char* engine = qsbc_loader_engine_version_string();
    uint32_t capabilities = qsbc_loader_capabilities();
    int license_status = license_file == NULL ? qsbc_loader_license_status() : qsbc_loader_license_status_file(license_file);

    printf("QS Barcode ABI: %s\n", abi == NULL ? "" : abi);
    printf("QS Barcode engine: %s\n", engine == NULL ? "" : engine);
    printf("Capabilities: 0x%x\n", capabilities);
    printf("License status: 0x%x\n", license_status);

    if (qsbc_loader_abi_version_major() != 2) {
        return fail("unexpected native ABI major version");
    }
    if (abi == NULL || abi[0] == '\0') {
        return fail("ABI version string is empty");
    }
    if (engine == NULL || engine[0] == '\0') {
        return fail("engine version string is empty");
    }
    if ((capabilities & QSBC_CAP_FILE_SCAN) == 0) {
        return fail("file scan capability is missing");
    }
    if ((capabilities & QSBC_CAP_MEMORY_SCAN) == 0) {
        return fail("memory scan capability is missing");
    }
    if (qsbc_loader_is_format_supported(QSBC_FORMAT_TIFF) == 0) {
        return fail("TIFF support is missing");
    }
    if (qsbc_loader_status_is_error(QSBC_ERROR_INVALID_ARGUMENT) == 0) {
        return fail("status helper failed");
    }
    return 0;
}

static int scan_input(const char* input, unsigned int dpi, unsigned int timeout_ms, int require_hit) {
    qsbc_loader_scan_options options;
    result_counter counter = {0};
    int status = qsbc_loader_scan_options_init(&options);
    if (status < 0) {
        fprintf(stderr, "options failed: %s (%d)\n", qsbc_loader_status_name(status), status);
        return 1;
    }

    options.mask = -1;
    options.dpi = dpi;
    options.scan_timeout_ms = timeout_ms;

    printf("Scanning: %s\n", input);
    status = qsbc_loader_scan_file_cb_with_options(input, &options, on_result, &counter);
    if (status < 0 && status != QSBC_ERROR_LICENSE_REQUIRED) {
        fprintf(stderr, "scan failed: %s (%d)\n", qsbc_loader_status_name(status), status);
        return 1;
    }
    if (require_hit && counter.count == 0) {
        return fail("scan returned no barcode results");
    }

    printf("Result count: %d\n", counter.count);
    return 0;
}

static void print_usage(void) {
    fprintf(stderr,
        "Usage: qsbarcode_public_c_demo [--self-test] [--input image-or-pdf] [--license qsbc.lic]\n"
        "                                [--dpi 300] [--timeout-ms 5000] [--require-hit]\n");
}

int main(int argc, char** argv) {
    const char* input = NULL;
    const char* license_file = NULL;
    unsigned int dpi = 300;
    unsigned int timeout_ms = 0;
    int require_hit = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--self-test") == 0) {
            continue;
        } else if (strcmp(argv[i], "--input") == 0 && i + 1 < argc) {
            input = argv[++i];
        } else if (strcmp(argv[i], "--license") == 0 && i + 1 < argc) {
            license_file = argv[++i];
        } else if (strcmp(argv[i], "--dpi") == 0 && i + 1 < argc) {
            dpi = (unsigned int)strtoul(argv[++i], NULL, 10);
        } else if (strcmp(argv[i], "--timeout-ms") == 0 && i + 1 < argc) {
            timeout_ms = (unsigned int)strtoul(argv[++i], NULL, 10);
        } else if (strcmp(argv[i], "--require-hit") == 0) {
            require_hit = 1;
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage();
            return 2;
        } else if (input == NULL) {
            input = argv[i];
        } else {
            print_usage();
            return 2;
        }
    }

    if (license_file != NULL) {
        set_license_env(license_file);
        printf("License file: %s\n", license_file);
    } else {
        printf("No qsbc.lic passed; demo mode may degrade decoded values.\n");
    }

    if (verify_runtime(license_file) != 0) {
        return 1;
    }
    if (input != NULL) {
        return scan_input(input, dpi, timeout_ms, require_hit);
    }

    printf("Public QSBarcode C SDK self-test passed.\n");
    return 0;
}
