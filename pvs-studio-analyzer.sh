#!/bin/bash
pvs-studio-analyzer trace -o /tmp/strace_out make;
pvs-studio-analyzer analyze -f /tmp/strace_out -l /home/user/.config/PVS-Studio/PVS-Studio.lic -o /tmp/PVS-Studio.log;
plog-converter -a GA:1,2 -t tasklist -o /tmp/report.tasks /tmp/PVS-Studio.log;
vim /tmp/report.tasks
