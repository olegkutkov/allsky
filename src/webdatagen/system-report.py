#!/usr/bin/python

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches

import subprocess
import sys

import config

def execute_system_cmd(cmd):
	return subprocess.check_output(cmd).decode('utf-8')

def os_report():
	board_name = ''
	os_release = ''
	kernel_version = ''
	system_uptime = ''

	with open(config.BOARD_MODEL_FILE, 'r') as model_file:
		board_name = model_file.read()

	with open(config.OS_RELEASE_FILE, 'r') as os_release_file:
		os_release = os_release_file.readline().split('=')[1]
		os_release = os_release[1:-2]

	kernel_version = execute_system_cmd(['uname', '-r']).strip()

	system_uptime = execute_system_cmd(['uptime', '-p']).strip()

	return board_name, os_release, kernel_version, system_uptime

def clock_and_voltages_report():
	cpu_freq = 'N/A'
	sys_mem = 'N/A'
	core_volt = 'N/A'
	sdram_volt = 'N/A'

	for line in execute_system_cmd(['vcgencmd', 'get_config', 'int']).strip().split('\n'):
		if 'arm_freq' in line:
			cpu_freq = line.split('=')[1]
			break

	for line in execute_system_cmd(['vcgencmd', 'get_mem', 'arm']).strip().split('\n'):
		if 'arm' in line:
			sys_mem = line.split('=')[1]
			break

	for line in execute_system_cmd(['vcgencmd', 'measure_volts', 'core']).strip().split('\n'):
		if 'volt' in line:
			core_volt = line.split('=')[1]

	for line in execute_system_cmd(['vcgencmd', 'measure_volts', 'sdram_c']).strip().split('\n'):
		if 'volt' in line:
			sdram_volt = line.split('=')[1]

	return cpu_freq, sys_mem, core_volt, sdram_volt

def cameras_report():
	qhy_dev = execute_system_cmd([config.QHY_CAM_BIN, '-l']).strip()
	rpi_cam = execute_system_cmd(['vcgencmd', 'get_camera']).strip()

	qhy_model = '<span class="text-danger">Failed to detect</span>'

	for line in qhy_dev.split('\n'):
		if 'Model' in line:
			qhy_model = line.split(':')[1]
			break

	rpi_cam_status = 'Raspberry camera module. Supported: '

	rpi_cam_params = rpi_cam.split()

	if len(rpi_cam_params) < 2:
		rpi_cam_status += '<span class="text-danger">No</span>'
	else:
		supported = '<span class="success">Yes</span>' \
					if (rpi_cam_params[0].split('=')[1] == '1') else '<span class="text-danger">No</span>'

		detected = '<span class="success">Yes</span>' \
					if (rpi_cam_params[1].split('=')[1] == '1') else '<span class="text-danger">No</span>'

		rpi_cam_status += str(supported) + '  Detected: ' + str(detected)

	return ["Night camera:", qhy_model], ["Day camera:", rpi_cam_status]

def plot_disks_usage(output_file):
	df_full = execute_system_cmd(['df']).strip().split('\n')

	print '\nPlotting disks usage pies'

	root_metric = []
	storage_metric = []

	for line in df_full:
		if 'root' in line:
			root_metric = line.split()

		if 'storage' in line:
			storage_metric = line.split()

	labels = ['Free', 'Used']
	colors = ['yellowgreen', 'Red']

	fig, (ax1, ax2) = plt.subplots(1, 2)

	sizes = [int(root_metric[3]), int(root_metric[2])]

	ax1.pie(sizes, labels=labels, autopct='%1.1f%%', startangle=180, colors=colors)
	ax1.axis('equal')
	ax1.set_title("Root")

	red_patch = mpatches.Patch(color='red', label='Used: ' + str(int(root_metric[2]) / 1024) + ' Mbytes')
	green_patch = mpatches.Patch(color='yellowgreen', label='Free: ' + str(int(root_metric[3]) / 1024) + ' Mbytes')
	ax1.legend(handles=[red_patch, green_patch])

	sizes = [int(storage_metric[3]), int(storage_metric[2])]

	ax2.pie(sizes, labels=labels, autopct='%1.1f%%', startangle=180, colors=colors)
	ax2.axis('equal')
	ax2.set_title("Storage")

	red_patch = mpatches.Patch(color='red', label='Used: ' + str(int(storage_metric[2]) / 1024) + ' Mbytes')
	green_patch = mpatches.Patch(color='yellowgreen', label='Free: ' + str(int(storage_metric[3]) / 1024) + ' Mbytes')
	ax2.legend(handles=[red_patch, green_patch])

	fig.tight_layout(pad=2)

	plt.savefig(output_file, dpi=120)

	print 'Pies saved as ' + output_file

def build_page(template_html_file, report_html_file):
	html_template = open(template_html_file).read()

	print 'Building web page using template ' + template_html_file

	os_report_list = os_report()
	clock_voltages_report = clock_and_voltages_report()

	print 'OS report:'
	print os_report_list

	print '\nClock and voltages report:'
	print clock_voltages_report

	board_name = os_report_list[0]
	os_release = os_report_list[1]
	kernel_version = os_report_list[2]
	system_uptime = os_report_list[3]

	cpu_freq = str(clock_voltages_report[0]) + ' MHz'
	sys_mem = clock_voltages_report[1]
	core_volt = clock_voltages_report[2]
	sdram_volt = clock_voltages_report[3]

	cameras = cameras_report()

	print '\nCameras report'
	print cameras

	camera_list = ''

	for camera in cameras:
		camera_list += '<tr>'
		camera_list += '<td>' + camera[0] + '</td>' 
		camera_list += '<td>' + camera[1] + '</td>'
		camera_list += '</tr>\n'

	html_report_page = html_template.format(**locals())

	output_html = open(report_html_file, "w")
	output_html.write(html_report_page)
	output_html.close()

	print 'Report html page saved as ' + report_html_file

build_page(template_html_file=config.SYSTEM_HTML_TEMPLATE, report_html_file=config.SYSTEM_HTML_RESULT)
plot_disks_usage(output_file=config.PLOT_DISKS_USAGE)

print 'Done\n'

