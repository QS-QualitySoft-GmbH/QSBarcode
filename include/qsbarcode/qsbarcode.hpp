#pragma once

#include "qsbarcode_loader.h"

#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace qsbarcode {

struct bounds {
    int x{};
    int y{};
    int width{};
    int height{};
};

struct barcode_result {
    int format{};
    int symbology_mask{};
    std::string text;
    std::vector<std::uint8_t> raw_text;
    std::uint32_t width{};
    std::uint32_t height{};
    std::uint32_t image_index{};
    int page_index{};
    bool has_bounds{};
    bounds region{};
};

struct rendered_image {
    std::vector<std::uint8_t> pixels;
    std::uint32_t width{};
    std::uint32_t height{};
    int format{};
    int page_index{};
    std::uint32_t stride{};
    bool gray8{};
};

class native_error final : public std::runtime_error {
public:
    explicit native_error(int status)
        : std::runtime_error(qsbc_loader_status_name(status) ? qsbc_loader_status_name(status) : "native error"),
          status_(status) {}

    int status() const noexcept { return status_; }

private:
    int status_;
};

struct reader_options : qsbc_loader_scan_options {
    reader_options() {
        auto status = qsbc_loader_scan_options_init(this);
        if (status < 0) {
            throw native_error(status);
        }
        struct_size = sizeof(qsbc_loader_scan_options);
    }
};

class native_library {
public:
    static std::string abi_version() { return string_or_empty(qsbc_loader_abi_version_string()); }
    static std::string engine_version() { return string_or_empty(qsbc_loader_engine_version_string()); }
    static std::uint32_t capabilities() { return qsbc_loader_capabilities(); }
    static int license_status() { return qsbc_loader_license_status(); }
    static int license_status_file(const std::string& path) { return qsbc_loader_license_status_file(path.c_str()); }
    static std::string format_name(int format) { return string_or_empty(qsbc_loader_format_name(format)); }
    static std::string status_name(int status) { return string_or_empty(qsbc_loader_status_name(status)); }
    static bool is_error(int status) { return qsbc_loader_status_is_error(status) != 0; }
    static bool is_format_supported(int format) { return qsbc_loader_is_format_supported(format) != 0; }

private:
    static std::string string_or_empty(const char* value) { return value == nullptr ? std::string() : std::string(value); }
    static void check(int status) {
        if (status < 0) {
            throw native_error(status);
        }
    }
};

class reader {
public:
    explicit reader(reader_options options = reader_options()) : options_(std::move(options)) {}

    std::vector<barcode_result> read_file(const std::string& path) const {
        std::vector<barcode_result> results;
        auto status = qsbc_loader_scan_file_cb_with_options(path.c_str(), &options_, &reader::collect, &results);
        finish_scan(status);
        return results;
    }

    std::vector<barcode_result> read_bytes(const std::uint8_t* bytes, std::size_t length) const {
        if (bytes == nullptr || length == 0) {
            throw std::invalid_argument("bytes must not be empty");
        }
        std::vector<barcode_result> results;
        auto status = qsbc_loader_scan_image_memory_cb_with_options(bytes, length, &options_, &reader::collect, &results);
        finish_scan(status);
        return results;
    }

    std::vector<barcode_result> read_bytes(const std::vector<std::uint8_t>& bytes) const {
        return read_bytes(bytes.data(), bytes.size());
    }

    std::vector<barcode_result> read_gray8(const std::uint8_t* pixels, std::uint32_t width, std::uint32_t height, std::uint32_t stride) const {
        if (pixels == nullptr || width == 0 || height == 0) {
            throw std::invalid_argument("pixels, width and height are required");
        }
        std::vector<barcode_result> results;
        auto status = qsbc_loader_scan_gray8_cb_with_options(pixels, width, height, stride == 0 ? width : stride, &options_, &reader::collect, &results);
        finish_scan(status);
        return results;
    }

    int page_count_file(const std::string& path) const {
        auto status = qsbc_loader_page_count_file(path.c_str());
        if (status < 0) {
            throw native_error(status);
        }
        return status;
    }

    int page_count_bytes(const std::uint8_t* bytes, std::size_t length) const {
        if (bytes == nullptr || length == 0) {
            throw std::invalid_argument("bytes must not be empty");
        }
        auto status = qsbc_loader_page_count_image_memory(bytes, length);
        if (status < 0) {
            throw native_error(status);
        }
        return status;
    }

