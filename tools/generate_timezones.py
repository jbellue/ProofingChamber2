#!/usr/bin/env python3
"""
Generate Timezones.h from posix_tz_db zones.csv
"""

import csv
import os
from collections import defaultdict

# Path to the zones.csv file
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
CSV_FILE = os.path.join(SCRIPT_DIR, 'posix_tz_db', 'zones.csv')
OUTPUT_FILE = os.path.join(SCRIPT_DIR, '..', 'src', 'Timezones.h')

def parse_zones():
    """Parse zones.csv and organize by continent"""
    continents = defaultdict(list)
    
    with open(CSV_FILE, 'r', encoding='utf-8') as f:
        reader = csv.reader(f)
        for row in reader:
            if len(row) >= 2:
                zone_name = row[0].strip('"')
                posix_string = row[1].strip('"')
                
                # Split zone name into continent and city
                parts = zone_name.split('/')
                if len(parts) >= 2:
                    continent = parts[0]
                    city = '/'.join(parts[1:])  # Handle cities with multiple parts
                    
                    # Clean up city name for display (replace underscores with spaces)
                    city_display = city.replace('_', ' ')
                    
                    continents[continent].append({
                        'city': city_display,
                        'posix': posix_string
                    })
    
    return continents

def escape_string(s):
    """Escape special characters in C strings"""
    return s.replace('\\', '\\\\').replace('"', '\\"')

def generate_header(continents):
    """Generate the Timezones.h header file"""
    
    # Sort continents for consistent output
    sorted_continents = sorted(continents.keys())
    
    header = []
    header.append('#pragma once')
    header.append('')
    header.append('// AUTO-GENERATED FILE - DO NOT EDIT')
    header.append('// Generated from posix_tz_db/zones.csv')
    header.append('')
    header.append('namespace timezones {')
    header.append('    struct Timezone {')
    header.append('        const char* name;')
    header.append('        const char* posixString;')
    header.append('    };')
    header.append('')
    header.append('    struct Continent {')
    header.append('        const char* name;')
    header.append('        const Timezone* timezones;')
    header.append('        int count;')
    header.append('    };')
    header.append('')
    
    # Generate timezone arrays for each continent
    for continent in sorted_continents:
        zones = continents[continent]
        const_name = continent.upper().replace(' ', '_')
        
        header.append(f'    // {continent} timezones')
        header.append(f'    static const Timezone {const_name}[] = {{')
        
        for zone in zones:
            city = escape_string(zone['city'])
            posix = escape_string(zone['posix'])
            header.append(f'        {{"{city}", "{posix}"}},')
        
        header.append('    };')
        header.append('')
    
    # Generate continents array
    header.append('    // All continents')
    header.append('    static const Continent CONTINENTS[] = {')
    
    for continent in sorted_continents:
        const_name = continent.upper().replace(' ', '_')
        header.append(f'        {{"{continent}", {const_name}, sizeof({const_name}) / sizeof(Timezone)}},')
    
    header.append('    };')
    header.append('')
    header.append('    static const int CONTINENT_COUNT = sizeof(CONTINENTS) / sizeof(Continent);')
    
    # Try to find Europe and Paris for defaults
    default_continent_idx = 0
    default_timezone_idx = 0
    
    if 'Europe' in continents:
        default_continent_idx = sorted_continents.index('Europe')
        europe_zones = continents['Europe']
        # Look for Paris
        for idx, zone in enumerate(europe_zones):
            if zone['city'] == 'Paris':
                default_timezone_idx = idx
                break
    
    header.append(f'    static const int DEFAULT_CONTINENT_INDEX = {default_continent_idx};  // {sorted_continents[default_continent_idx]}')
    header.append(f'    static const int DEFAULT_TIMEZONE_INDEX = {default_timezone_idx};   // Default city')
    
    header.append('}')
    header.append('')
    header.append('')
    
    return '\n'.join(header)

def main():
    print(f'Reading timezone data from: {CSV_FILE}')
    continents = parse_zones()
    
    print(f'Found {len(continents)} continents:')
    for continent in sorted(continents.keys()):
        print(f'  {continent}: {len(continents[continent])} timezones')
    
    header_content = generate_header(continents)
    
    print(f'Writing header file to: {OUTPUT_FILE}')
    with open(OUTPUT_FILE, 'w', encoding='utf-8') as f:
        f.write(header_content)
    
    print('Done!')

if __name__ == '__main__':
    main()
