@echo off
REM ============================================================================
REM run_integration_tests.bat
REM Wrapper to run integration-tests-generate.py in a specified directory
REM Usage: run_integration_tests.bat [path_to_directory] [additional args...]
REM ============================================================================

:: Capture the first argument as the target script directory
set SCRIPT_PATH=%1

if "%SCRIPT_PATH%"=="" (
    echo No path provided. Defaulting to the current directory.
    set SCRIPT_PATH=%CD%
)

:: The 'shift' command moves all arguments one position to the left.
:: This discards %1 (used for SCRIPT_PATH) and allows subsequent arguments to be accessed as %1, %2, etc.
shift
set SCRIPT_ARGS=%*

:: Change to the specified script directory (switch drives if needed)
cd /d "%SCRIPT_PATH%"

:: Invoke the Python integration test generator with the remaining arguments
py integration-tests-run.py %SCRIPT_ARGS%

