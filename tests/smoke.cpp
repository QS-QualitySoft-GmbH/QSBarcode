#include <qsbarcode/qsbarcode.hpp>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

std::vector<std::uint8_t> read_all_bytes(const char* path) {
    std::ifstream stream(path, std::ios::binary);
    if (!stream) {
        throw std::runtime_error(std::string("failed to open ") + path);
    }

    return std::vector<std::uint8_t>(
        std::istreambuf_iterator<char>(stream),
        std::istreambuf_iterator<char>());
}

void require(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void smoke_scan_file(const char* path) {
    qsbarcode::reader_options options;
    options.mask = QSBC_SCAN_QR;
    options.min_length = 1;

    qsbarcode::reader reader(options);
    const auto results = reader.read_file(path);
    require(!results.empty(), "file scan returned no barcode results");

    const auto bytes = read_all_bytes(path);
    const auto memory_results = reader.read_bytes(bytes);
    require(!memory_results.empty(), "memory scan returned no barcode results");
}

void smoke_pdf_file(const char* path) {
    qsbarcode::reader_options options;
    options.dpi = 150;
    options.page_start = 0;
    options.page_count = 1;

    qsbarcode::reader reader(options);
    const auto page_count = reader.page_count_file(path);
    require(page_count > 0, "PDF page count must be positive");

    const auto rendered = reader.render_file_page_gray8(path);
    require(rendered.gray8, "PDF render did not return Gray8");
    require(rendered.width > 0 && rendered.height > 0, "PDF render returned empty dimensions");
    require(!rendered.pixels.empty(), "PDF render returned empty pixels");
}

} // namespace

int main() {
    try {
        const auto abi = qsbarcode::native_library::abi_version();
        const auto engine = qsbarcode::native_library::engine_version();
        const auto caps = qsbarcode::native_library::capabilities();

        std::cout << "QS Barcode ABI: " << abi << "\n";
        std::cout << "QS Barcode engine: " << engine << "\n";
        std::cout << "Capabilities: 0x" << std::hex << caps << std::dec << "\n";
        std::cout << "License status: 0x" << std::hex << qsbarcode::native_library::license_status() << std::dec << "\n";

        require(qsbc_loader_abi_version_major() == 2, "unexpected native ABI major version");
        require(!abi.empty(), "ABI version string is empty");
        require(!engine.empty(), "engine version string is empty");
        require((caps & QSBC_CAP_MEMORY_SCAN) != 0, "memory scan capability is missing");
        require((caps & QSBC_CAP_FILE_SCAN) != 0, "file scan capability is missing");
        require((caps & QSBC_CAP_CALLBACK_SCAN) != 0, "callback scan capability is missing");
        require((caps & QSBC_CAP_MULTIPAGE_SCAN) != 0, "multipage capability is missing");
        require(qsbarcode::native_library::is_format_supported(QSBC_FORMAT_TIFF), "TIFF support is missing");
        require(qsbarcode::native_library::is_error(QSBC_ERROR_INVALID_ARGUMENT), "error status helper failed");
        require(!qsbarcode::native_library::status_name(QSBC_STATUS_HIT).empty(), "status name helper failed");
        require(!qsbarcode::native_library::format_name(QSBC_FORMAT_TIFF).empty(), "format name helper failed");

        if (const auto* image = std::getenv("QSBC_NATIVE_SMOKE_IMAGE")) {
            smoke_scan_file(image);
            std::cout << "Image smoke fixture passed: " << image << "\n";
        }

        if (const auto* pdf = std::getenv("QSBC_NATIVE_SMOKE_PDF")) {
            smoke_pdf_file(pdf);
            std::cout << "PDF smoke fixture passed: " << pdf << "\n";
        }

        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Native smoke failed: " << ex.what() << "\n";
        return 1;
    }
}
