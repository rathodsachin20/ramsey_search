from __future__ import with_statement
from fabric.api import *
from fabric.contrib.console import confirm
from job import *
from fabric.contrib.files import exists
import json
import sys
from ilogue.fexpect import expect, expecting, run 
from fabric.contrib.files import exists

f = open(env.f)
host_result = json.load(f)

if "hosts" in host_result:
	env.hosts = host_result["hosts"]
if "password" in host_result:
	env.passwords = host_result["passwords"]
if "key_filename" in host_result:
	env.key_filename = host_result["key_filename"]
if "user" in host_result:
	env.user = host_result["user"]
if "ipaddress" in host_result:
	ipaddress = host_result["ipaddress"]

code_dir = host_result["code_dir"]

prompts = []
prompts += expect("Username for 'https://github.com'",'dbjnbnrj')
prompts += expect("Password for 'https://dbjnbnrj@github.com'",'')


def run_bg(cmd, before=None, sockname="dtach", use_sudo=False):
    if not exists("/usr/bin/screen"):
        sudo("apt-get install screen")
    if before:
        cmd = "{}; dtach -n `mktemp -u /tmp/{}.XXXX` {}".format(
            before, sockname, cmd)
    else:
        cmd = "dtach -n `mktemp -u /tmp/{}.XXXX` {}".format(sockname, cmd)
    if use_sudo:
        return sudo(cmd)
    else:
        return run(cmd)

def start_server():
	#run("lsof -P | grep ':8888' | awk '{print $2}' | xargs kill -9")
	with cd(code_dir):
		sudo("git pull")
	  	sudo('screen -d python server.py; sleep 1')

@parallel
def start_client():
	with cd(code_dir):
			run("make clean")
			run("make all")
			sudo('screen -d python client_2.py '+ipaddress+'; sleep 1')

def setup_client():
		sudo("apt-get install python-pip python-dev build-essential")
		sudo("apt-get install screen")
		sudo("pip install pexpect")
		if exists(code_dir):
			sudo("rm -rf "+code_dir)
		with expecting(prompts):
			sudo("git clone https://github.com/dbjnbnrj/ramsey_search.git")
		with cd(code_dir):
			sudo("make clean")

def check():
	run("ls")
