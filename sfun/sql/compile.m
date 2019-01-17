clc;
clear;
close all;

incflag1 = ['-I' fullfile('..')];
incflag2 = ['-I' fullfile('c:\Program Files\','MySQL','MySQL Connector C 6.1','include')];
libmysqlclient = fullfile('c:\Program Files\','MySQL','MySQL Connector C 6.1','lib','vs14','mysqlclient.lib');

mex('sfun_simsql.c','mysql_wrapper.c', ...
    '../common.c', '../dconsole.c', '../storage_file.c', ...
    libmysqlclient, incflag1, incflag2);
