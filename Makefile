#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project directory, and will be used to build the application.
#
# This file is optional for projects using ESP-IDF v4.0 and later.
#

PROJECT_NAME := valve_controller

include $(IDF_PATH)/make/project.mk

# Custom target to upload web files
upload-web:
	python scripts/upload_web_files.py

.PHONY: upload-web
