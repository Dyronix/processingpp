@echo off
REM ============================================================================
REM generate_docs.bat
REM Wrapper to invoke doxygen_to_markdown.py with source and docs directories
REM Usage: generate_docs.bat <src_dir> <docs_dir>
REM ============================================================================

:: Capture first argument as source directory
set "SRC_DIR=%~1"
if "%SRC_DIR%"=="" (
    echo [Error] No source directory provided.
    echo Usage: %~nx0 src_dir docs_dir
    exit /b 1
)

:: Capture second argument as docs directory
set "DOCS_DIR=%~2"
if "%DOCS_DIR%"=="" (
    echo [Error] No docs directory provided.
    echo Usage: %~nx0 src_dir docs_dir
    exit /b 1
)

:: Change to script's own directory
cd /d "%~dp0"

:: Invoke the Python script
py generate_documentation.py "%SRC_DIR%" "%DOCS_DIR%"
