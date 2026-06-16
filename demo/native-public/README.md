# QSBarcode Public C/C++ Demo

This demo verifies the public `QSBarcode` C/C++ SDK distribution as a real
consumer would use it.

At CMake configure time it:

- downloads `qsbarcode-native-6.0.0.zip` from the public GitHub Release
- verifies the release ZIP SHA256
- extracts the matching `native/<rid>/` runtime libraries
- fetches the public `QSBarcode` repository at tag `v6.0.0`
- builds a C demo and a C++17 demo against the public headers and runtime

Configure and build:

```powershell
cmake -S demo/native-public -B artifacts/demo-native-public
cmake --build artifacts/demo-native-public
ctest --test-dir artifacts/demo-native-public --output-on-failure
```

If the local CMake installation cannot validate the corporate or system CA
store while downloading from GitHub, configure with
`-DQSBC_PUBLIC_TLS_VERIFY=OFF`. The ZIP SHA256 is still verified.

Run a scan fixture:

```powershell
$inputPath = (Resolve-Path examples/BarTest.tif).Path
cmake -S demo/native-public -B artifacts/demo-native-public `
  "-DQSBC_DEMO_INPUT=$inputPath"
cmake --build artifacts/demo-native-public
ctest --test-dir artifacts/demo-native-public --output-on-failure
```

Or run the C++ executable directly:

```powershell
artifacts/demo-native-public/Debug/qsbarcode_public_cpp_demo.exe `
  --input examples/BarTest.tif `
  --license demo/dotnet/QualitySoft.Barcode.Demo/qsbc.lic `
  --require-hit
```

For single-config generators such as Ninja, the executable is directly under
`artifacts/demo-native-public/`.

Without a license file the runtime can still run in demo mode, but decoded
values may be deliberately modified.
