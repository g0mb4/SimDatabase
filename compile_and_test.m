compiledll = true;
uselocal = false;
install = true;

interpolate = 1;

incpath = fullfile('c:\Program Files\','MySQL','MySQL Connector C 6.1','include');
incflag = ['-I' fullfile('c:\Program Files\','MySQL','MySQL Connector C 6.1','include')];
libpath = fullfile('c:\Program Files\','MySQL','MySQL Connector C 6.1','lib','vs14');
libmysqlclient = fullfile('c:\Program Files\','MySQL','MySQL Connector C 6.1','lib','vs14','mysqlclient.lib');
addpath(incpath)

if compiledll
    mex('LDEXT=.dll','LINKEXPORT=','LINKEXPORTVER=', incflag, 'simdatabase.c','common.c','dconsole.c','mysql_wrapper.c', 'local_database.c', libmysqlclient);
end

% Load the library
[~,~] = loadlibrary(['simdatabase',system_dependent('GetSharedLibExt')],'simdatabase.h', 'includepath', incpath);
% Display functions available in the library
%libfunctionsview('exmysql');

calllib('simdatabase','simdatabase_init');

vals = zeros(1, 10);
valPtr = libpointer('doublePtr', vals);

if uselocal == false
    for t = 0:4000:16000
        calllib('simdatabase','simdatabase_step_mysql','127.0.0.1', 3306, 'root', '', 'sim_test', 'data', '2018-04-17','06:00:00', '2018-04-17', '20:30:00', 'TW,I', t, interpolate, valPtr);
        disp(valPtr.Value(1:2));
    end
else
    for t = 0:4000:16000
        calllib('simdatabase','simdatabase_step_local', 'local.csv', '2018-04-17','06:00:00', '2018-04-17', '10:00:00', 'TW,I', t, interpolate, valPtr);
        disp(valPtr.Value(1:2));
    end
end

% Terminate
calllib('simdatabase','simdatabase_term');

% Unload the library
unloadlibrary('simdatabase');

if install == true
    copyfile simdatabase.dll ..\simdatabase.dll
    copyfile resolve_pointer.m ..\resolve_pointer.m
end


