clear; close all; clc;
%% Create Serial Object
s = serial('COM3');
set(s,'DataBits',8);
set(s,'StopBits',1);
set(s,'Parity','none');
fopen(s);

%% Wait for communication link
%call and response
a = 'b';
while a~='a'
    a=fread(s,1,'uchar');   %wait for call from arduino
end
disp('Serial Link Created')
fprintf(s,'%c','a');        %send response

%% begin reading data;
disp('Reading Data')
samples = 100;
data = zeros(9,samples);
fread(s,[1,2]);
for i = 1:samples;
data(1:9,i)=fread(s,[1,9],'single');
fprintf('.');
end
fprintf('\n');
%% Terminate connection
fclose all;
delete(instrfindall);
disp('Serial Link Closed')

%% Play with data
x = 1:samples;
figure('units','normalized','position',[.1,.25,.8,.6],'menubar','none');
plot(x,data(1,:),x,data(2,:),x,data(3,:))
title Accelerometer

figure('units','normalized','position',[.1,.2,.8,.6],'menubar','none');
plot(x,data(4,:),x,data(5,:),x,data(6,:))
title Gyroscope

figure('units','normalized','position',[.1,.15,.8,.6],'menubar','none');
plot(x,data(7,:),x,data(8,:),x,data(9,:))
title Magnetometer