set SCRIPT_PATH=%1

if "%SCRIPT_PATH%"=="" (
    echo Please provide the path to the directory where the Python script is located.
    exit /b
)

cd /d "%SCRIPT_PATH%"

py integration-tests-run.py
