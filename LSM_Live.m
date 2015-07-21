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
fread(s,[1,2]);

plots = gobjects(9,1);
graphs = gobjects(9,1);
figure('units','normalized','position',[.05,.2,.9,.6])
for i = 1:9
    plots(i) = subplot(1,9,i);
    graphs(i) = plot(0,0);
    set(graphs(i),'marker','o','markersize',5,'linewidth',5,'color','r')
end
axis(plots(1:3),[-1,1,-3,3]);
axis(plots(4:6),[-1,1,-300,300]);
axis(plots(7:9),[-1,1,-3,3]);
xlabel(plots(1),'x accel');
xlabel(plots(2),'y accel');
xlabel(plots(3),'z accel');
xlabel(plots(4),'x gyro');
xlabel(plots(5),'y gyro');
xlabel(plots(6),'z gyro');
xlabel(plots(7),'x mag');
xlabel(plots(8),'y mag');
xlabel(plots(9),'z mag');

H = msgbox('Press OK to stop reading.');
set(H,'units','normalized','Position',[.45,.85,.1,.08]);

while ishandle(H)
    data(1:9,1) = fread(s,[1,9],'single');
    for i = 1:9
        set(graphs(i),'Ydata',data(i));
    end
    drawnow
end

%% Terminate connection
fclose all;
delete(instrfindall);
disp('Serial Link Closed')
close all;