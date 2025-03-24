set SCRIPT_PATH=%1

if "%SCRIPT_PATH%"=="" (
    echo No path provided. Defaulting to the current directory.
    set SCRIPT_PATH=%CD%
)

:: The 'shift' command moves all arguments one position to the left.
:: This discards %1 (used for SCRIPT_PATH) and allows subsequent arguments to be accessed as %1, %2, etc.
shift
set SCRIPT_ARGS=%*

cd /d "%SCRIPT_PATH%"

py unit-tests-run.py %SCRIPT_ARGS%

