#!/usr/bin/python3

import sqlite3
import sys
from enum import Enum

class DB:
    m_db            = None
    _db             = None

    table_user      = "email_user"
    table_admin     = "admin"
    table_help      = "help"
    table_url       = "url"
    table_now       = "now"

    type_id         = "id"
    type_url        = "url"
    type_email      = "email"
    type_name       = "name"
    type_server     = "server"
    type_password   = "password"
    def init():
        DB.m_db     = sqlite3.connect(f"{sys.path[0]}/db/user.db", check_same_thread=False)
        DB._db      = DB.m_db.cursor()

class Email:
    NetWork_start   = True      #网络状态
    m_morning       = 9         #白天
    m_night         = 18        #晚上

    m_bill_type     = None      #true:电    false:水
    m_bill_status   = None      #true:充值成功  false:提示充值

    m_min_tip       = 10        #最低余额提示
    m_smtp_port     = 25        #25为不加密， 加密465 有问题无法登录

    #info
    subject          = "!!!账单!!!"
    body             = "请及时缴费"
    water_text       = f"水费不足 {m_min_tip}元"
    electricity_text = f"电费不足 {m_min_tip}元"

    subject_success          = "!!!恭喜!!!"
    body_success             = "余额为：： "
    water_success_text       = " 水费充值成功"
    electricity_success_text = " 电费充值成功"

    #电费，水费，金额
    m_now_water           = None
    m_now_electricity     = None

    m_old_water           = None
    m_old_electricity     = None

class Cmd:
    _ADD         = 0
    _ADMIN       = 1
    _SETNOW      = 2
    _SETURL      = 3
    _EDIT        = 4
    _EADMIN      = 5
    _REMOVE      = 6
    _SELECT      = 7
    _GETALL      = 8
    _GETADMIN    = 9
    _GETBILL     = 10
    _QUIT        = 11
    _HELP        = 12

class Index:
    _EMAIL       = 0
    _NAME        = 1
    _SERVER      = 2
    _PASSWORD    = 3
