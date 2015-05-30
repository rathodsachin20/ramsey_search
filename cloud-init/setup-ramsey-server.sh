#!/bin/bash

apt-get update -y 
if [ $? -eq 0 ]
then
	echo "Update done" >> ~/init.log
fi

apt-get install gcc -y
if [ $? -eq 0 ]
then
	echo "Installed gcc" >> ~/init.log
fi

apt-get install -y python-pip python-dev build-essential python-boto
if [ $? -eq 0 ]
then
	echo "installed python stuff"
fi

apt-get install make
if [ $? -eq 0 ]
then
	echo "Installed Make"
fi

apt-get install git -y
if [ $? -eq 0 ]
then
	echo "Installed git" >> ~/init.log
	echo "cloning git repo" >> ~/init.log
	git clone https://username:passwd@github.com/dbjnbnrj/ramsey_search.git
	if [ $? -eq 0 ]
	then
		cd ramsey_search/ramsey_server
		python server_3.py 2>&1 ~/server.log &
	fi
fi


