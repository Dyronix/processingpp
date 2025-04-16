import os
import subprocess
import argparse

def run_unit_tests_in_directory(directory):
    """Run all unit tests in the specified directory."""
    if not os.path.isdir(directory):
        print(f"Directory not found: {directory}, skipping.")
        return

    print(f"Running unit tests in: {directory}")
    os.chdir(directory)

    files = os.listdir(directory)
    exe_files = [f for f in files if f.endswith(".exe") and f.startswith("unit-tests-")]

    for exe in exe_files:
        print(f"Running {exe}...")
        command = [os.path.join(directory, exe)]
        try:
            subprocess.run(
                command,
                check=True,
                text=True,
                cwd=directory
            )
            print(f"{exe} ran successfully.")
        except subprocess.CalledProcessError as e:
            print(f"Error while running {exe}: {e}")

def main():
    parser = argparse.ArgumentParser(description="Run unit tests for executables.")
    parser.add_argument(
        "--mode",
        choices=["Debug", "Release", "both"],
        default="both",
        help="Specify which build mode to run (Debug, Release, or both). Default: both."
    )
    args, unknown_args = parser.parse_known_args()

    script_dir = os.path.dirname(os.path.abspath(__file__))
    root_dir = os.path.abspath(os.path.join(script_dir, "../../"))
    build_debug = os.path.join(root_dir, "build", "Debug")
    build_release = os.path.join(root_dir, "build", "Release")

    if args.mode in ["Debug", "both"]:
        run_unit_tests_in_directory(build_debug)
    if args.mode in ["Release", "both"]:
        run_unit_tests_in_directory(build_release)

if __name__ == "__main__":
    main()
