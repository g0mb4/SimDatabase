clc;
clear;
close all;

incflag1 = ['-I' fullfile('..')];
incflag2 = ['-I' fullfile('.','cjson')];

mex('sfun_simwebjson.c', 'webjson_wrapper.c', 'tcp_clnt.c', 'cjson/cJSON.c', ...
    '../common.c','../dconsole.c', '../str_buf.c', '../storage_file.c', ...
    'Ws2_32.lib', 'Mswsock.lib', 'AdvApi32.lib', incflag1, incflag2);
