$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $MyInvocation.MyCommand.Path
$build = Join-Path $root 'build'
if (!(Test-Path $build)) { New-Item -Path $build -ItemType Directory | Out-Null }

Write-Host "Compilation du test..."
& g++ -std=c++17 -I"$root/src" -I"$root/src/models" -I"$root/src/services" -I"$root/src/persistence" -I"$root/src/exceptions" `
    "$root/tests/test_main.cpp" `
    "$root/src/models/UndergraduateStudent.cpp" `
    "$root/src/models/GraduateStudent.cpp" `
    "$root/src/models/PhDStudent.cpp" `
    "$root/src/models/Student.cpp" `
    "$root/src/services/StudentManager.cpp" `
    "$root/src/persistence/PersistenceManager.cpp" -o "$build/test_main.exe"

if ($LASTEXITCODE -ne 0) { Write-Error "Compilation a échoué."; exit $LASTEXITCODE }

Write-Host "Execution des tests..."
& "$build/test_main.exe"

# Preserve exit code, prompt user to keep window open until they press Enter, then exit with preserved code
$exitCode = $LASTEXITCODE
Write-Host ""
Write-Host "Execution terminee. Appuyez sur Entrée pour fermer cette fenêtre..."
Read-Host | Out-Null
exit $exitCode