param (
    [switch]$Clean  # Use -CleanBuild flag to trigger the clean process
)

# Function to clean the contents of the build folder but keep the folder
function Clean-BuildFolder {
    $buildFolder = ".\build"

    if (Test-Path $buildFolder) {
        Write-Host "Cleaning contents of '$buildFolder'..."
        Get-ChildItem -Path $buildFolder -Recurse | Remove-Item -Recurse -Force
        Write-Host "Cleaned contents of '$buildFolder'."
    } else {
        Write-Host "Build folder '$buildFolder' does not exist. Creating it..."
        New-Item -ItemType Directory -Path $buildFolder
    }
}

# Clean the build folder if the -Clean flag is provided
if ($Clean) {
    Clean-BuildFolder
}

# Run CMake commands TODO only do this step if specified
Write-Host "Running CMake configuration..."
cmake -S . -B build

if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configuration failed!" -ForegroundColor Red
    exit $LASTEXITCODE
}

Write-Host "Building the project..."
cmake --build .\build\

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    exit $LASTEXITCODE
} else {
    Write-Host "Build succeeded!" -ForegroundColor Green
}
