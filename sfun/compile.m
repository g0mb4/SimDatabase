clc;
clear;
close all;

incpath = fullfile('c:\Program Files\','MySQL','MySQL Connector C 6.1','include');
incflag = ['-I' fullfile('c:\Program Files\','MySQL','MySQL Connector C 6.1','include')];
libpath = fullfile('c:\Program Files\','MySQL','MySQL Connector C 6.1','lib','vs14');
libmysqlclient = fullfile('c:\Program Files\','MySQL','MySQL Connector C 6.1','lib','vs14','mysqlclient.lib');

mex('sfun_simdatabase.c','common.c','dconsole.c','mysql_wrapper.c', libmysqlclient, incflag);
