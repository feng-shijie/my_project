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

from bill_class import DB
from bill_class import Email

from itertools import chain
from email.header import Header
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart 


#/**********************变量*********************/
#是否开启打印
IS_PING = False

# #电表url		以初始化在数据库中
# electricity_url = "http://wx.tqdianbiao.com/Client/27f69m211003334646"
# #水表url
# water_url 		= "http://wx.tqdianbiao.com/Client/dc2edm30010040002526"

#充值的水费和电费
m_ele_balanc	= 0
m_water_balanc	= 0

#/**********************函数*********************/
#获取文件里面的内容
# def get_file_data(_file, _data):
# 	_fp = open(_file, 'r')

# 	_row		= _fp.readline()
# 	_isrect		= 0			#判断是否需要写入
# 	_row_val 	= 1			#当前文件的位置
# 	_data_back 	= None

# 	while _row:
# 		if _row == 0:
# 			break
# 		if _row == 1:
# 			_isrect = 1
# 			_cmd = 'sed -i \"' + _row_val + 's/.*/0/\" ' + _file
# 			print(_cmd)
# 			os.system(_cmd)
# 		if _isrect == 1:
# 			if _row:
# 				_data_back.append(_row)
# 		_row = _fp.readline()
# 		_row_val += 1

# 	_data = _data_back
# 	_fp.close()

# #初始化 所有数据以文本的形式存储在/etc/auto_bill 目录下
# def init():
# 	_dir		= "/etc/auto_bill"
# 	_file_user	= "/etc/auto_bill/email_user"
# 	_file_admin	= "/etc/auto_bill/email_admin_user"
# 	_file_url	= "/etc/auto_bill/water_electricity_url"
# 	_isGetData	= True

# 	if not os.path.exists(_dir):
# 		_isGetData = False
# 		#sh脚本执行路劲为相对路劲执行的所以要注意执行位置
# 		_cmd = '/bash/sh create.sh ' + _dir + ' ' + _file_user + ' ' + _file_admin + ' ' + _file_url
# 		if IS_PING: print(_cmd)
# 		os.system(_cmd)

# 	if _isGetData:
# 		get_file_data(_file_user, m_email_user)
# 		get_file_data(_file_admin, m_email_admin)
# 		get_file_data(_file_url, m_url)

# #添加用户
# def add_email(_usr):
# 	m_email_user.append(_usr)
# #删除用户
# def delete_email(_usr):
# 	#if !email.remove(_usr):
# 		if IS_PING:	print("not ", _usr)
#上面是从文本获取数据，以优化为从数据库获取


#/**********************费用余额*********************/
#获取水费电费余额
def get_balanc():
	l_bill_url = DB._db.execute("SELECT * FROM url")

	for curr_info in l_bill_url:
		#获取html
		res = requests.get(str(curr_info[1]))
		res.encoding = 'utf_8'

		if IS_PING:	print("type::", type(res))

		#解析?
		bs1 = BeautifulSoup(res.text,'html.parser')

		html_all = bs1.find_all('script', type='text/javascript')

		#通过正则表达式获取当前电表费用
		balanc = re.search('"value":(.*?),', str(html_all[1].string))

		if IS_PING:	print(balanc.group())

		if str(balanc).find(':')  > 0 or str(balanc).find('.') >0:
			#只要整数
			i_balance = int(str(balanc)[str(balanc).find(':') + 1 : str(balanc).rfind('.')])
			if	 str(curr_info[0]) == 'water':			Email.m_now_water 		= i_balance
			elif str(curr_info[0]) == 'electricity':	Email.m_now_electricity = i_balance

#/**********************email*********************/
#充值成功通知所有人, 每次只能通知一个人NOW不能在这里++
def email_all():
	now_user = get_now()
	if Email.m_bill_type:	_bill = now_user + "!  电费充值成功,金额为: " + str(m_ele_balanc)
	else:					_bill = now_user + "!  水费充值成功,金额为: " + str(m_water_balanc)

	#NOW++
	return _bill

#提醒NOW充值费用
def email_only():
	if Email.m_bill_type:	_bill = "电费不足,请及时缴费: " + str(Email.m_now_water)
	else:					_bill = "水费不足,请及时缴费: " + str(Email.m_now_electricity)
	return _bill

def get_now():
	sql_res = DB._db.execute("SELECT * FROM now")
	now_email = list(chain.from_iterable(list(sql_res)))
	if len(now_email):	return str(now_email[0])
	else:
		print("当前缴费用户为空")
		return str()

#发送email
def send_email():
	sql_res = DB._db.execute("SELECT * FROM admin")
	admin_info = list(chain.from_iterable(list(sql_res)))

	if len(admin_info) < 4:
		print("请补全admin邮箱账户信息")
		exit()

	sql_res = DB._db.execute("SELECT * FROM email_user")
	l_email_user = list(chain.from_iterable(list(sql_res)))

	if not bool(l_email_user):
		print("用户email为空,请添加用户")
		exit()

	admin_email  = str(admin_info[0])
	admin_name   = str(admin_info[1])
	admin_server = str(admin_info[2])
	admin_pw	 = str(admin_info[3])

	print(admin_info)
	#加密的有问题无法登录，可能是要smtp授权码等等，没找到
	# email_con = smtplib.SMTP_SSL(admin_server, Email.m_smtp_port)
	email_con = smtplib.SMTP(admin_server, Email.m_smtp_port)
	email_con.login(admin_email, admin_pw)

	email_msg = MIMEMultipart()
	if Email.m_bill_type:	email_theme = Header('电费账单', "utf-8").encode()
	else:					email_theme = Header('水费账单', "utf-8").encode()

	email_msg['Subject'] = email_theme
	# email_msg['From'] = 'f_2315@tom.com <f_2315@tom.com>'
	email_msg['From'] = admin_email

	now_user = get_now()
	if not bool(now_user):	exit()

	if(Email.m_bill_status == None):
		print("status none")
		exit()

	#需要缴费只给一个人发送email, 充值成功给所有人发丝email
	if Email.m_bill_status:
		_bill = email_all()
		for e_user in l_email_user:	email_msg['To'] = e_user[1]
	else:
		Email.m_bill_status = True
		_bill = email_only()
		email_msg['To'] = now_user

	_text = MIMEText(_bill, 'plain', 'utf-8')
	email_msg.attach(_text)
	email_con.sendmail(admin_email, now_user, email_msg.as_string())
#如果是发送给所有人应当有个循环发送,不用多次调用
	email_con.quit()
	print("发送完毕")


#/**********************main*********************/
if __name__=="__main__":
	# init()
	# get_balanc(electricity_url)
	# get_balanc(water_url)
	# send_email(m_email_user , 1)
	exit()