    int page_count_bytes(const std::vector<std::uint8_t>& bytes) const {
        return page_count_bytes(bytes.data(), bytes.size());
    }

    int detect_format_file(const std::string& path) const {
        return qsbc_loader_detect_file_format(path.c_str());
    }

    int detect_format_bytes(const std::uint8_t* bytes, std::size_t length) const {
        if (bytes == nullptr || length == 0) {
            throw std::invalid_argument("bytes must not be empty");
        }
        return qsbc_loader_detect_image_format(bytes, length);
    }

    int detect_format_bytes(const std::vector<std::uint8_t>& bytes) const {
        return detect_format_bytes(bytes.data(), bytes.size());
    }

    rendered_image render_file_page_bmp(const std::string& path) const {
        qsbc_loader_image_buffer buffer{};
        auto status = qsbc_loader_render_file_page_bmp_with_options(path.c_str(), &options_, &buffer);
        return finish_render(status, &buffer, false);
    }

    rendered_image render_file_page_gray8(const std::string& path) const {
        qsbc_loader_image_buffer buffer{};
        auto status = qsbc_loader_render_file_page_gray8_with_options(path.c_str(), &options_, &buffer);
        return finish_render(status, &buffer, true);
    }

    rendered_image render_bytes_bmp(const std::uint8_t* bytes, std::size_t length) const {
        if (bytes == nullptr || length == 0) {
            throw std::invalid_argument("bytes must not be empty");
        }
        qsbc_loader_image_buffer buffer{};
        auto status = qsbc_loader_render_image_memory_page_bmp_with_options(bytes, length, &options_, &buffer);
        return finish_render(status, &buffer, false);
    }

    rendered_image render_bytes_bmp(const std::vector<std::uint8_t>& bytes) const {
        return render_bytes_bmp(bytes.data(), bytes.size());
    }

    rendered_image render_bytes_gray8(const std::uint8_t* bytes, std::size_t length) const {
        if (bytes == nullptr || length == 0) {
            throw std::invalid_argument("bytes must not be empty");
        }
        qsbc_loader_image_buffer buffer{};
        auto status = qsbc_loader_render_image_memory_page_gray8_with_options(bytes, length, &options_, &buffer);
        return finish_render(status, &buffer, true);
    }

    rendered_image render_bytes_gray8(const std::vector<std::uint8_t>& bytes) const {
        return render_bytes_gray8(bytes.data(), bytes.size());
    }

private:
    static void finish_scan(int status) {
        if (status < 0 && status != QSBC_ERROR_LICENSE_REQUIRED) {
            throw native_error(status);
        }
    }

    static rendered_image finish_render(int status, qsbc_loader_image_buffer* buffer, bool gray8) {
        if (status < 0) {
            throw native_error(status);
        }
        rendered_image image;
        image.width = buffer->width;
        image.height = buffer->height;
        image.format = buffer->format;
        image.page_index = buffer->page_index;
        image.gray8 = gray8;
        image.stride = gray8 ? buffer->width : 0;
        if (buffer->data != nullptr && buffer->len > 0) {
            image.pixels.assign(buffer->data, buffer->data + buffer->len);
        }
        qsbc_loader_free_image_buffer(buffer);
        return image;
    }

    static int collect(const qsbc_loader_barcode_result* native_result, void* user_data) {
        if (native_result == nullptr || user_data == nullptr) {
            return QSBC_STATUS_MISS;
        }
        auto* results = static_cast<std::vector<barcode_result>*>(user_data);
        barcode_result result;
        result.format = native_result->format;
        result.symbology_mask = native_result->symbology_mask;
        result.width = native_result->width;
        result.height = native_result->height;
        result.image_index = native_result->image_index;
        result.page_index = native_result->page_index;
        if (native_result->text != nullptr && native_result->text_len > 0) {
            result.raw_text.assign(native_result->text, native_result->text + native_result->text_len);
            result.text.assign(reinterpret_cast<const char*>(native_result->text), static_cast<std::size_t>(native_result->text_len));
        }
        result.has_bounds = native_result->has_bounds != 0;
        result.region = { native_result->barcode_x, native_result->barcode_y, native_result->barcode_width, native_result->barcode_height };
        results->push_back(std::move(result));
        return 0;
    }

    reader_options options_;
};

} // namespace qsbarcode
