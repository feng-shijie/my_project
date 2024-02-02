#/*****************线程函数*****************/
#!/usr/bin/python3
# -*- coding: utf-8 -*-  

from datetime import datetime
from itertools import chain
import os
import sys
import time
import threading

import create
import water
import send_email
from bill_class import DB
from bill_class import Cmd
from bill_class import Index
from bill_class import Email
from bill_class import Check_Time


def edit_config():
    Email.m_bill_status     = False
    Email.m_old_water       = Email.m_now_water
    Email.m_old_electricity = Email.m_now_electricity

    if Email.m_now_electricity < Email.m_min_tip:    Email.m_bill_type   = True
    if Email.m_now_water       < Email.m_min_tip:    Email.m_bill_type   = False

# #/*****************检查费用余额*****************/
def check_balance():
    while True:
        _res = os.system("ping -c 1 www.baidu.com")
        if _res == 0:   break
        time.sleep(60)

# #获取账单低于10元发送email
# 修改一下，只有一个url也可以
    water.get_balanc()
    if Email.m_now_water == None or Email.m_now_electricity == None:    return

    Email.m_bill_status = False
    Email.m_bill_type   = False
    water.send_email()
    exit()

    if Email.m_now_electricity< Email.m_min_tip or Email.m_now_water < Email.m_min_tip:
        edit_config()

        while True:
            _res = os.system("ping -c 1 www.baidu.com")
            if _res == 0:
                water.send_email()
                return
            else:   time.sleep(60)

#/*****************时间线程*****************/
def time_thread(_temp):
    check_balance()

    while True:
        i_now_hour = datetime.now().hour
        #判断是否是白天
        if  i_now_hour > Check_Time.m_morning or i_now_hour < Check_Time.m_night:   time.sleep(7200)
        else:
            check_balance()
            time.sleep(60 * 30)

#/*****************交互线程*****************/
def check_email(_table, _email):
    _cmd = "SELECT *FROM %s WHERE %s = '%s';" %(_table, DB.type_email, _email)
    _res = DB._db.execute(_cmd)
    if len(list(_res)) == 0:
        return 0
    return 1

def result(_table, _email):
    DB.m_db.commit()
    _cmd = "SELECT *FROM %s WHERE %s = '%s';" %(_table, DB.type_email, _email)
    _res = DB._db.execute(_cmd)

    print(list(chain.from_iterable(list(_res))))

#0 为普通用户， 1 为管理员用户
def add_user(_list, _is_special):
    if _is_special == 0:
        if check_email(DB.table_user,_list[1]):
            print("用户已存在, 请使用edit命令来修改用户")
            return
        _cmd = "INSERT INTO %s(%s,%s) VALUES('%s', '%s');" \
        %(DB.table_user, DB.type_email, DB.type_name, _list[1], _list[2])

        DB._db.execute(_cmd)
        result(DB.table_user, _list[1])
    else:
        _result = DB._db.execute(f"SELECT *FROM {DB.table_admin};")
        _result = DB._db.execute(f"SELECT *FROM {DB.table_admin};")
        _res = list(chain.from_iterable(list(_result)))
        if bool(_res):
            print("admin用户已存在, 请使用eadmin命令来修改admin用户")
            return
        else:
            _cmd = "INSERT INTO %s(%s,%s,%s,%s) VALUES('%s', '%s', '%s', '%s');" \
            %(DB.table_admin, DB.type_email, DB.type_name, DB.type_server, DB.type_password, _list[1], _list[2], _list[3], _list[4])
            # _cmd = f"INSERT INTO {DB.table_admin}({DB.type_email},{DB.type_name},{DB.type_server}) VALUES('{_list[1]}', '{_list[2]}', '{_list[3]}');"
            
            DB._db.execute(_cmd)
            result(DB.table_admin, _list[1])
    print("用户已添加")

def setnow(_list):
    _result = DB._db.execute(f"SELECT *FROM {DB.table_user} WHERE {DB.type_id} = {_list[1]};")
    _res = list(chain.from_iterable(list(_result)))

    if len(_res) == 0:
        print(f"没有ID为::{_list[1]} 的用户")
        return

    DB._db.execute(f"INSERT INTO {DB.table_now} VALUES({_res[Index._ID]});")
    DB.m_db.commit()
    print(f"now = {_res[Index._ID]} 设置成功")

def seturl(_list):
    if not (_list[1] == "water" or _list[1] == "electricity"):
        print("name 错误 水费为:water 电费为:electricity")
        return
    DB._db.execute(f"UPDATE {DB.table_url} SET {DB.type_url} = '{_list[2]}' WHERE {DB.type_name} = '{_list[1]}';")
    DB.m_db.commit()
    print(f"name = {_list[1]} url = {_list[2]} 修改成功")


def remove_user(_list):
    if not check_email(DB.table_user,_list[1]):
        print("用户不存在, 请使用add命令来添加用户")
    else:
        result(DB.table_user, _list[1])
        _cmd = "DELETE FROM %s WHERE %s = '%s' AND %s = '%s';" \
        %(DB.table_user, DB.type_email,_list[1], DB.type_name, _list[2])
        
        DB._db.execute(_cmd)
        DB.m_db.commit()
        if check_email(DB.table_user,_list[1]):
            print("DELETE error")
        else:
            print("用户已删除")

