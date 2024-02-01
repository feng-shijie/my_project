#! /bin/bash

# echo "$1"
# echo "$2"
# echo "$3"
# echo "$4"

if [ ! -e "$1" ]; then
	mkdir "$1"
	fi
if [ ! -e "$2" ]; then
	touch "$2"
	{
		echo \#数字0代表无改动,数字1代表有修改
		echo \#如增加用户,请按顺序增加,手动修改请修改数字为1
		echo 0
	} >> "$2"
	fi
if [ ! -e "$3" ]; then
	touch "$3"
	{
		echo \#该文件存储的为admin user 所有操作将通知admin
		echo \#数字0代表无改动,数字1代表有修改
		echo \#手动修改请修改数字为1
		echo 0
	} >> "$3"
	fi

if [ ! -e "$4" ]; then
	touch "$4"
	{
		echo \#如手动修改电费url在前,水费url在后
		echo \#手动修改请修改数字为1
		echo 0
	} >> "$4"
	fi
