"""
PlatformIO pre-build script to generate Timezones.h
"""

Import("env")
import subprocess
import sys
import os

# Get the project directory
project_dir = env['PROJECT_DIR']
script_path = os.path.join(project_dir, 'tools', 'generate_timezones.py')

print("=" * 60)
print("Running timezone generator...")
print("=" * 60)

try:
    result = subprocess.run(
        [sys.executable, script_path],
        cwd=project_dir,
        capture_output=True,
        text=True,
        check=True
    )
    print(result.stdout)
    if result.stderr:
        print(result.stderr, file=sys.stderr)
    print("Timezone generation completed successfully")
except subprocess.CalledProcessError as e:
    print(f"ERROR: Failed to generate timezones: {e}", file=sys.stderr)
    print(f"stdout: {e.stdout}", file=sys.stderr)
    print(f"stderr: {e.stderr}", file=sys.stderr)
    sys.exit(1)
except Exception as e:
    print(f"ERROR: Unexpected error: {e}", file=sys.stderr)
    sys.exit(1)

print("=" * 60)
