import os
import subprocess
import argparse

def main():
    # Parse command-line arguments
    parser = argparse.ArgumentParser(description="Run integration tests for executables.")
    parser.add_argument(
        "--mode",
        choices=["Debug", "Release", "both"],
        default="Debug",
        help="Specify which build mode to run (Debug, Release, or both). Default: both."
    )
    parser.add_argument(
        "--generate-new-data",
        action="store_true",
        help="Pass this flag to generate new data for tests."
    )
    args = parser.parse_args()

    # Determine root and build directories
    script_dir = os.path.dirname(os.path.abspath(__file__))  # Current script directory
    root_dir = os.path.abspath(os.path.join(script_dir, "../../"))  # Navigate two levels up
    build_debug = os.path.join(root_dir, "build", "Debug")
    build_release = os.path.join(root_dir, "build", "Release")

    # Optional arguments to pass to the executables
    optional_arguments = []
    if args.generate_new_data:
        optional_arguments.append("--generate-new-data")

    # Run tests in the specified mode
    if args.mode in ["Debug", "both"]:
        run_tests_in_directory(build_debug, optional_arguments)
    if args.mode in ["Release", "both"]:
        run_tests_in_directory(build_release, optional_arguments)


def run_tests_in_directory(directory, optional_arguments):
    """Run all tests in the specified directory."""
    if not os.path.isdir(directory):
        print(f"Directory not found: {directory}, skipping.")
        return

    print(f"Running tests in: {directory}")
    os.chdir(directory)  # Change to the target directory

    # List all files in the directory
    files = os.listdir(directory)

    # Filter .exe files
    exe_files = [f for f in files if f.endswith(".exe")]

    for exe in exe_files:
        # Create corresponding test PNG filename
        test_png = f"test-{exe.replace('.exe', '')}.png"

        # Check if the test PNG exists
        if test_png in files:
            print(f"Running {os.path.join(directory, exe)} with matching test file {test_png}")

            # Run the executable with optional arguments
            command = [os.path.join(directory, exe), "--color=always"] + optional_arguments
            try:
                # Add environment variables to force color if supported by the executable
                env = {**os.environ, "CLICOLOR": "1", "FORCE_COLOR": "1"}

                # Run the executable, directly passing output to the terminal
                subprocess.run(
                    command,
                    check=True,
                    text=True,       # Ensures text-based output instead of bytes
                    env=env,         # Injects environment variables
                    cwd=directory    # Set the working directory
                )
                print(f"{exe} ran successfully.")
            except subprocess.CalledProcessError as e:
                print(f"Error while running {exe}: {e}")
        else:
            print(f"No matching test file found for {exe}, skipping.")

if __name__ == "__main__":
    main()
