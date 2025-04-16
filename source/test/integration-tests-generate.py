import os
import subprocess
import argparse

def generate_new_data(directory):
    """Generate new data for tests in the specified directory."""
    if not os.path.isdir(directory):
        print(f"Directory not found: {directory}, skipping.")
        return

    print(f"Generating new data in: {directory}")
    os.chdir(directory)

    # List all files in the directory
    files = os.listdir(directory)

    # Filter .exe files
    exe_files = [f for f in files if f.endswith(".exe") and f.startswith("integration-test-")]

    for exe in exe_files:
        print(f"Generating data using {os.path.join(directory, exe)}")
        command = [os.path.join(directory, exe), "--generate-new-data", "--no_testing"]
        try:
            subprocess.run(command, check=True, text=True, cwd=directory)
            print(f"Data generated successfully for {exe}.")
        except subprocess.CalledProcessError as e:
            print(f"Error while generating data with {exe}: {e}")

def main():
    parser = argparse.ArgumentParser(description="Generate new data for integration tests.")
    parser.add_argument(
        "--mode",
        choices=["Debug", "Release", "both"],
        default="Debug",
        help="Specify which build mode to run data generation (Debug, Release, or both). Default: Debug."
    )
    # Parse known arguments
    args, unknown_args = parser.parse_known_args()

    script_dir = os.path.dirname(os.path.abspath(__file__))
    root_dir = os.path.abspath(os.path.join(script_dir, "../../"))
    build_debug = os.path.join(root_dir, "build", "Debug")
    build_release = os.path.join(root_dir, "build", "Release")

    if args.mode in ["Debug", "both"]:
        generate_new_data(build_debug)
    if args.mode in ["Release", "both"]:
        generate_new_data(build_release)

if __name__ == "__main__":
    main()