#0为普通用户，1为admin用户
def edit_user(_list, _is_special):
    if not _is_special:
        if not check_email(DB.table_user,_list[1]):
            print("用户不存在, 请使用add命令来添加用户")
            return
        _cmd = "UPDATE %s SET %s = '%s' AND %s = '%s' WHERE %s = '%s' AND %s = '%s';" \
        %(DB.table_user, DB.type_email, _list[1], DB.type_name, _list[2], DB.type_email, _list[1], DB.type_name, _list[2])
        
        DB._db.execute(_cmd)
        result(DB.table_user, _list[1])
    else:
        if not check_email(DB.table_user,_list[1]):
            print("admin用户不存在, 请使用admin命令来添加admin用户")
            return
        _cmd = "UPDATE %s SET %s = '%s' AND %s = '%s' WHERE %s = '%s' AND %s = '%s';" \
        %(DB.table_admin, DB.type_email, _list[1], DB.type_name, _list[2], DB.type_email, _list[1], DB.type_name, _list[2])
        
        DB._db.execute(_cmd)
        result(DB.table_admin, _list[1])
    print("用户已修改")


def select_user(_list):
    _cmd = "SELECT *FROM %s WHERE %s = '%s' OR %s = '%s';" \
    %(DB.table_user, DB.type_email, _list[1], DB.type_name, _list[1])
    
    _result = DB._db.execute(_cmd)
    _res = list(chain.from_iterable(list(_result)))     #把多维list解为一层list
    if bool(_res):
        print("ID = ", _res[0], " email = ", _res[1], " name = ", _res[2])
    else:
        print("用户不存在")

def getall_user():
    _cmd = "SELECT *FROM %s;" %(DB.table_user)
    _result = DB._db.execute(_cmd)
    if bool(_result):
        for _val in _result:
            print("ID = ", _val[0], " email = ", _val[1], " name = ", _val[2])
    else:
        print("没有用户, 请使用add命令提那加用户")

def getadmin_user():
    _cmd = f"SELECT *FROM {DB.table_admin};"
    _result = DB._db.execute(_cmd)
    _res = list(chain.from_iterable(list(_result)))
    if bool(_res):
        print("email = ", _res[0], " name = ", _res[1], " server = ", _res[2], " password = ", _res[3])
    else:
        print("没有admin用户, 请使用add命令添加admin用户")

def get_balance():
    print("%s")
    print("%s")

def quit():
    DB._db.close()
    DB.m_db.close()
    exit()

def help(_dic = None):
    _result = DB._db.execute("SELECT *FROM help;")
    _mat = "{0:<10}\t{1:<35}\t{2:<10}"
    print(_mat.format("命令", "参数", "说明"))
    i = 0
    for _str in _result:
        _case_list = _str[1].split('|')
        if not _dic == None:
            _dic[_case_list[0]] = i   #ID key
            # _dic[i] = _case_list[0]     # name key
            i+=1
        print(_mat.format(_case_list[0], _case_list[1], _case_list[2]))
        # print("{0:<10}\t{1:<10}\t{2:<10}" .format(_case_list[0], _case_list[1], _case_list[2]))

def execute_cmd(_dic, _list):
    _max = 5
    _min = 0
    _len = len(_list)
    _val = _dic.get(_list[0])
    if _len > _max or _len == _min or _val == None:
        print("命令格式错误，请重试！")
        return

    if _val == Cmd._ADD:
        if _len == 3:
            add_user(_list, 0)
        else:
            print("参数错误，请重试")
    elif _val == Cmd._ADMIN:
        if _len == 5:
            add_user(_list, 1)
        else:
            print("参数错误，请重试")
    elif _val == Cmd._SETNOW:
        if _len == 2:
            setnow(_list)
        else:
            print("参数错误,请重试")
    elif _val == Cmd._SETURL:
        if _len == 3:
            seturl(_list)
        else:
            print("参数错误,请重试")
    elif _val == Cmd._REMOVE:
        if _len == 3:
            remove_user(_list)
        else:
            print("参数错误，请重试")
    elif _val == Cmd._EDIT:
        if _len == 3:
            edit_user(_list, 0)
        else:
            print("参数错误，请重试")
    elif _val == Cmd._EADMIN:
        if _len == 3:
            edit_user(_list, 1)
        else:
            print("参数错误，请重试")
    elif _val == Cmd._SELECT:
        if _len == 2:
            select_user(_list)
        else:
            print("参数错误，请重试")
    elif _val == Cmd._GETALL:
        if _len == 1:
            getall_user()
        else:
            print("命令格式错误，请重试")
    elif _val == Cmd._GETADMIN:
        if _len == 1:
            getadmin_user()
        else:
            print("命令格式错误，请重试")
    elif _val == Cmd._GETBILL:
        if _len == 1:
            get_balance()
        else:
            print("命令格式错误，请重试")
    elif _val == Cmd._QUIT:
        if _len == 1:
            quit()
        else:
            print("命令格式错误，请重试")
    elif _val == Cmd._HELP:
        if _len == 1:
            help()
        else:
            print("命令格式错误，请重试")
    else:
        print("没有该命令，请重试")


def reply_thread(_temp):
    # _cmd        = []
    _case_list  = []
    _dic        = {}
    help(_dic)
    
    while True:
        _case_list.clear()
        _cmd = input("请输入：")
        _cmd = _cmd.strip()
        # _case_list.append(_cmd.split(' '))    #append是把返回的列表添加到列表里面，所以它的type是list[list[]] 而不是 list["str"]
        _case_list = _cmd.split(' ')
        execute_cmd(_dic, _case_list)

if __name__ == '__main__':
    th_index = 0
    create.create_db()
    DB.init()

    if len(sys.argv) > 2:
        print("format error ")
        sys.exit()
    elif len(sys.argv) == 2:
        if not sys.argv[1] == "back":
            print("parameter error 请输入 back")
            sys.exit()
        th_index += 1
        _reply = threading.Thread(target = reply_thread, args = (th_index,))
        _reply.start()
        _reply.join()
    else:
        th_index += 1
        _time = threading.Thread(target = time_thread, args = (th_index,))
        _time.start()
        _time.join()