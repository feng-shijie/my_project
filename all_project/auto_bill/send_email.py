#! /usr/bin/python3

import smtplib

from bill_class import DB
from bill_class import Email
from bill_class import Index


def istype(_val):
    if _val == 0:
        if Email.istype == 0:
            return Email.water_text
        return Email.electricity_text
    else:
        if Email.istype == 0:
            return Email.water_success_text
        return Email.electricity_success_text
    
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