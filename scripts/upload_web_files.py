#!/usr/bin/env python3
"""
Script to upload web files to ESP32 SPIFFS partition
Usage: python upload_web_files.py
"""

import os
import subprocess
import sys

def upload_to_spiffs():
    """Upload web files to SPIFFS partition"""
    
    # Check if ESP-IDF is available
    try:
        idf_path = os.environ['IDF_PATH']
        print(f"Using ESP-IDF from: {idf_path}")
    except KeyError:
        print("ERROR: ESP-IDF environment not found. Please run 'get_idf' first.")
        return False
    
    # Create data directory if it doesn't exist
    data_dir = "data"
    if not os.path.exists(data_dir):
        os.makedirs(data_dir)
        print(f"Created {data_dir} directory")
    
    # Copy web files to data directory
    web_files = ["web/index.html"]
    
    for file_path in web_files:
        if os.path.exists(file_path):
            dest_path = os.path.join(data_dir, os.path.basename(file_path))
            with open(file_path, 'r') as src, open(dest_path, 'w') as dst:
                dst.write(src.read())
            print(f"Copied {file_path} to {dest_path}")
        else:
            print(f"WARNING: {file_path} not found")
    
    # Upload to SPIFFS
    try:
        print("Uploading files to SPIFFS...")
        result = subprocess.run([
            "python", f"{idf_path}/components/spiffs/spiffsgen.py",
            "0x100000",  # SPIFFS size (1MB)
            data_dir,
            "spiffs.bin"
        ], check=True, capture_output=True, text=True)
        
        print("SPIFFS image created successfully")
        
        # Flash the SPIFFS partition
        print("Flashing SPIFFS partition...")
        flash_result = subprocess.run([
            "python", f"{idf_path}/components/esptool_py/esptool/esptool.py",
            "--chip", "esp32",
            "--port", "/dev/ttyUSB0",  # Adjust port as needed
            "--baud", "460800",
            "write_flash", "-z",
            "0x110000", "spiffs.bin"  # SPIFFS partition offset
        ], check=True, capture_output=True, text=True)
        
        print("SPIFFS partition flashed successfully!")
        return True
        
    except subprocess.CalledProcessError as e:
        print(f"ERROR: {e}")
        print(f"stdout: {e.stdout}")
        print(f"stderr: {e.stderr}")
        return False
    
    except Exception as e:
        print(f"ERROR: {e}")
        return False

if __name__ == "__main__":
    if upload_to_spiffs():
        print("Web files uploaded successfully!")
    else:
        print("Failed to upload web files")
        sys.exit(1)
