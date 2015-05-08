from __future__ import with_statement
from fabric.api import *
from fabric.contrib.console import confirm
from job import *
from fabric.contrib.files import exists
import json
from ilogue.fexpect import expect, expecting, run 
from fabric.contrib.files import exists


def run_bg(cmd, before=None, sockname="dtach", use_sudo=False):
    """Run a command in the background using dtach

    :param cmd: The command to run
    :param output_file: The file to send all of the output to.
    :param before: The command to run before the dtach. E.g. exporting
                   environment variable
    :param sockname: The socket name to use for the temp file
    :param use_sudo: Whether or not to use sudo
    """
    if not exists("/usr/bin/dtach"):
        sudo("apt-get install dtach")
    if before:
        cmd = "{}; dtach -n `mktemp -u /tmp/{}.XXXX` {}".format(
            before, sockname, cmd)
    else:
        cmd = "dtach -n `mktemp -u /tmp/{}.XXXX` {}".format(sockname, cmd)
    if use_sudo:
        return sudo(cmd)
    else:
        return run(cmd)


host_file = open('hosts.json')
host_result = json.load(host_file)
env.hosts = host_result["hosts"].keys()
env.passwords = host_result["hosts"]

code_dir = host_result["code_dir"]
client_dir = host_result["code_dir"]+"ramsey_client"

prompts = []
prompts += expect("Username for 'https://github.com'",'dbjnbnrj')
#prompts += expect("Password for 'https://dbjnbnrj@github.com'",'')

ipaddress = "169.231.16.186"

@hosts('localhost')
def start_server():
	local("lsof -P | grep ':8888' | awk '{print $2}' | xargs kill -9")
	pid = local("python server.py & echo $!")
	print "PID: ",pid

@parallel
def start_client():
	job = {}
	run("lsof -P | grep ':8888' | awk '{print $2}' | xargs kill -9 ")
	with cd(client_dir):
			run("make clean")
			run("make anneal")
			pid = run_bg("./anneal "+ipaddress)
			print pid
			job[env.host_string] = pid
			print job

def setup_client():
		run("sudo apt-get install python-pip python-dev build-essential")
		run("sudo pip install pexpect")
		if exists(code_dir):
			run("rm -rf "+code_dir)
		with expecting(prompts):
			run("git clone https://github.com/dbjnbnrj/ramsey_search.git")
		with cd(client_dir):
			run("make clean")

def stop_client():
	run("kill -9 44751")
