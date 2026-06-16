#include <qsbarcode/qsbarcode.hpp>

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

namespace {

void set_license_env(const char* value) {
#ifdef _WIN32
    _putenv_s("QSBC_LICENSE_FILE", value);
#else
    setenv("QSBC_LICENSE_FILE", value, 1);
#endif
}

void require(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void print_usage() {
    std::cerr
        << "Usage: qsbarcode_public_cpp_demo [--self-test] [--input image-or-pdf] [--license qsbc.lic]\n"
        << "                                  [--dpi 300] [--timeout-ms 5000] [--require-hit]\n";
}

void verify_runtime(const std::string& license_file) {
    const auto abi = qsbarcode::native_library::abi_version();
    const auto engine = qsbarcode::native_library::engine_version();
    const auto capabilities = qsbarcode::native_library::capabilities();
    const auto license_status = license_file.empty()
        ? qsbarcode::native_library::license_status()
        : qsbarcode::native_library::license_status_file(license_file);

    std::cout << "QS Barcode ABI: " << abi << "\n";
    std::cout << "QS Barcode engine: " << engine << "\n";
    std::cout << "Capabilities: 0x" << std::hex << capabilities << std::dec << "\n";
    std::cout << "License status: 0x" << std::hex << license_status << std::dec << "\n";

    require(qsbc_loader_abi_version_major() == 2, "unexpected native ABI major version");
    require(!abi.empty(), "ABI version string is empty");
    require(!engine.empty(), "engine version string is empty");
    require((capabilities & QSBC_CAP_FILE_SCAN) != 0, "file scan capability is missing");
    require((capabilities & QSBC_CAP_MEMORY_SCAN) != 0, "memory scan capability is missing");
    require(qsbarcode::native_library::is_format_supported(QSBC_FORMAT_TIFF), "TIFF support is missing");
    require(qsbarcode::native_library::is_error(QSBC_ERROR_INVALID_ARGUMENT), "status helper failed");
}

void scan_input(const std::string& input, unsigned int dpi, unsigned int timeout_ms, bool require_hit) {
    qsbarcode::reader_options options;
    options.mask = -1;
    options.dpi = dpi;
    options.scan_timeout_ms = timeout_ms;

    qsbarcode::reader reader(options);
    std::cout << "Scanning: " << input << "\n";
    const auto results = reader.read_file(input);
    if (require_hit) {
        require(!results.empty(), "scan returned no barcode results");
    }

    for (const auto& result : results) {
        std::cout << "  " << qsbarcode::native_library::format_name(result.format)
                  << " page=" << result.page_index
                  << " text=" << result.text << "\n";
    }
    std::cout << "Result count: " << results.size() << "\n";
}

} // namespace

int main(int argc, char** argv) {
    std::string input;
    std::string license_file;
    unsigned int dpi = 300;
    unsigned int timeout_ms = 0;
    bool require_hit = false;

    for (int index = 1; index < argc; index++) {
        std::string arg = argv[index];
        if (arg == "--self-test") {
            continue;
        } else if (arg == "--input" && index + 1 < argc) {
            input = argv[++index];
        } else if (arg == "--license" && index + 1 < argc) {
            license_file = argv[++index];
        } else if (arg == "--dpi" && index + 1 < argc) {
            dpi = static_cast<unsigned int>(std::stoul(argv[++index]));
        } else if (arg == "--timeout-ms" && index + 1 < argc) {
            timeout_ms = static_cast<unsigned int>(std::stoul(argv[++index]));
        } else if (arg == "--require-hit") {
            require_hit = true;
        } else if (arg == "--help" || arg == "-h") {
            print_usage();
            return 2;
        } else if (input.empty()) {
            input = arg;
        } else {
            print_usage();
            return 2;
        }
    }

    try {
        if (!license_file.empty()) {
            set_license_env(license_file.c_str());
            std::cout << "License file: " << license_file << "\n";
        } else {
            std::cout << "No qsbc.lic passed; demo mode may degrade decoded values.\n";
        }

        verify_runtime(license_file);
        if (!input.empty()) {
            scan_input(input, dpi, timeout_ms, require_hit);
        }

        std::cout << "Public QSBarcode C++ SDK self-test passed.\n";
        return 0;
    } catch (const qsbarcode::native_error& ex) {
        std::cerr << "QSBarcode public C++ demo failed: " << ex.what() << " (" << ex.status() << ")\n";
        return 1;
    } catch (const std::exception& ex) {
        std::cerr << "QSBarcode public C++ demo failed: " << ex.what() << "\n";
        return 1;
    }
}
