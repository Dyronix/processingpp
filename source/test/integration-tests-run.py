import os
import subprocess
import argparse

def run_tests_in_directory(directory):
    """Run all tests in the specified directory."""
    if not os.path.isdir(directory):
        print(f"Directory not found: {directory}, skipping.")
        return

    print(f"Running tests in: {directory}")
    os.chdir(directory)  # Change to the target directory

    # List all files in the directory
    files = os.listdir(directory)

    # Filter .exe files
    exe_files = [f for f in files if f.endswith(".exe") and f.startswith("integration-test-")]

    for exe in exe_files:
        # Create corresponding test PNG filename
        test_png = f"{exe.replace('.exe', '')}.png"

        # Check if the test PNG exists
        if test_png in files:
            print(f"Running {os.path.join(directory, exe)} with matching test file {test_png}")

            # Run the executable with optional arguments
            command = [os.path.join(directory, exe)]
            try:
                # Run the executable, directly passing output to the terminal
                subprocess.run(
                    command,
                    check=True,
                    text=True,       # Ensures text-based output instead of bytes
                    cwd=directory    # Set the working directory
                )
                print(f"{exe} ran successfully.")
            except subprocess.CalledProcessError as e:
                print(f"Error while running {exe}: {e}")
        else:
            print(f"No matching test file found for {exe}, skipping.")

def main():
    # Parse command-line arguments
    parser = argparse.ArgumentParser(description="Run integration tests for executables.")
    parser.add_argument(
        "--mode",
        choices=["Debug", "Release", "both"],
        default="Debug",
        help="Specify which build mode to run (Debug, Release, or both). Default: both."
    )
    # Parse known arguments
    args, unknown_args = parser.parse_known_args()

    # Determine root and build directories
    script_dir = os.path.dirname(os.path.abspath(__file__))  # Current script directory
    root_dir = os.path.abspath(os.path.join(script_dir, "../../"))  # Navigate two levels up
    build_debug = os.path.join(root_dir, "build", "Debug")
    build_release = os.path.join(root_dir, "build", "Release")

    # Run tests in the specified mode
    if args.mode in ["Debug", "both"]:
        run_tests_in_directory(build_debug)
    if args.mode in ["Release", "both"]:
        run_tests_in_directory(build_release)

if __name__ == "__main__":
    main()
