$TestTargets = @(
    "check-pcg8_once_insecure",
    "check-pcg8_oneseq_once_insecure",
    "check-pcg16_once_insecure",
    "check-pcg16_oneseq_once_insecure",
    "check-pcg32",
    "check-pcg32_fast",
    "check-pcg32_k1024",
    "check-pcg32_k1024_fast",
    "check-pcg32_k16384",
    "check-pcg32_k16384_fast",
    "check-pcg32_k2",
    "check-pcg32_k2_fast",
    "check-pcg32_k64",
    "check-pcg32_k64_fast",
    "check-pcg32_k64_oneseq",
    "check-pcg32_once_insecure",
    "check-pcg32_oneseq",
    "check-pcg32_oneseq_once_insecure",
    "check-pcg64",
    "check-pcg64_fast",
    "check-pcg64_k1024",
    "check-pcg64_k1024_fast",
    "check-pcg64_k32",
    "check-pcg64_k32_fast",
    "check-pcg64_k32_oneseq",
    "check-pcg64_once_insecure",
    "check-pcg64_oneseq",
    "check-pcg64_oneseq_once_insecure",
    "check-pcg128_once_insecure",
    "check-pcg128_oneseq_once_insecure",
    "check-pcg32_c1024",
    "check-pcg32_c1024_fast",
    "check-pcg32_c64",
    "check-pcg32_c64_fast",
    "check-pcg32_c64_oneseq",
    "check-pcg32_unique",
    "check-pcg64_c1024",
    "check-pcg64_c1024_fast",
    "check-pcg64_c32",
    "check-pcg64_c32_fast",
    "check-pcg64_c32_oneseq",
    "check-pcg64_unique"
)

Write-Output "======================================== running ========================================"


Remove-Item -Path "./actual" -Recurse

foreach ($TestTarget in $TestTargets) {
    cmake --build ../build --target "$TestTarget"
    cmake --build ../build --target "gen-$TestTarget"
}

Write-Output "======================================== verity ========================================"

foreach ($TestTarget in $TestTargets) {
    if ((Test-Path -Path "actual/$TestTarget.out" -PathType Leaf) -and (Test-Path -Path "expected/$TestTarget.out" -PathType Leaf)) {
        $actual = Get-Content -Path "actual/$TestTarget.out" -Raw
        $expected = Get-Content -Path "expected/$TestTarget.out" -Raw
    
        $actual = $actual -replace '\r\n|\r|\n', "`n"
        $expected = $expected -replace '\r\n|\r|\n', "`n"
    
        if ($actual -ne $expected) {
            Write-Output "actual/$TestTarget.out != expected/$TestTarget.out"
            # TODO: fc is Windows only
            fc.exe "$PSScriptRoot\actual\$TestTarget.out" "$PSScriptRoot\expected\$TestTarget.out"
        }
    }
}
