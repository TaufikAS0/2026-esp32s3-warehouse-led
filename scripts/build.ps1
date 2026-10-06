param([string]$ArduinoCli = $env:ARDUINO_CLI, [switch]$InstallDependencies)
$ErrorActionPreference = 'Stop'
$taskRoot = Split-Path $PSScriptRoot -Parent
if (!$ArduinoCli) {
    $taskCommand = Get-Command arduino-cli -ErrorAction SilentlyContinue
    if ($taskCommand) { $ArduinoCli = $taskCommand.Source }
    else {
        $taskCandidate = Join-Path $env:LOCALAPPDATA 'Programs\Arduino IDE\resources\app\lib\backend\resources\arduino-cli.exe'
        if (Test-Path -LiteralPath $taskCandidate) { $ArduinoCli = $taskCandidate }
        else { throw 'Set ARDUINO_CLI or pass -ArduinoCli with the installed arduino-cli executable.' }
    }
}
$taskSandbox = Join-Path $taskRoot '_sandbox'
$taskUser = Join-Path $taskSandbox 'sketchbook'
New-Item -ItemType Directory -Force -Path $taskUser | Out-Null
$taskConfig = Join-Path $taskSandbox 'arduino-cli.yaml'
# Only change the sketchbook for this invocation. Reuse the installed core data directory.
$taskYamlUser = $taskUser.Replace('\','/').Replace("'", "''")
Set-Content -LiteralPath $taskConfig -Encoding utf8 -Value "directories:`n  user: '$taskYamlUser'"
if ($InstallDependencies) {
    & $ArduinoCli --config-file $taskConfig lib install 'ArduinoJson@7.4.2' 'Adafruit NeoPixel@1.15.2'
    if ($LASTEXITCODE) { throw 'Library installation failed.' }
}
foreach ($taskDependency in @(@('ArduinoJson','7.4.2'), @('Adafruit_NeoPixel','1.15.2'))) {
    $taskProperties = Join-Path $taskUser ('libraries/' + $taskDependency[0] + '/library.properties')
    if (!(Test-Path -LiteralPath $taskProperties) -or
        !(Get-Content -LiteralPath $taskProperties | Where-Object { $_.Trim() -eq ('version=' + $taskDependency[1]) })) {
        throw ('Pinned library missing/mismatched: ' + $taskDependency[0] + '. Run with -InstallDependencies.')
    }
}
$taskCore = & $ArduinoCli --config-file $taskConfig core list --json | ConvertFrom-Json
if (!($taskCore.platforms | Where-Object { $_.id -eq 'esp32:esp32' -and $_.installed_version -eq '3.3.10' })) {
    throw 'Required core esp32:esp32@3.3.10 missing. Install explicitly with arduino-cli core install esp32:esp32@3.3.10.'
}
& $ArduinoCli --config-file $taskConfig compile --fqbn 'esp32:esp32:esp32s3:FlashSize=4M,PartitionScheme=default,PSRAM=disabled,USBMode=hwcdc,CDCOnBoot=cdc' --warnings all --build-path (Join-Path $taskSandbox 'build') (Join-Path $taskRoot '02_Firmware\WarehouseLed')
if ($LASTEXITCODE) { throw 'ESP32-S3 build failed.' }
