#!/bin/bash

apt-get update -y 
if [ $? -eq 0 ]
then
	echo "Update done" >> ~/client.log
fi

apt-get install gcc -y
if [ $? -eq 0 ]
then
	echo "Installed gcc" >> ~/client.log
fi

apt-get install python-pip python-dev build-essential python-boto
if [ $? -eq 0 ]
then
	echo "installed python stuff" >> ~/client.log
fi

apt-get install make
if [ $? -eq 0 ]
then
	echo "Installed Make" >> ~/client.log
fi

apt-get install git -y
if [ $? -eq 0 ]
then
	echo "Installed git" >> ~/client.log
	echo "cloning git repo" >> ~/client.log
	git clone https://username:passwd@github.com/dbjnbnrj/ramsey_search.git /home/ubuntu/ramsey_search
	if [ $? -eq 0 ]
	then
		cd ramsey_search/ramsey_client
		make all
		if [ $? -eq 0 ]
		then
			echo "Code compiled successfully" >> ~/client.log
		fi
		python client_multi_threaded.py  2>&1 >> ~/client.log &
	fi
fi


