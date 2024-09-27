#! /usr/bin/python3
#-*- coding:utf-8 -*-

# 线程,当通知缴费的时候,5分钟获取一次余额,当余额刷新,进入沉默模式,
#每天晚上10点和早上8点获取一次,低于10元进行通知 注::工作日为沉默模式,周末为每一小时获取一次
#


import re
import os
import smtplib
import requests
from bs4 import BeautifulSoup

from email.header import Header
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart 


#/**********************变量*********************/
#现在要进行充值的人
NOW 	= 0
#是否开启打印
IS_PING = True

#电表url
electricity_url = "http://wx.tqdianbiao.com/Client/27f69m211003334646"
#水表url
water_url 		= "http://wx.tqdianbiao.com/Client/dc2edm30010040002526"

# 0为水表url, 1为电表url
m_url 	= None

m_user	= None

#email用户 列表
m_email_url				= None
m_email_user			= None
m_email_admin			= None
m_email_authorize_code	= None

#月度总结 列表
m_count_month	= 0

#充值的水费和电费
m_ele_balanc	= 0
m_water_balanc	= 0

#/**********************函数*********************/
#获取文件里面的内容
def get_file_data(_file, _data):
	_fp = open(_file, 'r')

	_row		= _fp.readline()
	_isrect		= 0			#判断是否需要写入
	_row_val 	= 1			#当前文件的位置
	_data_back 	= None

	while _row:
		if _row == 0:
			break
		if _row == 1:
			_isrect = 1
			_cmd = 'sed -i \"' + _row_val + 's/.*/0/\" ' + _file
			print(_cmd)
			os.system(_cmd)
		if _isrect == 1:
			if _row:
				_data_back.append(_row)
		_row = _fp.readline()
		_row_val += 1

	_data = _data_back
	_fp.close()

#初始化 所有数据以文本的形式存储在/etc/auto_bill 目录下
def init():
	_dir		= "/etc/auto_bill"
	_file_user	= "/etc/auto_bill/email_user"
	_file_admin	= "/etc/auto_bill/email_admin_user"
	_file_url	= "/etc/auto_bill/water_electricity_url"
	_isGetData	= True

	if not os.path.exists(_dir):
		_isGetData = False
		#sh脚本执行路劲为相对路劲执行的所以要注意执行位置
		_cmd = '/bash/sh create.sh ' + _dir + ' ' + _file_user + ' ' + _file_admin + ' ' + _file_url
		if IS_PING: print(_cmd)
		os.system(_cmd)

	if _isGetData:
		get_file_data(_file_user, m_email_user)
		get_file_data(_file_admin, m_email_admin)
		get_file_data(_file_url, m_url)

#添加用户
def add_email(_usr):
	m_email_user.append(_usr)
#删除用户
def delete_email(_usr):
	#if !email.remove(_usr):
		if IS_PING:	print("not ", _usr)

#/**********************费用余额*********************/
#获取水费电费余额
#默认为获取电费, 0为获取水费
def get_balanc(url, ele_water = 1):
	#获取html
	res = requests.get(url)
	res.encoding = 'utf_8'

	if IS_PING:	print(type(res))

	#解析?
	bs1 = BeautifulSoup(res.text,'html.parser')

	html_all = bs1.find_all('script', type='text/javascript')

	#通过正则表达式获取当前电表费用
	balanc = re.search('"value":(.*?),', str(html_all[1].string))

	if ele_water:
		m_ele_balanc = balanc.group()
	else:
		m_water_balanc = balanc.group()
	if IS_PING:	print(balanc.group())

#/**********************email*********************/
#充值成功通知所有人, 每次只能通知一个人NOW不能在这里++
def email_all(_ele_water):
	if _ele_water:
		_bill = m_user[NOW] + "!  电费充值成功,金额为: " + m_ele_balanc
	else:
		_bill = m_user[NOW] + "!  水费充值成功,金额为: " + m_water_balanc

	#NOW++
	return _bill

#提醒NOW充值费用
def email_only(_ele_water):
	if _ele_water:
		_bill = "电费不足,请及时缴费: " + m_ele_balanc
	else:
		_bill = "水费不足,请及时缴费: " + m_water_balanc
	return _bill

#发送email
def send_email(_email, _is_ele_water, _balanc_success = 0):
	print(_email)
	email_con = smtplib.SMTP_SSL('smtp.qy.tom.com', 25)
	email_con.login(m_email_admin, m_email_authorize_code)

	email_msg = MIMEMultipart()
	if _is_ele_water:
		email_theme = Header('电费账单', "utf-8").encode()
	else:
		email_theme = Header('水费账单', "utf-8").encode()

	email_msg['Subject'] = email_theme
	email_msg['From'] = 'f_2315@tom.com <f_2315@tom.com>'

	#需要缴费只给一个人发送email
	#充值成功给所有人发丝email
	if _balanc_success:
		_bill = email_all(_is_ele_water)
		for i in _email.end():
			email_msg['To'] = _email[i]
	else:
		_bill = email_only(_is_ele_water)
		email_msg['To'] = _email[NOW]

	_text = MIMEText(_bill, 'plain', 'utf-8')
	email_msg.attach(_text)
	email_con.sendmail(m_email_admin, _email[NOW], email_con.at.string)
#如果是发送给所有人应当有个循环发送,不用多次调用
	email_con.quit()


#/**********************main*********************/
if __name__=="__main__":
	init()
	get_balanc(electricity_url)
	get_balanc(water_url, 0)
	send_email(m_email_user , 1)

