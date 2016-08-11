
md = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.win.4\console\md", -1, 9);

t = md(:,1);

rr = md(:,2);
ri = md(:,3);
ra = md(:,4);
rp = md(:,5);

ir = md(:,6);
ii = md(:,7);
ia = md(:,8);
ip = md(:,9);






clf;

plot(t, ra,'b');
plot(t, ia,'k');

plot(t, rp,'m');
plot(t, ip,'r');



