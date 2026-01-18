#!/usr/bin/env python3
"""
Generate Timezones.h from posix_tz_db zones.json
Creates a simple flat array of timezones with continent information
"""

import json
import os
import subprocess
import hashlib
from datetime import datetime

# Path to the zones.json file
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
SUBMODULE_DIR = os.path.join(SCRIPT_DIR, 'posix_tz_db')
JSON_FILE = os.path.join(SUBMODULE_DIR, 'zones.json')
OUTPUT_FILE = os.path.join(SCRIPT_DIR, '..', 'src', 'Timezones.h')

def get_submodule_commit():
    """Get the current commit hash of the posix_tz_db submodule"""
    try:
        result = subprocess.run(
            ['git', 'rev-parse', 'HEAD'],
            cwd=SUBMODULE_DIR,
            capture_output=True,
            text=True,
            check=True
        )
        return result.stdout.strip()
    except (subprocess.CalledProcessError, FileNotFoundError):
        return "unknown"

def get_json_hash():
    """Get SHA256 hash of the zones.json file"""
    sha256_hash = hashlib.sha256()
    with open(JSON_FILE, 'rb') as f:
        for byte_block in iter(lambda: f.read(4096), b""):
            sha256_hash.update(byte_block)
    return sha256_hash.hexdigest()

def parse_metadata_from_existing(file_path):
    """Parse metadata from existing Timezones.h if it exists"""
    if not os.path.exists(file_path):
        return None
    
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            lines = f.readlines()
            # Look for metadata comments
            metadata = {}
            for line in lines[:20]:  # Check first 20 lines
                if '// Submodule commit:' in line:
                    metadata['commit'] = line.split(':', 1)[1].strip()
                elif '// JSON hash:' in line:
                    metadata['json_hash'] = line.split(':', 1)[1].strip()
            return metadata if metadata else None
    except Exception:
        return None

def needs_regeneration():
    """Check if regeneration is needed based on submodule version"""
    current_commit = get_submodule_commit()
    current_hash = get_json_hash()
    
    existing_metadata = parse_metadata_from_existing(OUTPUT_FILE)
    
    if existing_metadata is None:
        print("No existing file or metadata found, will generate")
        return True
    
    if existing_metadata.get('commit') != current_commit:
        print(f"Submodule commit changed: {existing_metadata.get('commit', 'unknown')} -> {current_commit}")
        return True
    
    if existing_metadata.get('json_hash') != current_hash:
        print(f"JSON hash changed")
        return True
    
    print(f"Timezones.h is up to date (commit: {current_commit[:8]})")
    return False

def parse_zones():
    """Parse zones.json and create a flat list with continent information"""
    timezones = []
    
    with open(JSON_FILE, 'r', encoding='utf-8') as f:
        zones_data = json.load(f)
    
    for zone_name, posix_string in sorted(zones_data.items()):
        # Split zone name into continent and city
        parts = zone_name.split('/')
        if len(parts) >= 2:
            continent = parts[0]
            city = '/'.join(parts[1:])  # Handle cities with multiple parts
            
            # Clean up city name for display (replace underscores with spaces)
            city_display = city.replace('_', ' ')
            
            timezones.append({
                'continent': continent,
                'city': city_display,
                'posix': posix_string
            })
    
    return timezones

def escape_string(s):
    """Escape special characters in C strings"""
    return s.replace('\\', '\\\\').replace('"', '\\"')

def generate_header(timezones, commit_hash, json_hash):
    """Generate the Timezones.h header file with only static data"""
    
    # Count total timezones and continents
    total_timezones = len(timezones)
    continents = sorted(set(tz['continent'] for tz in timezones))
    
    # Get generation timestamp
    timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S UTC')
    
    # Calculate max lengths for alignment
    max_continent_len = max(len(tz['continent']) for tz in timezones)
    max_city_len = max(len(escape_string(tz['city'])) for tz in timezones)
    
    header = []
    header.append('#pragma once')
    header.append('')
    header.append('// ============================================================================')
    header.append('// AUTO-GENERATED FILE - DO NOT EDIT')
    header.append('// ============================================================================')
    header.append('//')
    header.append('// Generated from: posix_tz_db/zones.json')
    header.append(f'// Source: https://github.com/nayarsystems/posix_tz_db')
    header.append(f'// Submodule commit: {commit_hash}')
    header.append(f'// JSON hash: {json_hash}')
    header.append(f'// Generated at: {timestamp}')
    header.append(f'// Total timezones: {total_timezones}')
    header.append(f'// Continents: {len(continents)}')
    header.append('//')
    header.append('// To regenerate: python3 tools/generate_timezones.py')
    header.append('//')
    header.append('// Note: Helper functions are in TimezoneHelpers.h')
    header.append('// ============================================================================')
    header.append('')
    header.append('namespace timezones {')
    header.append('    struct Timezone {')
    header.append('        const char* continent;')
    header.append('        const char* name;')
    header.append('        const char* posixString;')
    header.append('    };')
    header.append('')
    header.append('    // ========================================================================')
    header.append('    // All Timezones (sorted alphabetically)')
    header.append('    // ========================================================================')
    header.append('')
    header.append('    static const Timezone TIMEZONES[] = {')
    
    # Generate timezone entries
    for tz in timezones:
        continent = tz['continent']
        city = escape_string(tz['city'])
        posix = escape_string(tz['posix'])
        
        # Align the entries for better readability
        continent_padding = ' ' * (max_continent_len - len(continent))
        city_padding = ' ' * (max_city_len - len(city))
        
        header.append(f'        {{"{continent}",{continent_padding} "{city}",{city_padding} "{posix}"}},')
    
    header.append('    };')
    header.append('')
    header.append('    static const int TIMEZONE_COUNT = sizeof(TIMEZONES) / sizeof(Timezone);')
    header.append('')
    header.append('    // ========================================================================')
    header.append('    // Default')
    header.append('    // ========================================================================')
    header.append('')
    
    # Find default timezone (Europe/Paris)
    default_index = 0
    for idx, tz in enumerate(timezones):
        if tz['continent'] == 'Europe' and tz['city'] == 'Paris':
            default_index = idx
            break
    
    header.append(f'    // Default: Europe/Paris')
    header.append(f'    static const int DEFAULT_TIMEZONE_INDEX = {default_index};')
    header.append('')
    header.append('}  // namespace timezones')
    header.append('')
    
    return '\n'.join(header)

def main():
    print(f'Reading timezone data from: {JSON_FILE}')
    
    # Check if regeneration is needed
    if not needs_regeneration():
        print("Skipping generation - file is up to date")
        return
    
    timezones = parse_zones()
    
    print(f'Found {len(timezones)} timezones')
    continents = sorted(set(tz['continent'] for tz in timezones))
    print(f'Across {len(continents)} continents: {", ".join(continents)}')
    
    commit_hash = get_submodule_commit()
    json_hash = get_json_hash()
    
    header_content = generate_header(timezones, commit_hash, json_hash)
    
    print(f'Writing header file to: {OUTPUT_FILE}')
    with open(OUTPUT_FILE, 'w', encoding='utf-8') as f:
        f.write(header_content)
    
    print('Done!')

if __name__ == '__main__':
    main()
