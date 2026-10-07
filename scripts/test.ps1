param([string]$Cxx = $env:WAREHOUSE_CXX)
$ErrorActionPreference = 'Stop'
$taskRoot = Split-Path $PSScriptRoot -Parent
Push-Location $taskRoot
try {
    python -m unittest discover -s tests -p 'test_*.py' -v
    if ($LASTEXITCODE) { throw 'Python tests failed.' }
    if (!$Cxx) {
        $taskZig = Join-Path $taskRoot '_sandbox/python-tools/ziglang/zig.exe'
        if (Test-Path -LiteralPath $taskZig) { $Cxx = $taskZig }
        else { $Cxx = 'g++' }
    }
    $taskPrefix = @()
    if ((Split-Path $Cxx -Leaf) -match '^zig(\.exe)?$') { $taskPrefix = @('c++') }
    New-Item -ItemType Directory -Force -Path _sandbox | Out-Null
    & $Cxx @taskPrefix -std=c++17 -Wall -Wextra -Werror -I 02_Firmware/WarehouseLed tests/domain_test.cpp -o _sandbox/domain_test.exe
    if ($LASTEXITCODE) { throw 'Domain test build failed.' }
    & './_sandbox/domain_test.exe'
    if ($LASTEXITCODE) { throw 'Domain tests failed.' }
    & $Cxx @taskPrefix -std=c++17 -Wall -Wextra -Werror -I 02_Firmware/WarehouseLed tests/animation_test.cpp -o _sandbox/animation_test.exe
    if ($LASTEXITCODE) { throw 'Animation test build failed.' }
    & './_sandbox/animation_test.exe'
    if ($LASTEXITCODE) { throw 'Animation tests failed.' }
    & $Cxx @taskPrefix -std=c++17 -Wall -Wextra -Werror -DARDUINO=10819 -DESP32 -I tests/neopixel_host -I 02_Firmware/WarehouseLed -I _sandbox/sketchbook/libraries/Adafruit_NeoPixel tests/led_driver_test.cpp 02_Firmware/WarehouseLed/led_driver.cpp _sandbox/sketchbook/libraries/Adafruit_NeoPixel/Adafruit_NeoPixel.cpp -o _sandbox/led_driver_test.exe
    if ($LASTEXITCODE) { throw 'NeoPixel driver test build failed.' }
    & './_sandbox/led_driver_test.exe'
    if ($LASTEXITCODE) { throw 'NeoPixel driver tests failed.' }
    & $Cxx @taskPrefix -std=c++17 -Wall -Wextra -Werror -include tests/host_string.h -I 02_Firmware/WarehouseLed -I _sandbox/sketchbook/libraries/ArduinoJson/src tests/codec_test.cpp 02_Firmware/WarehouseLed/codec.cpp -o _sandbox/codec_test.exe
    if ($LASTEXITCODE) { throw 'Codec test build failed.' }
    & './_sandbox/codec_test.exe'
    if ($LASTEXITCODE) { throw 'Codec tests failed.' }
    node tests/ui_test.cjs
    if ($LASTEXITCODE) { throw 'UI DOM tests failed. Install jsdom@26.1.0 into _sandbox/ui-tests.' }
} finally { Pop-Location }
