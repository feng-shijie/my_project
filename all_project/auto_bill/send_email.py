#! /usr/bin/python3

import re
import os
import requests
import smtplib
from bs4 import BeautifulSoup

from bill_class import DB
from bill_class import Email
from bill_class import Index

IS_PING = 0

def istype(_val):
    if _val == 0:
        if Email.istype == 0:
            return Email.water_text
        return Email.electricity_text
    else:
        if Email.istype == 0:
            return Email.water_success_text
        return Email.electricity_success_text
    
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
        

def send_all(_smtpObj):
    _res = DB._db.execute(f"SELECT *FROM {DB.table_user} WHELE {DB.type_email} = {Email.now_email};")
    _result = list(_res)

    message = f'''
    {Email.subject_success}
    {_result[Index._NAME]}{istype(1)}
    {Email.body_success} {Email.balance}
    '''
    _res = DB._db.execute(f"SELECT *FROM {DB.table_user};")

    for _result in _res:
        Email.receiver_email.append(_result[Index._EMAIL])

    for receiver_email in Email.receiver_email:
        _smtpObj.sendmail(Email.sender_email, receiver_email, message)

def send_now(_smtpObj):
    message = f'''
    {Email.subject}
    {istype(0)}
    {Email.body}
    '''
    _res = DB._db.execute(f"SELECT *FROM {DB.table_user} WHELE {DB.type_email} = {Email.now_email};")
    if not len(list(_res)):
        print("没有要接收的用户")
        exit()

    _result = list(_res)
    Email.receiver_email.append(_result[Index._EMAIL])
    _smtpObj.sendmail(Email.sender_email, Email.receiver_email, message)


if __name__ == '__main__':
    DB.init()
    _res = DB._db.execute(f"SELECT *FROM {DB.table_admin};")
    if not len(list(_res)):
        print("没有admin用户")
        exit()

    _result = list(_res)
    Email.sender_email      = _result[Index._EMAIL]
    Email.smtp_server       = _result[Index._SERVER]
    Email.sender_password   = _result[Index._PASSWORD]

    smtpObj = smtplib.SMTP(Email.smtp_server, Email.smtp_port)
    smtpObj.login(Email.sender_email, Email.sender_password)

    if Email.balance > 10:
        send_all(smtpObj)
    else:
        send_now(smtpObj)

    smtpObj.quit()