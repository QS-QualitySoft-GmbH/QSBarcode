# QS Barcode Native SDK

C ABI distribution and C++17 header-only wrapper for the native QS Barcode 6.0
runtime.

Public repository: `https://github.com/QS-QualitySoft-GmbH/QSBarcode`

Release artifact:

```text
qsbarcode-native-6.0.0.zip
```

Product page, pricing and documentation:
`https://qualitysoft.de/products/qs-barcode-sdk/`

## What It Does

The QS Barcode Native SDK provides the C ABI and C++17 wrapper for integrating
the QS Barcode engine directly into native applications and services. It is
intended for document and image based barcode recognition in business
workflows.

Supported feature groups include:

- 1D / linear barcodes such as Code 128, Code 39, EAN/UPC, GS1-128 and related
  industrial formats
- PDF417
- Data Matrix
- QR Code
- Aztec
- multi-page PDF input
- cross-platform native execution on Windows, Linux and macOS
- license-aware feature checks for demo, evaluation and commercial deployments

## Contents

- `include/qsbarcode/qsbarcode_loader.h` - stable C ABI
- `include/qsbarcode/qsbarcode.hpp` - C++17 RAII wrapper
- `cmake/QualitySoftBarcodeConfig.cmake` - CMake package config
- `tests/` - header and smoke tests
- `native/<rid>/` - release-only runtime assets
- `SHA256SUMS.txt` - release manifest

The public source repository keeps `native/` as a placeholder. Native binaries
are distributed through GitHub Releases.

## Installation

Download the release archive and checksum from GitHub:

- `https://github.com/QS-QualitySoft-GmbH/QSBarcode/releases/download/v6.0.0/qsbarcode-native-6.0.0.zip`
- `https://github.com/QS-QualitySoft-GmbH/QSBarcode/releases/download/v6.0.0/qsbarcode-native-6.0.0.zip.sha256`

Verify and extract the archive:

```bash
sha256sum -c qsbarcode-native-6.0.0.zip.sha256
unzip qsbarcode-native-6.0.0.zip
```

## Demo

The repository includes a public C/C++ consumer demo under
`demo/native-public`. It downloads the official release ZIP, verifies its
SHA256, fetches the public SDK tag and builds both a C and a C++17 executable.

```powershell
cmake -S demo/native-public -B artifacts/demo-native-public
cmake --build artifacts/demo-native-public
ctest --test-dir artifacts/demo-native-public --output-on-failure
```

If the local CMake installation cannot validate the system CA store while
downloading from GitHub, configure with `-DQSBC_PUBLIC_TLS_VERIFY=OFF`. The
release ZIP SHA256 is still verified.

## Build From Source

```bash
cmake -S . -B build -DQSBC_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build
```

The header compile test does not require native binaries. The native smoke test
runs only when CMake can find a QS Barcode loader library.

## Use With CMake

After extracting the release ZIP:

```cmake
find_package(QualitySoftBarcode CONFIG REQUIRED)

target_link_libraries(my_app PRIVATE QualitySoftBarcode::headers)
qsbarcode_configure_windows_stack(my_app)
```

If you link directly against the native loader, keep the loader and PDFium
together in the same runtime directory. The native C ABI for this release is
2.0.0.

On Windows, native scan calls need a larger stack than the platform default for
some barcode searches. CMake consumers should call
`qsbarcode_configure_windows_stack(<target>)`, which sets a 16 MB executable
stack. Applications that run scans on their own threads should create those
threads with an equivalent stack size.

The C++17 header exposes RAII-style convenience for:

- native version, engine version and capabilities
- license status and explicit license-file status
- format/status helper names
- file, memory and raw Gray8 scanning
- format detection and page count from file or memory
- BMP and Gray8 page rendering from file or memory
- in-process PDF rendering through the native PDFium runtime on one native
  render thread

PDFium runs inside the native loader. The loader owns one dedicated render
thread and serializes PDF document open, page render and close work through a
bounded queue. The release does not build, package or configure a separate PDF
render executable.

The PDF render worker path and warmup entry points from the previous design are
removed from ABI 2.0.0. Applications should delete those calls when upgrading;
there is no replacement because PDF rendering is now internal to the loader.

For performance-sensitive scans, set an explicit barcode mask instead of
passing the native default mask when the expected formats are known. C/C++ and
wrapper-level benchmarks should use the same bytes or raw pixels and the same
scan options.

## License File

The native runtime searches for `qsbc.lic` by default. For services,
containers and explicit deployments, set `QSBC_LICENSE_FILE` before the first
scan:

```bash
export QSBC_LICENSE_FILE=/etc/qualitysoft/qsbc.lic
```

`*.lic` is ignored by this repository and must not be committed.

Without a valid commercial license the runtime runs in demo/evaluation mode.
Results may still be returned, but decoded 1D and 2D values are deliberately
modified. Production code should check the license status and reject demo mode.

## Supported Platforms

- `win-x86`
- `win-x64`
- `win-arm64`
- `linux-x64`
- `linux-arm64`
- `osx-x64`
- `osx-arm64`

Windows and macOS native assets are platform-signed before release packaging.
Linux assets are covered by SHA256 manifests and signed release artifacts.

## Release Downloads

Download `qsbarcode-native-6.0.0.zip` and
`qsbarcode-native-6.0.0.zip.sha256` from the GitHub Release page for this
repository.

The source repository keeps `native/` as a placeholder. Runtime libraries are
provided by the release ZIP.

## Verify Release Artifact

After downloading a release ZIP:

```bash
sha256sum -c qsbarcode-native-6.0.0.zip.sha256
unzip -l qsbarcode-native-6.0.0.zip
```

The ZIP must contain `native/<rid>/` folders for the supported platforms and
must not contain `qs_barcode_pdf_render_worker` or another PDF worker
executable. PDF rendering is internal to the native loader in ABI 2.0.0.

## Repository Scope

This public repository contains C/C++ headers, CMake metadata, tests,
documentation and native asset placeholders. Runtime libraries are distributed
through GitHub Release assets.

## Pricing And Commercial Use

QS Barcode is proprietary commercial software owned by QS QualitySoft GmbH.
Evaluation and production use require a valid license agreement unless your
agreement explicitly grants demo usage.

Pricing, licensing options and product documentation are maintained on the
official product page:

`https://qualitysoft.de/products/qs-barcode-sdk/`

## Distribution Model

The public GitHub repository is source-only. Runtime binaries are not committed
to Git. Official native releases are delivered through GitHub Release ZIPs,
which contain the matching runtime assets under `native/<rid>/` for Windows,
Linux and macOS.

For local C/C++ development, extract the release ZIP and point CMake or your
application runtime layout at the matching `native/<rid>/` folder. Keep
customer license files local as `qsbc.lic`; they are intentionally ignored by
Git. Without a license file the SDK can run in demo mode and returns degraded
demo results.

## Links

- Product page, pricing and documentation: `https://qualitysoft.de/products/qs-barcode-sdk/`
- Public repository: `https://github.com/QS-QualitySoft-GmbH/QSBarcode`
- Release downloads: `https://github.com/QS-QualitySoft-GmbH/QSBarcode/releases`
